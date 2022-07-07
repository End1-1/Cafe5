#include "cr5consumptionbysales.h"
#include "cr5consumptionbysalesfilter.h"
#include "c5gridgilter.h"
#include "cr5consuptionreason.h"
#include "c5tablemodel.h"
#include "c5mainwindow.h"
#include "c5waiterorderdoc.h"
#include "c5progressdialog.h"
#include "c5user.h"
#include "c5double.h"
#include "dlgchangeoutputstore.h"
#include "c5storedraftwriter.h"
#include "c5storedoc.h"
#include "cr5goodsmovement.h"
#include <QInputDialog>

static const int col_goodsid = 0;
static const int col_goodsgroup = 1;
static const int col_goodsname = 2;
static const int col_goodscode = 3;
static const int col_qtybefore = 4;
static const int col_qtyinput = 5;
static const int col_qtysale = 6;
static const int col_qtyout = 7;
static const int col_qtystore = 8;
static const int col_qtyafter = 9;
static const int col_qtyinv = 10;
static const int col_qtydiff = 11;
static const int col_qtyunit = 12;

CR5ConsumptionBySales::CR5ConsumptionBySales(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/goods.png";
    fLabel = tr("Consumption of goods base on sales");

    fColumnNameIndex["f_goodsid"] = col_goodsid;
    fColumnNameIndex["f_goodsgroup"] = col_goodsgroup;
    fColumnNameIndex["f_goodsname"] = col_goodsname;
    fColumnNameIndex["f_goodscode"] = col_goodscode;
    fColumnNameIndex["f_qtybefore"] = col_qtybefore;
    fColumnNameIndex["f_qtyinput"] = col_qtyinput;
    fColumnNameIndex["f_qtysale"] = col_qtysale;
    fColumnNameIndex["f_qtyout"] = col_qtyout;
    fColumnNameIndex["f_qtystore"] = col_qtystore;
    fColumnNameIndex["f_qtyafter"] = col_qtyafter;
    fColumnNameIndex["f_qtyinv"] = col_qtyinv;
    fColumnNameIndex["f_qtydiff"] = col_qtydiff;
    fColumnNameIndex["f_qtyunit"] = col_qtyunit;

    fTranslation["f_goodsid"] = tr("Goods code");
    fTranslation["f_goodsgroup"] = tr("Group");
    fTranslation["f_goodsname"] = tr("Goods name");
    fTranslation["f_goodscode"] = tr("Scancode");
    fTranslation["f_qtybefore"] = tr("Before");
    fTranslation["f_qtyinput"] = tr("Input");
    fTranslation["f_qtysale"] = tr("Sale");
    fTranslation["f_qtyout"] = tr("Out");
    fTranslation["f_qtystore"] = tr("Store");
    fTranslation["f_qtyafter"] = tr("After");
    fTranslation["f_qtyinv"] = tr("Inventory");
    fTranslation["f_qtydiff"] = tr("Diff");
    fTranslation["f_qtyunit"] = tr("Unit");

    fColumnsSum << "f_qtybefore"
                << "f_qtyinput"
                << "f_qtysale"
                << "f_qtyout"
                << "f_qtystore"
                << "f_qtyafter"
                << "f_qtyinv"
                << "f_qtydiff";

    fFilterWidget = new CR5ConsumptionBySalesFilter(dbParams);
    fFilter = static_cast<CR5ConsumptionBySalesFilter*>(fFilterWidget);
}

