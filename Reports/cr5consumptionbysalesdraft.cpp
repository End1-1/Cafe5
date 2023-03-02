#include "cr5consumptionbysalesdraft.h"
#include "cr5consumptionbysalesfilterdraft.h"
#include "c5gridgilter.h"
#include "cr5consuptionreason.h"
#include "c5tablemodel.h"
#include "c5user.h"
#include "c5mainwindow.h"
#include "c5waiterorderdoc.h"
#include "c5progressdialog.h"
#include "c5double.h"
#include "cr5goodsmovement.h"
#include "dlgchangeoutputstore.h"
#include "c5storedraftwriter.h"
#include "c5storedoc.h"
#include <QInputDialog>

static const int col_goodsid = 0;
static const int col_goodsgroup = 1;
static const int col_goodsname = 2;
static const int col_goodscode = 3;
static const int col_qtybefore = 4;
static const int col_prbefore = 5;
static const int col_qtyinput = 6;
static const int col_prinput = 7;
static const int col_qtysale = 8;
static const int col_prsale = 9;
static const int col_qtyout = 10;
static const int col_prout = 11;
static const int col_qtystore = 12;
static const int col_prstore = 13;
static const int col_qtyinv = 14;
static const int col_prinv = 15;
static const int col_qtydiff = 16;
static const int col_prdiff = 17;

CR5ConsumptionBySalesDraft::CR5ConsumptionBySalesDraft(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/goods.png";
    fLabel = tr("Consumption of goods base on sales, draft");

    fColumnNameIndex["f_goodsid"] = col_goodsid;
    fColumnNameIndex["f_goodsgroup"] = col_goodsgroup;
    fColumnNameIndex["f_goodsname"] = col_goodsname;
    fColumnNameIndex["f_goodscode"] = col_goodscode;
    fColumnNameIndex["f_qtybefore"] = col_qtybefore;
    fColumnNameIndex["f_prbefore"] = col_prbefore;
    fColumnNameIndex["f_qtyinput"] = col_qtyinput;
    fColumnNameIndex["f_prinput"] = col_prinput;
    fColumnNameIndex["f_qtysale"] = col_qtysale;
    fColumnNameIndex["f_prsale"] = col_prsale;
    fColumnNameIndex["f_qtyout"] = col_qtyout;
    fColumnNameIndex["f_prout"] = col_prout;
    fColumnNameIndex["f_qtystore"] = col_qtystore;
    fColumnNameIndex["f_prstore"] = col_prstore;
    fColumnNameIndex["f_qtyinv"] = col_qtyinv;
    fColumnNameIndex["f_prinv"] = col_prinv;
    fColumnNameIndex["f_qtydiff"] = col_qtydiff;
    fColumnNameIndex["f_prdiff"] = col_prdiff;

    fColumnsSum << "f_qtybefore"
                << "f_prbefore"
                << "f_qtyinput"
                << "f_prinput"
                << "f_qtysale"
                << "f_prsale"
                << "f_qtyout"
                << "f_prout"
                << "f_qtystore"
                << "f_prstore"
                << "f_qtyinv"
                << "f_prinv"
                << "f_qtydiff"
                << "f_prdiff";

    fTranslation["f_goodsid"] = tr("Goods code");
    fTranslation["f_goodsgroup"] = tr("Group");
    fTranslation["f_goodsname"] = tr("Goods name");
    fTranslation["f_goodscode"] = tr("Scancode");
    fTranslation["f_qtybefore"] = tr("Qty, before");
    fTranslation["f_prbefore"] = tr("Amount, before");
    fTranslation["f_qtyinput"] = tr("Qty, input");
    fTranslation["f_prinput"] = tr("Amount, input");
    fTranslation["f_qtysale"] = tr("Qty, sale");
    fTranslation["f_prsale"] = tr("Amount, sale");
    fTranslation["f_qtyout"] = tr("Qty, out");
    fTranslation["f_prout"] = tr("Amount, out");
    fTranslation["f_qtystore"] = tr("Qty, store");
    fTranslation["f_prstore"] = tr("Amount, store");
    fTranslation["f_qtyinv"] = tr("Qty, inv");
    fTranslation["f_prinv"] = tr("Amount, inv");
    fTranslation["f_qtydiff"] = tr("Qty, diff");
    fTranslation["f_prdiff"] = tr("Amount, diff");

    fFilterWidget = new CR5ConsumptionBySalesFilterDraft(dbParams);
    fFilter = static_cast<CR5ConsumptionBySalesFilterDraft*>(fFilterWidget);
}

