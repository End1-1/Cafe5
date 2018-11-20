#include "cr5consumptionbysales.h"
#include "cr5consumptionbysalesfilter.h"
#include "c5gridgilter.h"
#include "c5tablemodel.h"
#include "c5storedoc.h"
#include <QInputDialog>

CR5ConsumptionBySales::CR5ConsumptionBySales(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/goods.png";
    fLabel = tr("Consumption of goods base on sales");

    fColumnNameIndex["f_goodsid"] = 0;
    fColumnNameIndex["f_goodsname"] = 1;
    fColumnNameIndex["f_qtysale"] = 2;
    fColumnNameIndex["f_qtystore"] = 3;
    fColumnNameIndex["f_amountstore"] = 4;
    fColumnNameIndex["f_qtyafter"] = 5;
    fColumnNameIndex["f_amountafter"] = 6;
    fColumnNameIndex["f_qtyinv"] = 7;
    fColumnNameIndex["f_qtydiff"] = 8;

    fTranslation["f_goodsid"] = tr("Goods code");
    fTranslation["f_goodsname"] = tr("Goods name");
    fTranslation["f_qtysale"] = tr("Qty, sale");
    fTranslation["f_qtystore"] = tr("Qty, store");
    fTranslation["f_amountstore"] = tr("Amount, store");
    fTranslation["f_qtyafter"] = tr("Qty, after");
    fTranslation["f_amountafter"] = tr("Amount, after");
    fTranslation["f_qtyinv"] = tr("Qty, inv");
    fTranslation["f_qtydiff"] = tr("Qty, diff");

    fFilterWidget = new CR5ConsumptionBySalesFilter(dbParams);
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
        QAction *a = new QAction(QIcon(":/goods.png"), tr("Make output"), this);
        connect(a, SIGNAL(triggered(bool)), this, SLOT(makeOutput(bool)));
        fToolBar->insertAction(fToolBar->actions().at(0), a);
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
    /* get output based on recipes */
    db[":f_store"] = f->store();
    db[":f_datecash1"] = f->date1();
    db[":f_datecash2"] = f->date2();
    db[":f_headerstate"] = ORDER_STATE_CLOSE;
    db[":f_bodystate1"] = DISH_STATE_OK;
    db[":f_bodystate2"] = DISH_STATE_VOID;
    db.exec("select r.f_goods, g.f_name, sum(ob.f_qty2 * r.f_qty) as f_qty "
            "from o_body ob "
            "left join o_header oh on oh.f_id=ob.f_header "
            "left join d_recipes r on r.f_dish=ob.f_dish "
            "left join c_goods g on g.f_id=r.f_goods "
            "where oh.f_datecash between :f_datecash1 and :f_datecash2 and oh.f_state=:f_headerstate "
            "and (ob.f_state=:f_bodystate1 or ob.f_state=:f_bodystate2) and r.f_goods is not null and ob.f_store=:f_store "
            "group by 1, 2");
    while (db.nextRow()) {
        QList<QVariant> row;
        row << db.getInt("f_goods")
            << db.getString("f_name")
            << db.getDouble("f_qty")
            << QVariant()
            << QVariant()
            << QVariant()
            << QVariant()
            << QVariant()
            << QVariant()
               ;
        rows.append(row);
        goodsMap[db.getInt("f_goods")] = rows.count() - 1;
    }
    /* get store at the date end */
    db[":f_store"] = f->store();
    db[":f_date"] = f->date2();
    db.exec("select s.f_base, s.f_goods, sum(s.f_qty*s.f_type) as f_qty, s.f_price, sum(s.f_total*s.f_type) as f_total "
                "from a_store s "
                "inner join a_header d on d.f_id=s.f_document "
                "where s.f_store=:f_store and d.f_date<=:f_date "
                "group by 1, 2, 4 "
                "having sum(s.f_qty*s.f_type) > 0.001");
    while (db.nextRow()) {
        if (!goodsMap.contains(db.getInt(1))) {
            continue;
        }
        int r = goodsMap[db.getInt(1)];
        rows[r][3] = db.getDouble("f_qty");
        rows[r][4] = db.getDouble("f_total");
    }
    /* get inventory quantities */
    db[":f_date"] = f->date2();
    db[":f_store"] = f->store();
    db[":f_type"] = DOC_TYPE_STORE_INVENTORY;
    db.exec("select i.f_goods, sum(i.f_qty) as f_qty "
            "from a_store_inventory i "
            "inner join a_header h on h.f_id=i.f_document "
            "where h.f_date=:f_date and h.f_type=:f_type "
            "group by 1 ");
    while (db.nextRow()) {
        if (!goodsMap.contains(db.getInt(0))) {
            continue;
        }
        int r = goodsMap[db.getInt(0)];
        rows[r][7] = db.getDouble("f_qty");
    }
    /* final calculation */
    for (int i = 0, count = rows.count(); i < count; i++) {
        rows[i][5] = rows[i][3].toDouble() - rows[i][2].toDouble();
        if (rows[i][3].toDouble() > 0.001) {
            rows[i][6] = (rows[i][4].toDouble() / rows[i][3].toDouble()) * rows[i][5].toDouble();
        }
//        if (rows[i][7].toDouble() > 0.001) {

//        } else {
//            rows[i][8] =
//        }
        if (rows[i][5].toDouble() > 0.001) {
            rows[i][8] = rows[i][2].toDouble();
        } else {
            rows[i][8] = 0;
        }
    }

    fModel->setExternalData(fColumnNameIndex, fTranslation);
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
        db2[":f_document"] = db.getInt(0);
        db2.exec("select * from a_store_inventory where f_document=:f_document");
        if (db2.nextRow()) {
            if (!result.isEmpty()) {
                result += ",";
            }
            result += db.getString(0);
        }
    }
    if (result.isEmpty()) {
        db[":f_state"] = DOC_STATE_SAVED;
        db[":f_type"] = DOC_TYPE_STORE_INVENTORY;
        db[":f_operator"] = __userid;
        db[":f_date"] = QDate::currentDate();
        db[":f_createDate"] = QDate::currentDate();
        db[":f_createTime"] = QTime::currentTime();
        db[":f_partner"] = 0;
        db[":f_amount"] = 0;
        db[":f_comment"] = tr("Created automaticaly");
        db[":f_raw"] = "";
        result = QString::number(db.insert("a_header"));
    }
    return result;
}