QToolBar *CR5ConsumptionBySales::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
            btn << ToolBarButtons::tbFilter
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        fToolBar = createStandartToolbar(btn);
        QAction *a = new QAction(QIcon(":/goods.png"), tr("Make\ninput/output"), this);
        connect(a, SIGNAL(triggered(bool)), this, SLOT(makeOutput(bool)));
        fToolBar->insertAction(fToolBar->actions().at(0), a);
        auto *b = new QAction(QIcon(":/calculator.png"), tr("Count output\nbase on recipes"), this);
        connect(b, SIGNAL(triggered(bool)), this, SLOT(countOutputBasedOnRecipes()));
        if (__c5config.frontDeskMode() == FRONTDESK_WAITER) {
            fToolBar->insertAction(a, b);
        }
        auto *d = new QAction(QIcon(":/decision.png"), tr("Change the store of output"), this);
        connect(d, SIGNAL(triggered(bool)), this, SLOT(changeOutputStore()));
        fToolBar->insertAction(b, d);
        auto *c = new QAction(QIcon(":/goods.png"), tr("Sales\noutput"), this);
        connect(c, SIGNAL(triggered(bool)), this, SLOT(salesOutput(bool)));
        fToolBar->insertAction(a, c);
        auto *e = new QAction(QIcon(":/goodsback.png"), tr("Rollback goods output"), this);
        connect(e, SIGNAL(triggered(bool)), this, SLOT(rollbackGoodsOutput(bool)));
        fToolBar->insertAction(c, e);

        auto *f = new QAction(QIcon(":/goodsback.png"), tr("Get from AS"), this);
        connect(f, SIGNAL(triggered(bool)), this, SLOT(asinput(bool)));
        fToolBar->insertAction(e, f);
        auto *g = new QAction(QIcon(":/goodsback.png"), tr("Output to AS"), this);
        connect(g, SIGNAL(triggered(bool)), this, SLOT(asoutput(bool)));
        fToolBar->insertAction(e, g);

        auto *h = new QAction(QIcon(":/update_prices.png"), tr("Update prices\nof inventorization"), this);
        connect(h, SIGNAL(triggered(bool)), this, SLOT(updateInventorizatinPrices()));
        fToolBar->addAction(h);
    }
    return fToolBar;
}