QToolBar *CR5ConsumptionBySalesDraft::toolBar()
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
        auto *f = new QAction(QIcon(":/goodsback.png"), tr("Get from AS"), this);
        connect(f, SIGNAL(triggered(bool)), this, SLOT(asinput(bool)));
        fToolBar->insertAction(b, f);
        auto *g = new QAction(QIcon(":/goodsback.png"), tr("Output to AS"), this);
        connect(g, SIGNAL(triggered(bool)), this, SLOT(asoutput(bool)));
        fToolBar->insertAction(b, g);
    }
    return fToolBar;
}

void CR5ConsumptionBySalesDraft::buildQuery()
{
    auto *f = static_cast<CR5ConsumptionBySalesFilterDraft*>(fFilterWidget);
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
    db.exec("select c.f_id, g.f_name as f_groupname, c.f_name, c.f_scancode "
            "from c_goods c "
            "left join c_groups g on g.f_id=c.f_group " + cond +
            "order by 2, 3");
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
            << QVariant()
            << QVariant()
            << QVariant()
            << QVariant()
            << QVariant()
            << QVariant();
        rows.append(row);
        goodsMap[db.getInt("f_id")] = rows.count() - 1;
    }
    /* get qty before */
    db[":f_store"] = f->store();
    db[":f_date"] = f->date1().addDays(-1);
    db.exec("select s.f_goods, sum(s.f_qty*s.f_type) as f_qty, sum(s.f_qty*s.f_type*g.f_lastinputprice) as f_pr "
                "from a_store_draft s "
                "inner join c_goods g on g.f_id=s.f_goods "
                "inner join a_header d on d.f_id=s.f_document "
                "where s.f_store=:f_store and d.f_date<=:f_date "
                "group by 1 ");
    while (db.nextRow()) {
        if (!goodsMap.contains(db.getInt(0))) {
            continue;
        }
        int r = goodsMap[db.getInt(0)];
        rows[r][col_qtybefore] = db.getDouble("f_qty");
        rows[r][col_prbefore] = db.getDouble("f_pr");
    }
    /* get input */
    db[":f_store"] = f->store();
    db[":f_date1"] = f->date1();
    db[":f_date2"] = f->date2();
    db[":f_type"] = 1;
    db[":f_store"] = f->store();
    db.exec("select s.f_goods, sum(s.f_qty), sum(s.f_qty*g.f_lastinputprice) "
            "from a_store_draft s "
            "inner join c_goods g on g.f_id=s.f_goods "
            "left join a_header h on h.f_id=s.f_document "
            "where h.f_date between :f_date1 and :f_date2 and s.f_store=:f_store "
            "and s.f_type=:f_type "
            "group by 1 ");
    while (db.nextRow()) {
        if (!goodsMap.contains(db.getInt(0))) {
            continue;
        }
        int r = goodsMap[db.getInt(0)];
        rows[r][col_qtyinput] = db.getDouble(1);
        rows[r][col_prinput] = db.getDouble(2);
    }
    /* get output based on recipes */
    QString cond1;
