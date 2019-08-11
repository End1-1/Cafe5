#include "cr5consumptionbysales.h"
#include "cr5consumptionbysalesfilter.h"
#include "c5gridgilter.h"
#include "c5tablemodel.h"
#include "c5mainwindow.h"
#include "c5double.h"
#include "dlgchangeoutputstore.h"
#include "c5storedoc.h"
#include <QInputDialog>

static const int col_goodsid = 0;
static const int col_goodsgroup = 1;
static const int col_goodsname = 2;
static const int col_qtybefore = 3;
static const int col_qtyinput = 4;
static const int col_qtysale = 5;
static const int col_qtyout = 6;
static const int col_qtystore = 7;
static const int col_qtyafter = 8;
static const int col_qtyinv = 9;
static const int col_qtydiff = 10;

CR5ConsumptionBySales::CR5ConsumptionBySales(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/goods.png";
    fLabel = tr("Consumption of goods base on sales");

    fColumnNameIndex["f_goodsid"] = col_goodsid;
    fColumnNameIndex["f_goodsgroup"] = col_goodsgroup;
    fColumnNameIndex["f_goodsname"] = col_goodsname;
    fColumnNameIndex["f_qtybefore"] = col_qtybefore;
    fColumnNameIndex["f_qtyinput"] = col_qtyinput;
    fColumnNameIndex["f_qtysale"] = col_qtysale;
    fColumnNameIndex["f_qtyout"] = col_qtyout;
    fColumnNameIndex["f_qtystore"] = col_qtystore;
    fColumnNameIndex["f_qtyafter"] = col_qtyafter;
    fColumnNameIndex["f_qtyinv"] = col_qtyinv;
    fColumnNameIndex["f_qtydiff"] = col_qtydiff;

    fColumnsSum << "f_qtybefore"
                << "f_qtyinput"
                << "f_qtysale"
                << "f_qtyout"
                << "f_qtystore"
                << "f_qtyafter"
                << "f_qtyinv"
                << "f_qtydiff";

    fTranslation["f_goodsid"] = tr("Goods code");
    fTranslation["f_goodsgroup"] = tr("Group");
    fTranslation["f_goodsname"] = tr("Goods name");
    fTranslation["f_qtybefore"] = tr("Qty, before");
    fTranslation["f_qtyinput"] = tr("Qty, input");
    fTranslation["f_qtysale"] = tr("Qty, sale");
    fTranslation["f_qtyout"] = tr("Qty, out");
    fTranslation["f_qtystore"] = tr("Qty, store");
    fTranslation["f_qtyafter"] = tr("Qty, after");
    fTranslation["f_qtyinv"] = tr("Qty, inv");
    fTranslation["f_qtydiff"] = tr("Qty, diff");

    fFilterWidget = new CR5ConsumptionBySalesFilter(dbParams);
    fFilter = static_cast<CR5ConsumptionBySalesFilter*>(fFilterWidget);
    connect(this, SIGNAL(tblDoubleClicked(int,int,QList<QVariant>)), this, SLOT(tblDoubleClicked(int,int,QList<QVariant>)));
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
        fToolBar->insertAction(a, b);
        auto *d = new QAction(QIcon(":/decision.png"), tr("Change the store of output"), this);
        connect(d, SIGNAL(triggered(bool)), this, SLOT(changeOutputStore()));
        fToolBar->insertAction(b, d);
        auto *c = new QAction(QIcon(":/goods.png"), tr("Sales\noutput"), this);
        connect(c, SIGNAL(triggered(bool)), this, SLOT(salesOutput(bool)));
        fToolBar->insertAction(a, c);
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
    C5Database db(fDBParams);
    /* get all goods */
    db.exec("select c.f_id, g.f_name as f_groupname, c.f_name "
            "from c_goods c "
            "left join c_groups g on g.f_id=c.f_group "
            "order by 2, 3");
    while (db.nextRow()) {
    QList<QVariant> row;
        row << db.getInt("f_id")
            << db.getString("f_groupname")
            << db.getString("f_name")
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
    db.exec("select s.f_goods, sum(s.f_qty*s.f_type) as f_qty "
                "from a_store s "
                "inner join a_header d on d.f_id=s.f_document "
                "where s.f_store=:f_store and d.f_date<=:f_date "
                "group by 1 "
                "having sum(s.f_qty*s.f_type) > 0.001");
    while (db.nextRow()) {
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
    db.exec("select s.f_goods, sum(s.f_qty) "
            "from a_store s "
            "left join a_header h on h.f_id=s.f_document "
            "where h.f_date between :f_date1 and :f_date2 and s.f_store=:f_store "
            "and h.f_state=:f_state and s.f_type=:f_type "
            "group by 1 ");
    while (db.nextRow()) {
        int r = goodsMap[db.getInt(0)];
        rows[r][col_qtyinput] = db.getDouble(1);
    }
    /* get output based on recipes */
    db[":f_store"] = f->store();
    db[":f_datecash1"] = f->date1();
    db[":f_datecash2"] = f->date2();
    db[":f_headerstate"] = ORDER_STATE_CLOSE;
    db[":f_bodystate1"] = DISH_STATE_OK;
    db[":f_bodystate2"] = DISH_STATE_VOID;
    db.exec("select so.f_goods, sum(so.f_qty) as f_qty "
            "from o_store_output so "
            "inner join o_header oh on oh.f_id=so.f_header "
            "inner join o_body ob on ob.f_id=so.f_body "
            "where oh.f_datecash between :f_datecash1 and :f_datecash2 and oh.f_state=:f_headerstate "
            "and (ob.f_state=:f_bodystate1 or ob.f_state=:f_bodystate2) and ob.f_store=:f_store "
            "group by 1");
    while (db.nextRow()) {
        int r = goodsMap[db.getInt(0)];
        rows[r][col_qtysale] = db.getDouble("f_qty");
    }
    /* get output */
    db[":f_store"] = f->store();
    db[":f_date1"] = f->date1();
    db[":f_date2"] = f->date2();
    db[":f_type"] = -1;
    db[":f_state"] = DOC_STATE_SAVED;
    db.exec("select s.f_goods, sum(s.f_qty) "
            "from a_store s "
            "left join a_header h on h.f_id=s.f_document "
            "where h.f_date between :f_date1 and :f_date2 and s.f_store=:f_store "
            "and s.f_type=:f_type and h.f_state=:f_state  "
            "group by 1 ");
    while (db.nextRow()) {
        int r = goodsMap[db.getInt(0)];
        rows[r][col_qtyout] = db.getDouble(1);
    }
    /* get store at the date end */
    db[":f_store"] = f->store();
    db[":f_date"] = f->date2();
    db.exec("select s.f_goods, sum(s.f_qty*s.f_type) as f_qty "
                "from a_store s "
                "inner join a_header d on d.f_id=s.f_document "
                "where s.f_store=:f_store and d.f_date<=:f_date "
                "group by 1 "
                "having sum(s.f_qty*s.f_type) > 0.001");
    while (db.nextRow()) {
        int r = goodsMap[db.getInt(0)];
        rows[r][col_qtystore] = db.getDouble("f_qty");
    }
    /* get counted qty */
    for (int i = 0; i < rows.count(); i++) {
        rows[i][col_qtyafter] = rows[i][col_qtybefore].toDouble() - rows[i][col_qtysale].toDouble();
    }
    /* get inventory quantities */
    db[":f_date"] = f->date2();
    db[":f_store"] = f->store();
    db[":f_type"] = DOC_TYPE_STORE_INVENTORY;
    db.exec("select i.f_goods, g.f_name, sum(i.f_qty) as f_qty "
            "from a_store_inventory i "
            "inner join a_header h on h.f_id=i.f_document "
            "inner join c_goods g on g.f_id=i.f_goods "
            "where h.f_date=:f_date and h.f_type=:f_type "
            "group by 1, 2 ");
    while (db.nextRow()) {
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
    restoreColumnsWidths();
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
        result = C5Database::uuid(fDBParams);
        db[":f_id"] = result;
        db[":f_state"] = DOC_STATE_SAVED;
        db[":f_type"] = DOC_TYPE_STORE_INVENTORY;
        db[":f_operator"] = __userid;
        db[":f_date"] = f->date2();
        db[":f_createDate"] = QDate::currentDate();
        db[":f_createTime"] = QTime::currentTime();
        db[":f_partner"] = 0;
        db[":f_amount"] = 0;
        db[":f_comment"] = tr("Created automaticaly");
        db[":f_raw"] = "";
        db.insert("a_header", false);
        result = QString("'%1'").arg(result);
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
                    - fModel->data(row, col_qtyafter, Qt::EditRole).toDouble());
}

void CR5ConsumptionBySales::tblDoubleClicked(int row, int column, const QList<QVariant> &values)
{
    bool ok;
    double qty;
    QString docid;
    C5Database db(fDBParams);
    switch (column) {
    case col_qtyinv:
        qty = QInputDialog::getDouble(this, tr("Inventory qty"), tr("Qty"), 0, 0, 100000, 4, &ok);
        if (!ok) {
            return;
        }
        if (qty < 0.0001) {
            C5Message::error(tr("Quantity must be greater than 0"));
            return;
        }
        docid = documentForInventory();
        db[":f_goods"] = values.at(0);
        db.exec(QString("delete from a_store_inventory where f_document in (%1) and f_goods=:f_goods").arg(docid));
        db[":f_id"] = C5Database::uuid(fDBParams);
        QString doc = docid.split(",", QString::SkipEmptyParts).at(0);
        db[":f_document"] = doc.replace("'", "");
        db[":f_store"] = fFilter->store();
        db[":f_goods"] = values.at(0);
        db[":f_qty"] = qty;
        db[":f_price"] = 0;
        db[":f_total"] = 0;
        db.insert("a_store_inventory", false);
        fModel->setData(row, column, qty);
        countRowQty(row);
        break;
    }
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
        writeDocs(DOC_TYPE_STORE_OUTPUT, DOC_REASON_SALE, goodsSale, tr("Sale"));
    }
    if (goodsLost.count() > 0) {
        writeDocs(DOC_TYPE_STORE_OUTPUT, DOC_REASON_LOST, goodsLost, tr("Lost"));
    }
    if (goodsOver.count() > 0) {
        writeDocs(DOC_TYPE_STORE_INPUT, DOC_REASON_OVER, goodsOver, tr("Over"));
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
        writeDocs(DOC_TYPE_STORE_OUTPUT, DOC_REASON_SALE, goodsSale, tr("Sale"));
    }
}

void CR5ConsumptionBySales::countOutputBasedOnRecipes()
{
    if (fFilter->store() == 0) {
        C5Message::error(tr("Store must be selected"));
        return;
    }
    C5Database db(fDBParams);
    db[":f_store"] = fFilter->store();
    db[":f_datecash1"] = fFilter->date1();
    db[":f_datecash2"] = fFilter->date2();
    db.exec("delete from o_store_output where f_body in "
            "(select f_id from o_body where f_store=:f_store and f_header in "
            "(select f_id from o_header where f_datecash between :f_datecash1 and :f_datecash2)) ");

    db[":f_store"] = fFilter->store();
    db[":f_datecash1"] = fFilter->date1();
    db[":f_datecash2"] = fFilter->date2();
    db[":f_headerstate"] = ORDER_STATE_CLOSE;
    db[":f_bodystate1"] = DISH_STATE_OK;
    db[":f_bodystate2"] = DISH_STATE_VOID;
    db.exec("select h.f_id as f_header, b.f_id as f_body, b.f_dish, r.f_goods, sum(b.f_qty1*r.f_qty) as f_qty "
            "from d_recipes r "
            "inner join o_body b on b.f_dish=r.f_dish "
            "inner join o_header h on h.f_id=b.f_header "
            "where h.f_datecash between :f_datecash1 and :f_datecash2 and h.f_state=:f_headerstate "
            "and (b.f_state=:f_bodystate1 or b.f_state=:f_bodystate2) and b.f_store=:f_store "
            "group by 1,2,3,4");
    C5Database db2(fDBParams);
    while (db.nextRow()) {
        db2[":f_header"] = db.getString("f_header");
        db2[":f_body"] = db.getString("f_body");
        db2[":f_goods"] = db.getString("f_goods");
        db2[":f_qty"] = db.getDouble("f_qty");
        db2[":f_price"] = 0;
        db2.insert("o_store_output", false);
    }
    C5Message::info(tr("Done"));
}

void CR5ConsumptionBySales::changeOutputStore()
{
    DlgChangeOutputStore *d = new DlgChangeOutputStore(fDBParams, this);
    d->refresh(fFilter->date1(), fFilter->date2());
    d->exec();
    delete d;
}

void CR5ConsumptionBySales::writeDocs(int doctype, int reason, const QMap<int, double> &data, const QString &comment)
{
    C5Database db(fDBParams);
    QJsonObject jo;
    switch (doctype) {
    case DOC_TYPE_STORE_OUTPUT:
        jo["f_storein"] = "";
        jo["f_storeout"] = QString::number(static_cast<CR5ConsumptionBySalesFilter*>(fFilterWidget)->store());
        break;
    case DOC_TYPE_STORE_INPUT:
        jo["f_storein"] = QString::number(static_cast<CR5ConsumptionBySalesFilter*>(fFilterWidget)->store());
        jo["f_storeout"] = "";
        break;
    }
    QJsonDocument jd(jo);
    QString docid = C5Database::uuid();
    db[":f_state"] = DOC_STATE_DRAFT;
    db[":f_type"] = doctype;
    db[":f_operator"] = __userid;
    db[":f_date"] = static_cast<CR5ConsumptionBySalesFilter*>(fFilterWidget)->date2();
    db[":f_createDate"] = QDate::currentDate();
    db[":f_createTime"] = QTime::currentTime();
    db[":f_partner"] = 0;
    db[":f_amount"] = 0;
    db[":f_comment"] = comment;
    db[":f_raw"] = jd.toJson();
    db[":f_id"]= docid;
    db.insert("a_header", false);
    for (QMap<int, double>::const_iterator it = data.begin(); it != data.end(); it++) {
        db[":f_id"] = C5Database::uuid();
        db[":f_document"] = docid;
        db[":f_goods"] = it.key();
        db[":f_qty"] = it.value();
        db[":f_price"] = 0;
        db[":f_total"] = 0;
        db[":f_reason"] = reason;
        db.insert("a_store_draft", false);
    }
    C5StoreDoc *sd = __mainWindow->createTab<C5StoreDoc>(fDBParams);
    if (!sd->openDoc(docid)) {
        __mainWindow->removeTab(sd);
    }
}