void CR5ConsumptionBySales::buildQuery()
{
    CR5ConsumptionBySalesFilter *f = static_cast<CR5ConsumptionBySalesFilter*>(fFilterWidget);
    if (f->store() ==  0) {
        if (!C5GridGilter::filter(f, fWhereCondition, fColumnsVisible, fTranslation)) {
            return;
        }
    }
    if (f->store() == 0) {
        C5Message::info(tr("Store must be defined"));
        return;
    }

    QList<QList<QVariant> > &rows = fModel->fRawData;
    rows.clear();
    QMap<int, int> goodsMap;
    QString cond = " where c.f_id>0 ";
    if (fFilter->group().length() > 0) {
        cond += " and c.f_group in (" + fFilter->group() + ") ";
    }
    if (fFilter->class1().length() > 0) {
        cond += " and c.f_group1 in (" + fFilter->class1() + ") ";
    }
    if (fFilter->class2().length() > 0) {
        cond += " and c.f_group2 in (" + fFilter->class2() + ") ";
    }
    if (fFilter->class3().length() > 0) {
        cond += " and c.f_group3 in (" + fFilter->class3() + ") ";
    }
    if (fFilter->class4().length() > 0) {
        cond += " and c.f_group4 in (" + fFilter->class4() + ") ";
    }
    C5Database db(fDBParams);
    /* get all goods */
    db.exec(QString("select c.f_id, g.f_name as f_groupname, c.f_name, c.f_scancode, "
            "u.f_name as f_unitname "
            "from c_goods c "
            "left join c_groups g on g.f_id=c.f_group "
            "left join c_units u on u.f_id=c.f_unit %1 "
            "order by 2, 3").arg(cond));
    while (db.nextRow()) {
    QList<QVariant> row;
        row << db.getInt("f_id")
            << db.getString("f_groupname")
            << db.getString("f_name")
            << db.getString("f_scancode")
            << QVariant()
            << QVariant()
            << QVariant()
            << QVariant()
            << QVariant()
            << QVariant()
            << QVariant()
            << QVariant()
            << db.getString("f_unitname");
        rows.append(row);
        goodsMap[db.getInt("f_id")] = rows.count() - 1;
    }
    /* get qty before */
    db[":f_store"] = f->store();
    db[":f_date"] = f->date1().addDays(-1);
    db.exec(QString("select s.f_goods, sum(%1*s.f_type) as f_qty "
                "from a_store s "
                "inner join a_header d on d.f_id=s.f_document "
                "where s.f_store=:f_store and d.f_date<=:f_date "
                "group by 1 "
                "having sum(%1*s.f_type) > 0.001")
            .arg(fFilter->reportType() == REPORTTYPE_AMOUNTS ? "s.f_total" : "s.f_qty"));
    while (db.nextRow()) {
        if (!goodsMap.contains(db.getInt(0))) {
            continue;
        }
        int r = goodsMap[db.getInt(0)];
        rows[r][col_qtybefore] = db.getDouble("f_qty");
    }
    /* get input */
    db[":f_store"] = f->store();
    db[":f_date1"] = f->date1();
    db[":f_date2"] = f->date2();
    db[":f_type"] = 1;
    db[":f_store"] = f->store();
    db[":f_state"] = DOC_STATE_SAVED;
    db.exec(QString("select s.f_goods, sum(%1) "
            "from a_store s "
            "left join a_header h on h.f_id=s.f_document "
            "where h.f_date between :f_date1 and :f_date2 and s.f_store=:f_store "
            "and h.f_state=:f_state and s.f_type=:f_type "
            "group by 1 ")
            .arg(fFilter->reportType() == REPORTTYPE_AMOUNTS ? "s.f_total" : "s.f_qty"));
    while (db.nextRow()) {
        if (!goodsMap.contains(db.getInt(0))) {
            continue;
        }
        int r = goodsMap[db.getInt(0)];
        rows[r][col_qtyinput] = db.getDouble(1);
    }
    /* get output based on recipes */
    QString cond1;
    if (!fFilter->draft()) {
        db[":f_state"] = DOC_STATE_SAVED;
        cond1 += " and h.f_state=:f_state ";
    }
    db[":f_store"] = f->store();
    db[":f_date1"] = f->date1();
    db[":f_date2"] = f->date2();
    db[":f_type"] = DOC_TYPE_STORE_OUTPUT;
    db[":f_reason"] = DOC_REASON_SALE;
    db.exec(QString("select s.f_goods, sum(%1) as f_qty "
            "from a_store_draft s "
            "inner join a_header h on h.f_id=s.f_document "
            "where h.f_date between :f_date1 and :f_date2 and s.f_store=:f_store "
            "and s.f_reason=:f_reason and h.f_type=:f_type " + cond1 +
            "group by 1 ")
            .arg(fFilter->reportType() == REPORTTYPE_AMOUNTS ? "s.f_total" : "s.f_qty"));
    while (db.nextRow()) {
        if (!goodsMap.contains(db.getInt(0))) {
            continue;
        }
        int r = goodsMap[db.getInt(0)];
        rows[r][col_qtysale] = db.getDouble("f_qty");
    }
    /* get output */
    db[":f_store"] = f->store();
    db[":f_date1"] = f->date1();
    db[":f_date2"] = f->date2();
    db[":f_type"] = -1;
    db[":f_state"] = DOC_STATE_SAVED;
    db[":f_reason"] = DOC_REASON_SALE;
    db.exec(QString("select s.f_goods, sum(%1) as f_qty "
            "from a_store s "
            "left join a_header h on h.f_id=s.f_document "
            "where h.f_date between :f_date1 and :f_date2 and s.f_store=:f_store "
            "and s.f_type=:f_type and h.f_state=:f_state  "
            "and s.f_reason<>:f_reason "
            "group by 1 ")
            .arg(fFilter->reportType() == REPORTTYPE_AMOUNTS ? "s.f_total" : "s.f_qty"));
    while (db.nextRow()) {
        if (!goodsMap.contains(db.getInt(0))) {
            continue;
        }
        int r = goodsMap[db.getInt(0)];
        rows[r][col_qtyout] = db.getDouble(1);
    }
    /* get store at the date end */
    db[":f_store"] = f->store();
    db[":f_date"] = f->date2();
    db.exec(QString("select s.f_goods, sum(%1*s.f_type) as f_qty "
                "from a_store s "
                "inner join a_header d on d.f_id=s.f_document "
                "where s.f_store=:f_store and d.f_date<=:f_date "
                "group by 1 "
                "having sum(%1*s.f_type) > 0.001")
            .arg(fFilter->reportType() == REPORTTYPE_AMOUNTS ? "s.f_total" : "s.f_qty"));
    while (db.nextRow()) {
        if (!goodsMap.contains(db.getInt(0))) {
            continue;
        }
        int r = goodsMap[db.getInt(0)];
        rows[r][col_qtystore] = db.getDouble("f_qty");
    }
    /* get counted qty */
    for (int i = 0; i < rows.count(); i++) {
        rows[i][col_qtyafter] = rows[i][col_qtybefore].toDouble()
                + rows[i][col_qtyinput].toDouble()
                - rows[i][col_qtysale].toDouble()
                - rows[i][col_qtyout].toDouble();
    }
    /* get inventory quantities */
    db[":f_date"] = f->date2();
    db[":f_store"] = f->store();
    db[":f_type"] = DOC_TYPE_STORE_INVENTORY;
    db.exec(QString("select i.f_goods, g.f_name, sum(%1) as f_qty "
            "from a_store_inventory i "
            "inner join a_header h on h.f_id=i.f_document "
            "inner join c_goods g on g.f_id=i.f_goods "
            "where h.f_date=:f_date and h.f_type=:f_type "
            "and i.f_store=:f_store "
            "group by 1, 2 ")
            .arg(fFilter->reportType() == REPORTTYPE_AMOUNTS ? "i.f_total" : "i.f_qty"));
    while (db.nextRow()) {
        if (!goodsMap.contains(db.getInt(0))) {
            continue;
        }
        int r = goodsMap[db.getInt(0)];
        rows[r][col_qtyinv] = db.getDouble("f_qty");
    }
    //remove unused
    for (int i  = rows.count() - 1; i > -1; i--) {
        if (zerodouble(rows[i][col_qtybefore].toDouble())
                && zerodouble(rows[i][col_qtysale].toDouble())
                && zerodouble(rows[i][col_qtyout].toDouble())
                && zerodouble(rows[i][col_qtystore].toDouble())
                && zerodouble(rows[i][col_qtyafter].toDouble())
                && zerodouble(rows[i][col_qtyinv].toDouble())
                && zerodouble(rows[i][col_qtydiff].toDouble())) {
            rows.removeAt(i);
        }
    }
    fModel->setExternalData(fColumnNameIndex, fTranslation);
    for (int i  = rows.count() - 1; i > -1; i--) {
        countRowQty(i);
    }
    fColumnsVisible["f_goodsid"] = true;
    fColumnsVisible["f_goodsgroup"] = true;
    fColumnsVisible["f_goodsname"] = true;
    fColumnsVisible["f_qtybefore"] = true;
    fColumnsVisible["f_qtyinput"] = true;
    fColumnsVisible["f_qtysale"] = true;
    fColumnsVisible["f_qtyout"] = true;
    fColumnsVisible["f_qtystore"] = true;
    fColumnsVisible["f_qtyafter"] = true;
    fColumnsVisible["f_qtyinv"] = true;
    fColumnsVisible["f_qtydiff"] = true;
    restoreColumnsWidths();
    sumColumnsData();
    emit refreshed();
}