//    if (!fFilter->draft()) {
//        db[":f_state"] = DOC_STATE_SAVED;
//        cond1 += " and h.f_state=:f_state ";
//    }
    db[":f_store"] = f->store();
    db[":f_date1"] = f->date1();
    db[":f_date2"] = f->date2();
    db[":f_type"] = DOC_TYPE_STORE_OUTPUT;
    db[":f_reason"] = DOC_REASON_SALE;
    db.exec("select s.f_goods, sum(s.f_qty) as f_qty, sum(s.f_qty*g.f_lastinputprice) as f_total "
            "from a_store_draft s "
            "inner join c_goods g on g.f_id=s.f_goods "
            "inner join a_header h on h.f_id=s.f_document "
            "where h.f_date between :f_date1 and :f_date2 and s.f_store=:f_store "
            "and s.f_reason=:f_reason and h.f_type=:f_type " + cond1 +
            "group by 1 ");
    while (db.nextRow()) {
        if (!goodsMap.contains(db.getInt(0))) {
            continue;
        }
        int r = goodsMap[db.getInt(0)];
        rows[r][col_qtysale] = db.getDouble("f_qty");
        rows[r][col_prsale] = db.getDouble("f_total");
    }
    /* get output */
    db[":f_store"] = f->store();
    db[":f_date1"] = f->date1();
    db[":f_date2"] = f->date2();
    db[":f_type"] = -1;
    db[":f_reason"] = DOC_REASON_SALE;
    db.exec("select s.f_goods, sum(s.f_qty) as f_qty, sum(s.f_qty*g.f_lastinputprice) as f_total "
            "from a_store_draft s "
            "inner join c_goods g on g.f_id=s.f_goods "
            "left join a_header h on h.f_id=s.f_document "
            "where h.f_date between :f_date1 and :f_date2 and s.f_store=:f_store "
            "and s.f_type=:f_type "
            "and s.f_reason<>:f_reason "
            "group by 1 ");
    while (db.nextRow()) {
        if (!goodsMap.contains(db.getInt(0))) {
            continue;
        }
        int r = goodsMap[db.getInt(0)];
        rows[r][col_qtyout] = db.getDouble(1);
        rows[r][col_prout] = db.getDouble(2);
    }
    /* get store at the date end */
    db[":f_store"] = f->store();
    db[":f_date"] = f->date2();
    db.exec("select s.f_goods, sum(s.f_qty*s.f_type) as f_qty, sum(s.f_qty*g.f_lastinputprice*s.f_type) as f_total "
                "from a_store_draft s "
                "inner join c_goods g on g.f_id=s.f_goods "
                "inner join a_header d on d.f_id=s.f_document "
                "where s.f_store=:f_store and d.f_date<=:f_date "
                "group by 1 ");
    while (db.nextRow()) {
        if (!goodsMap.contains(db.getInt(0))) {
            continue;
        }
        int r = goodsMap[db.getInt(0)];
        rows[r][col_qtystore] = db.getDouble("f_qty");
        rows[r][col_prstore] = db.getDouble("f_total");
    }

    /* get inventory quantities */
    db[":f_date"] = f->date2();
    db[":f_store"] = f->store();
    db[":f_type"] = DOC_TYPE_STORE_INVENTORY;
    db.exec("select i.f_goods, g.f_name, sum(i.f_qty) as f_qty, sum(i.f_qty*g.f_lastinputprice) as f_total "
            "from a_store_inventory i "
            "inner join a_header h on h.f_id=i.f_document "
            "inner join c_goods g on g.f_id=i.f_goods "
            "where h.f_date=:f_date and h.f_type=:f_type "
            "and i.f_store=:f_store "
            "group by 1, 2 ");
    while (db.nextRow()) {
        if (!goodsMap.contains(db.getInt(0))) {
            continue;
        }
        int r = goodsMap[db.getInt(0)];
        rows[r][col_qtyinv] = db.getDouble("f_qty");
        rows[r][col_prinv] = db.getDouble("f_total");
    }
    //remove unused
    for (int i  = rows.count() - 1; i > -1; i--) {
        if (zerodouble(rows[i][col_qtybefore].toDouble())
                && zerodouble(rows[i][col_qtysale].toDouble())
                && zerodouble(rows[i][col_qtyout].toDouble())
                && zerodouble(rows[i][col_qtystore].toDouble())
                && zerodouble(rows[i][col_qtyinv].toDouble())
                && zerodouble(rows[i][col_qtydiff].toDouble())) {
            rows.removeAt(i);
        }
    }
    fModel->setExternalData(fColumnNameIndex, fTranslation);
    for (int i  = rows.count() - 1; i > -1; i--) {
        countRowQty(i);
    }
    setPriceVisible(2);
    restoreColumnsWidths();
    sumColumnsData();
    emit refreshed();
}

void CR5ConsumptionBySalesDraft::refreshData()
{
    buildQuery();
}