void CR5ConsumptionBySales::tblDoubleClicked(int row, int column, const QList<QVariant> &values)
{
    bool ok;
    double qty;
    QString docid;
    CR5ConsumptionBySalesFilter *f = static_cast<CR5ConsumptionBySalesFilter*>(fFilterWidget);
    C5Database db(fDBParams);
    switch (column) {
    case 7:
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
        db[":f_document"] = docid.split(",", QString::SkipEmptyParts).at(0);
        db[":f_store"] = f->store();
        db[":f_goods"] = values.at(0);
        db[":f_qty"] = qty;
        db[":f_price"] = 0;
        db[":f_total"] = 0;
        db.insert("a_store_inventory", false);
        fModel->setData(row, column, qty);
        break;
    }
}

void CR5ConsumptionBySales::makeOutput(bool v)
{
    Q_UNUSED(v);
    QMap<int, double> goods;
    QList<QList<QVariant> > &rows = fModel->fRawData;
    for (int i = 0; i < rows.count(); i++) {
        if (rows[i][8].toDouble() > 0.001) {
            goods[rows[i][0].toInt()] = rows[i][8].toDouble();
        }
    }
    if (goods.count() > 0) {
        C5Database db(fDBParams);
        QJsonObject jo;
        jo["f_storein"] = "";
        jo["f_storeout"] = QString::number(static_cast<CR5ConsumptionBySalesFilter*>(fFilterWidget)->store());
        QJsonDocument jd(jo);
            db[":f_id"] = 0;
        int docid = db.insert("a_header");
        db[":f_state"] = DOC_STATE_DRAFT;
        db[":f_type"] = DOC_TYPE_STORE_OUTPUT;
        db[":f_operator"] = __userid;
        db[":f_date"] = static_cast<CR5ConsumptionBySalesFilter*>(fFilterWidget)->date2();
        db[":f_createDate"] = QDate::currentDate();
        db[":f_createTime"] = QTime::currentTime();
        db[":f_partner"] = 0;
        db[":f_amount"] = 0;
        db[":f_comment"] = tr("Sale");
        db[":f_raw"] = jd.toJson();
        db.update("a_header", where_id(docid));
        for (QMap<int, double>::const_iterator it = goods.begin(); it != goods.end(); it++) {
            db[":f_id"] = 0;
            int rec = db.insert("a_store_draft");
            db[":f_document"] = docid;
            db[":f_goods"] = it.key();
            db[":f_qty"] = it.value();
            db[":f_price"] = 0;
            db[":f_total"] = 0;
            db[":f_reason"] = DOC_REASON_SALE;
            db.update("a_store_draft", where_id(rec));
        }
        C5StoreDoc *sd = __mainWindow->createTab<C5StoreDoc>(fDBParams);
        if (!sd->openDoc(docid)) {
            __mainWindow->removeTab(sd);
        }
    }
}