void CR5ConsumptionBySales::refreshData()
{
    buildQuery();
}

QString CR5ConsumptionBySales::documentForInventory()
{
    QString result;
    CR5ConsumptionBySalesFilter *f = static_cast<CR5ConsumptionBySalesFilter*>(fFilterWidget);
    C5Database db(fDBParams);
    db[":f_date"] = f->date2();
    db[":f_type"] = DOC_TYPE_STORE_INVENTORY;
    db.exec("select f_id from a_header where f_date=:f_date and f_type=:f_type");
    while (db.nextRow()) {
        C5Database db2(fDBParams);
        db2[":f_document"] = db.getString(0);
        db2.exec("select * from a_store_inventory where f_document=:f_document");
        if (db2.nextRow()) {
            if (!result.isEmpty()) {
                result += ",";
            }
            result += QString("'%1'").arg(db.getString(0));
        }
    }
    if (result.isEmpty()) {
        C5StoreDraftWriter dw(db);
        dw.writeAHeader(result, QString::number(dw.counterAType(DOC_TYPE_STORE_INVENTORY)), DOC_STATE_SAVED, DOC_TYPE_STORE_INVENTORY, __user->id(), f->date2(), QDate::currentDate(), QTime::currentTime(), 0, 0, tr("Created automaticaly"));
    }
    return result;
}