QString CR5ConsumptionBySalesDraft::documentForInventory()
{
    QString result;
    auto *f = static_cast<CR5ConsumptionBySalesFilterDraft*>(fFilterWidget);
    C5Database db(fDBParams);
    db[":f_date"] = f->date2();
    db[":☺f_type"] = DOC_TYPE_STORE_INVENTORY;
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
        dw.writeAHeader(result, QString::number(dw.counterAType(DOC_TYPE_STORE_INVENTORY)), DOC_STATE_SAVED, DOC_TYPE_STORE_INVENTORY, __user->id(), f->date2(), QDate::currentDate(), QTime::currentTime(), 0, 0, tr("Created automaticaly"), __c5config.getValue(param_default_currency).toInt());
    }
    return result;
}

void CR5ConsumptionBySalesDraft::countRowQty(int row)
{
    fModel->setData(row, col_qtydiff,
                    fModel->data(row, col_qtyinv, Qt::EditRole).toDouble()
                    - fModel->data(row, col_qtystore, Qt::EditRole).toDouble());
    fModel->setData(row, col_prdiff,
                    fModel->data(row, col_prinv, Qt::EditRole).toDouble()
                    - fModel->data(row, col_prstore, Qt::EditRole).toDouble());
}

void CR5ConsumptionBySalesDraft::setPriceVisible(int mode)
{
    fColumnsVisible["f_goodsid"] = true;
    fColumnsVisible["f_goodsgroup"] = true;
    fColumnsVisible["f_goodsname"] = true;
    fColumnsVisible["f_goodscode"] = false;
    fColumnsVisible["f_qtybefore"] = mode == 1 || mode == 3;
    fColumnsVisible["f_qtyinput"] = mode == 1 || mode == 3;
    fColumnsVisible["f_qtysale"] = mode == 1 || mode == 3;
    fColumnsVisible["f_qtyout"] = mode == 1 || mode == 3;
    fColumnsVisible["f_qtystore"] = mode == 1 || mode == 3;
    fColumnsVisible["f_qtyinv"] = mode == 1 || mode == 3;
    fColumnsVisible["f_qtydiff"] = mode == 1 || mode == 3;
    fColumnsVisible["f_prbefore"] = mode == 2 || mode == 3;
    fColumnsVisible["f_prinput"] = mode == 2 || mode == 3;
    fColumnsVisible["f_prsale"] = mode == 2 || mode == 3;
    fColumnsVisible["f_prout"] = mode == 2 || mode == 3;
    fColumnsVisible["f_prstore"] = mode == 2 || mode == 3;
    fColumnsVisible["f_prinv"] = mode == 2 || mode == 3;
    fColumnsVisible["f_prdiff"] = mode == 2 || mode == 3;
}

bool CR5ConsumptionBySalesDraft::tblDoubleClicked(int row, int column, const QList<QVariant> &values)
{
    if (values.count() == 0) {
        return true;
    }
    bool ok;
    double qty;
    double price;
    QString docid;
    C5Database db(fDBParams);
    C5StoreDraftWriter dw(db);
    switch (column) {
    case col_qtyinput:
    case col_prinput: {
        auto *di = __mainWindow->createTab<CR5GoodsMovement>(fDBParams);
        di->setDocType(QString("%1,%2").arg(DOC_TYPE_STORE_INPUT).arg(DOC_TYPE_STORE_MOVE));
        di->setDate(fFilter->date1(), fFilter->date2());
        di->setStore(QString::number(fFilter->store()));
        di->setGoods(QString::number(values.at(0).toInt()));
        di->setReason(QString("%1,%2, %3").arg(DOC_REASON_INPUT).arg(DOC_REASON_MOVE).arg(DOC_REASON_SALE_RETURN));
        di->setInOut(1);
        di->buildQuery();
        break;
    }
    case col_qtyout:
    case col_prout: {
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
    case col_qtysale:
    case col_prsale: {
        //if (pr(fDBParams, cp_t3_consuption_reason)) {
            CR5ConsuptionReason *s = __mainWindow->createTab<CR5ConsuptionReason>(fDBParams);
            s->setFilterParams(fFilter->date1(), fFilter->date2(), fFilter->store(), values.at(0).toInt());
        //}
        break;
    }
    case col_qtyinv:
    case col_prinv:
        qty = QInputDialog::getDouble(this, tr("Inventory qty"), tr("Qty"), 0, 0, 100000, 4, &ok);
        if (!ok) {
            return true;
        }
        if ((fModel->data(row, col_qtybefore, Qt::EditRole).toDouble() + fModel->data(row, col_qtyinput, Qt::EditRole).toDouble()) > 0.00001
            && (fModel->data(row, col_qtybefore, Qt::EditRole).toDouble() + fModel->data(row, col_qtyinput, Qt::EditRole).toDouble()) < -0.00001) {
            price = abs((fModel->data(row, col_prbefore, Qt::EditRole).toDouble() + fModel->data(row, col_prinput, Qt::EditRole).toDouble())
                    / (fModel->data(row, col_qtybefore, Qt::EditRole).toDouble() + fModel->data(row, col_qtyinput, Qt::EditRole).toDouble()));
        }
        docid = documentForInventory();
        db[":f_goods"] = values.at(0);
        db.exec(QString("delete from a_store_inventory where f_document in (%1) and f_goods=:f_goods").arg(docid));
        QString id;
        if (qty > 0.0001) {
            QString d = docid.split(",", QString::SkipEmptyParts).at(0);
            dw.writeAStoreInventory(id, d.replace("'", ""), fFilter->store(), values.at(0).toInt(), qty, price, qty * price);
        }
        fModel->setData(row, column, qty);
        countRowQty(row);
        break;
    }
    return true;
}

void CR5ConsumptionBySalesDraft::makeOutput(bool v)
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

void CR5ConsumptionBySalesDraft::salesOutput(bool v)
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

void CR5ConsumptionBySalesDraft::rollbackGoodsOutput(bool v)
{
    Q_UNUSED(v);
    C5Database db(fDBParams);
    C5WaiterOrderDoc::clearStoreOutput(db, fFilter->date1(), fFilter->date2());
    C5Message::info(tr("Done"));
}

void CR5ConsumptionBySalesDraft::countOutputBasedOnRecipes()
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

void CR5ConsumptionBySalesDraft::changeOutputStore()
{
    DlgChangeOutputStore *d = new DlgChangeOutputStore(fDBParams);
    d->refresh(fFilter->date1(), fFilter->date2());
    d->exec();
    delete d;
}

C5StoreDoc *CR5ConsumptionBySalesDraft::writeDocs(int doctype, int reason, const QList<IGoods> &data, const QString &comment)
{
    C5Database db(fDBParams);
    C5StoreDraftWriter dw(db);
    QDate docDate = static_cast<CR5ConsumptionBySalesFilterDraft*>(fFilterWidget)->date2();
    int store = 0;
    int storein = 0;
    int storeout = 0;
    int sdtype = 0;
    switch (doctype) {
    case DOC_TYPE_STORE_OUTPUT:
        store = static_cast<CR5ConsumptionBySalesFilterDraft*>(fFilterWidget)->store();
        storeout = store;
        sdtype = -1;
        break;
    case DOC_TYPE_STORE_INPUT:
        store = static_cast<CR5ConsumptionBySalesFilterDraft*>(fFilterWidget)->store();
        storein = store;
        sdtype = 1;
        break;
    }
    QString documentId;
    QString cashid, cashrowid;
    dw.writeAHeader(documentId, dw.storeDocNum(doctype, store, true, 0), DOC_STATE_DRAFT, doctype, __user->id(), docDate,
                    QDate::currentDate(), QTime::currentTime(), 0, 0, comment, __c5config.getValue(param_default_currency).toInt());
    if (storein > 0) {
        C5Document c5doc(fDBParams);
        QString purpose = tr("Store input, correction") + QDate::currentDate().toString(FORMAT_DATE_TO_STR);
        QString cashUserId = QString("%1").arg(c5doc.genNumber(DOC_TYPE_CASH), C5Config::docNumDigitsInput(), 10, QChar('0'));
        dw.writeAHeader(cashid, cashUserId, DOC_STATE_DRAFT, DOC_TYPE_CASH, __user->id(), QDate::currentDate(),
                        QDate::currentDate(), QTime::currentTime(), 0, 0, purpose, __c5config.getValue(param_default_currency).toInt());
        dw.writeAHeaderCash(cashid, 0, 0, 1, documentId, "", 0);
        dw.writeECash(cashrowid, cashid, 0, -1, purpose, 0, cashrowid, 1);
    }
    dw.writeAHeaderStore(documentId, __user->id(), __user->id(), "", QDate(), storein, storeout, 0, cashid, 0, 0, "");
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