void CR5ConsumptionBySales::countRowQty(int row)
{
    double qtySale = 0.0;
    if (!fFilter->salesCounted()) {
        qtySale = fModel->data(row, col_qtysale, Qt::EditRole).toDouble();
    }
    fModel->setData(row, col_qtyafter,
                      fModel->data(row, col_qtybefore, Qt::EditRole).toDouble()
                    + fModel->data(row, col_qtyinput, Qt::EditRole).toDouble()
                    - qtySale
                    - fModel->data(row, col_qtyout, Qt::EditRole).toDouble());
    fModel->setData(row, col_qtydiff,
                    fModel->data(row, col_qtyinv, Qt::EditRole).toDouble()
                    - fModel->data(row, col_qtystore, Qt::EditRole).toDouble());
}

bool CR5ConsumptionBySales::tblDoubleClicked(int row, int column, const QList<QVariant> &values)
{
    if (values.count() == 0) {
        return true;
    }
    bool ok;
    double qty;
    QString docid;
    C5Database db(fDBParams);
    C5StoreDraftWriter dw(db);
    switch (column) {
    case col_qtyinput: {
        auto *ddi = __mainWindow->createTab<CR5GoodsMovement>(fDBParams);
        ddi->setDocType(QString("%1,%2").arg(DOC_TYPE_STORE_INPUT).arg(DOC_TYPE_STORE_MOVE));
        ddi->setDate(fFilter->date1(), fFilter->date2());
        ddi->setStore(QString::number(fFilter->store()));
        ddi->setGoods(QString::number(values.at(0).toInt()));
        ddi->setReason(QString("%1,%2, %3").arg(DOC_REASON_INPUT).arg(DOC_REASON_MOVE).arg(DOC_REASON_SALE_RETURN));
        ddi->setInOut(1);
        ddi->buildQuery();
        break;
    }
    case col_qtyout: {
        auto *ddo = __mainWindow->createTab<CR5GoodsMovement>(fDBParams);
        ddo->setDate(fFilter->date1(), fFilter->date2());
        ddo->setDocType(QString("%1,%2").arg(DOC_TYPE_STORE_OUTPUT).arg(DOC_TYPE_STORE_MOVE));
        ddo->setStore(QString::number(fFilter->store()));
        ddo->setGoods(QString::number(values.at(0).toInt()));
        ddo->setReason(QString("%1,%2").arg(DOC_REASON_OUT).arg(DOC_REASON_MOVE));
        ddo->setInOut(-1);
        ddo->buildQuery();
        break;
    }
    case col_qtysale: {
        //if (pr(fDBParams, cp_t3_consuption_reason)) {
            CR5ConsuptionReason *s = __mainWindow->createTab<CR5ConsuptionReason>(fDBParams);
            s->setFilterParams(fFilter->date1(), fFilter->date2(), fFilter->store(), values.at(0).toInt());
        //}
        break;
    }
    case col_qtyinv:
        qty = QInputDialog::getDouble(this, tr("Inventory qty"), tr("Qty"), 0, 0, 100000, 4, &ok);
        if (!ok) {
            return true;
        }
        docid = documentForInventory();
        db[":f_goods"] = values.at(0);
        db.exec(QString("delete from a_store_inventory where f_document in (%1) and f_goods=:f_goods").arg(docid));
        QString id;
        if (qty > 0.0001) {
            QString d = docid.split(",", QString::SkipEmptyParts).at(0);
            dw.writeAStoreInventory(id, d.replace("'", ""), fFilter->store(), values.at(0).toInt(), qty, 0, 0);
        }
        fModel->setData(row, column, qty);
        countRowQty(row);
        break;
    }
    return true;
}

void CR5ConsumptionBySales::makeOutput(bool v)
{
    Q_UNUSED(v);
    QMap<int, double> goodsSale, goodsLost, goodsOver;
    QList<QList<QVariant> > &rows = fModel->fRawData;
    for (int i = 0; i < rows.count(); i++) {
        double qty = rows[i][col_qtydiff].toDouble();
        if (rows[i][col_qtydiff].toDouble() > 0.0001) {
            qty = rows[i][col_qtydiff].toDouble();
        } else if (rows[i][col_qtydiff].toDouble() < -0.0001) {
            qty = rows[i][col_qtydiff].toDouble();
        }
        if (qty > 0.0001) {
            goodsOver[rows[i][col_goodsid].toInt()] = qty;
        }
        if (qty  < -0.0001) {
            goodsLost[rows[i][col_goodsid].toInt()] = qty * -1;
        }
    }
    if (goodsSale.count() > 0) {
        QList<IGoods> gl;
        for (QMap<int, double>::const_iterator it = goodsSale.begin(); it != goodsSale.end(); it++) {
            IGoods g;
            g.goodsId = it.key();
            g.goodsQty = it.value();
            gl.append(g);
        }
        writeDocs(DOC_TYPE_STORE_OUTPUT, DOC_REASON_SALE, gl, tr("Sale"));
    }
    if (goodsLost.count() > 0) {
        QList<IGoods> gl;
        for (QMap<int, double>::const_iterator it = goodsLost.constBegin(); it != goodsLost.constEnd(); it++) {
            IGoods g;
            g.goodsId = it.key();
            g.goodsQty = it.value();
            g.goodsPrice = 0;
            g.goodsTotal = 0;
            gl.append(g);
        }
        writeDocs(DOC_TYPE_STORE_OUTPUT, DOC_REASON_LOST, gl, tr("Lost"));
    }
    if (goodsOver.count() > 0) {
        QList<IGoods> gl;
        for (QMap<int, double>::const_iterator it = goodsOver.begin(); it != goodsOver.end(); it++) {
            IGoods g;
            g.goodsId = it.key();
            g.goodsQty = it.value();
            gl.append(g);
        }
        C5StoreDoc *sd = writeDocs(DOC_TYPE_STORE_INPUT, DOC_REASON_OVER, gl, tr("Over"));
        if (sd) {
            sd->setLastInputPrices();
        }
    }
}

void CR5ConsumptionBySales::salesOutput(bool v)
{
    Q_UNUSED(v);
    QMap<int, double> goodsSale;
    QList<QList<QVariant> > &rows = fModel->fRawData;
    for (int i = 0; i < rows.count(); i++) {
        double qty = rows[i][col_qtysale].toDouble();
        if (qty > 0.0001) {
            goodsSale[rows[i][col_goodsid].toInt()] = qty;
        }
    }
    if (goodsSale.count() > 0) {
        QList<IGoods> gl;
        for (QMap<int, double>::const_iterator it = goodsSale.begin(); it != goodsSale.end(); it++) {
            IGoods g;
            g.goodsId = it.key();
            g.goodsQty = it.value();
            gl.append(g);
        }
        writeDocs(DOC_TYPE_STORE_OUTPUT, DOC_REASON_SALE, gl, tr("Sale"));
    }
}

void CR5ConsumptionBySales::rollbackGoodsOutput(bool v)
{
    Q_UNUSED(v);
    C5Database db(fDBParams);
    C5WaiterOrderDoc::clearStoreOutput(db, fFilter->date1(), fFilter->date2());
    C5Message::info(tr("Done"));
}

void CR5ConsumptionBySales::countOutputBasedOnRecipes()
{
    C5ProgressDialog *pd = new C5ProgressDialog(this);
    connect(this, SIGNAL(updateProgressValue(int)), pd, SLOT(updateProgressValue(int)));
    C5Database db(fDBParams);
    db[":f_datecash1"] = fFilter->date1();
    db[":f_datecash2"] = fFilter->date2();
    db.exec("select f_id from o_header where f_datecash between :f_datecash1 and :f_datecash2 order by f_datecash");
    QStringList idList;
    while (db.nextRow()) {
        idList.append(db.getString(0));
    }
    pd->setMax(idList.count());
    pd->show();
    QString err;
    int count = 0;
    for (const QString &id: idList) {
        C5WaiterOrderDoc(id, db).makeOutputOfStore(db, err, DOC_STATE_SAVED);
        count++;
        emit updateProgressValue(count);
    }
    delete pd;
    if (err.isEmpty()) {
        C5Message::info(tr("Done"));
    } else {
        C5Message::error(tr("Complete with errors"));
    }
}

void CR5ConsumptionBySales::changeOutputStore()
{
    DlgChangeOutputStore *d = new DlgChangeOutputStore(fDBParams);
    d->refresh(fFilter->date1(), fFilter->date2());
    d->exec();
    delete d;
}

void CR5ConsumptionBySales::updateInventorizatinPrices()
{
    C5Database db(fDBParams);

    db[":f_store"] = fFilter->store();
    db[":f_date"] = fFilter->date2();
    db.exec(QString("select s.f_goods, sum(f_qty*s.f_type*f_price) / sum(f_qty*s.f_type) as f_price "
                "from a_store s "
                "inner join a_header d on d.f_id=s.f_document "
                "where s.f_store=:f_store and d.f_date<=:f_date "
                "group by 1 "));
    QMap<int, double> prices;
    while (db.nextRow()) {
        prices[db.getInt("f_goods")] = db.getDouble("f_price");
    }

    for (int i = 0; i < fModel->rowCount(); i++) {
        db[":f_type"] = DOC_TYPE_STORE_INVENTORY;
        db[":f_store"] = fFilter->store();
        db[":f_date"] = fFilter->date2();
        db[":f_goods"] = fModel->data(i, col_goodsid, Qt::EditRole);
        db[":f_price"] = prices[fModel->data(i, col_goodsid, Qt::EditRole).toInt()];
        db.exec("update a_store_inventory set f_price=:f_price, f_total=f_qty*:f_price where f_goods=:f_goods and f_store=:f_store "
            " and f_document in (select f_id from a_header where f_type=:f_type and f_date=:f_date)");
    }

    refreshData();
}

C5StoreDoc *CR5ConsumptionBySales::writeDocs(int doctype, int reason, const QList<IGoods> &data, const QString &comment)
{
    C5Database db(fDBParams);
    C5StoreDraftWriter dw(db);
    QDate docDate = fFilter->date2();
    int store = 0;
    int storein = 0;
    int storeout = 0;
    int sdtype = 0;
    switch (doctype) {
    case DOC_TYPE_STORE_OUTPUT:
        store = fFilter->store();
        storeout = store;
        sdtype = -1;
        break;
    case DOC_TYPE_STORE_INPUT:
        store = fFilter->store();
        storein = store;
        sdtype = 1;
        break;
    }
    QString documentId;
    QString cashid, cashrowid;
    dw.writeAHeader(documentId, dw.storeDocNum(doctype, store, true, 0), DOC_STATE_DRAFT, doctype, __user->id(), docDate,
                    QDate::currentDate(), QTime::currentTime(), 0, 0, comment);
    if (storein > 0) {
        C5Document c5doc(fDBParams);
        QString purpose = tr("Store input, correction") + QDate::currentDate().toString(FORMAT_DATE_TO_STR);
        QString cashUserId = QString("%1").arg(c5doc.genNumber(DOC_TYPE_CASH), C5Config::docNumDigitsInput(), 10, QChar('0'));
        dw.writeAHeader(cashid, cashUserId, DOC_STATE_DRAFT, DOC_TYPE_CASH, __user->id(), QDate::currentDate(),
                        QDate::currentDate(), QTime::currentTime(), 0, 0, purpose);
        dw.writeAHeaderCash(cashid, 0, 0, 1, documentId, "", 0);
        dw.writeECash(cashrowid, cashid, 0, -1, purpose, 0, cashrowid, 1);
    }
    dw.writeAHeaderStore(documentId, __user->id(), __user->id(), "", QDate(), storein, storeout, 0, cashid, 0, 0);
    int rownum = 1;
    foreach (const IGoods &g, data) {
        QString sdid;
        dw.writeAStoreDraft(sdid, documentId, store, sdtype, g.goodsId, g.goodsQty, g.goodsPrice, g.goodsPrice * g.goodsQty, reason, "", rownum++, "");
    }
    //= dw.writeDraft(docDate, doctype, store, reason, data, comment);
    auto *sd = __mainWindow->createTab<C5StoreDoc>(fDBParams);
    QString e;
    if (!sd->openDoc(documentId, e)) {
        __mainWindow->removeTab(sd);
        sd = nullptr;
        C5Message::error(e);
    }
    return sd;
}
