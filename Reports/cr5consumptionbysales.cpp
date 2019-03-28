#include "cr5consumptionbysales.h"
#include "cr5consumptionbysalesfilter.h"
#include "c5gridgilter.h"
#include "c5tablemodel.h"
#include "c5mainwindow.h"
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
    fColumnNameIndex["f_qtyout"] = 3;
    fColumnNameIndex["f_qtystore"] = 4;
    fColumnNameIndex["f_qtyafter"] = 5;
    fColumnNameIndex["f_qtyinv"] = 6;
    fColumnNameIndex["f_qtydiff"] = 7;

    fColumnsSum << "f_qtysale"
                << "f_qtyout"
                << "f_qtystore"
                << "f_qtyafter"
                << "f_qtyinv"
                << "f_qtydiff";

    fTranslation["f_goodsid"] = tr("Goods code");
    fTranslation["f_goodsname"] = tr("Goods name");
    fTranslation["f_qtysale"] = tr("Qty, sale");
    fTranslation["f_qtyout"] = tr("Qty, out");
    fTranslation["f_qtystore"] = tr("Qty, store");
    fTranslation["f_qtyafter"] = tr("Qty, after");
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
        QAction *a = new QAction(QIcon(":/goods.png"), tr("Make\ninput/output"), this);
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
               ;
        rows.append(row);
        goodsMap[db.getInt("f_goods")] = rows.count() - 1;
    }
    /* get already writed output based on sales */
    db[":f_store"] = f->store();
    db[":f_date1"] = f->date1();
    db[":f_date2"] = f->date2();
    db[":f_reason"] = DOC_REASON_SALE;
    db[":f_state"] = DOC_STATE_SAVED;
    db.exec("select s.f_goods, sum(s.f_qty) "
            "from a_store s "
            "left join a_header h on h.f_id=s.f_document "
            "where h.f_date between :f_date1 and :f_date2 and s.f_store=:f_store "
            "and s.f_reason=:f_reason and h.f_state=:f_state "
            "group by 1 ");
    while (db.nextRow()) {
        if (!goodsMap.contains(db.getInt(0))) {
            continue;
        }
        int r = goodsMap[db.getInt(0)];
        rows[r][3] = db.getDouble(1);
    }
    /* get store at the date end */
    db[":f_store"] = f->store();
    db[":f_date"] = f->date2();
    db.exec("select s.f_base, s.f_goods, sum(s.f_qty*s.f_type) as f_qty "
                "from a_store s "
                "inner join a_header d on d.f_id=s.f_document "
                "where s.f_store=:f_store and d.f_date<=:f_date "
                "group by 1, 2 "
                "having sum(s.f_qty*s.f_type) > 0.001");
    while (db.nextRow()) {
        if (!goodsMap.contains(db.getInt(1))) {
            continue;
        }
        int r = goodsMap[db.getInt(1)];
        rows[r][4] = db.getDouble("f_qty");
    }
    /* get counted qty */
    for (int i = 0; i < rows.count(); i++) {
        rows[i][5] = rows[i][4].toDouble() - rows[i][2].toDouble();
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
        if (!goodsMap.contains(db.getInt(0))) {
            QList<QVariant> row;
            row << db.getInt("f_goods")
                << db.getString("f_name")
                << db.getDouble("f_qty")
                << QVariant()
                << QVariant()
                << QVariant()
                << QVariant()
                << QVariant()
                   ;
            goodsMap[db.getInt(0)] = rows.count();
            rows.append(row);
        }
        int r = goodsMap[db.getInt(0)];
        rows[r][6] = db.getDouble("f_qty");
    }
    /* final calculation */
    for (int i = 0, count = rows.count(); i < count; i++) {
        rows[i][7] = rows[i][6].toDouble() - rows[i][5].toDouble() + rows[i][2].toDouble();
    }

    fModel->setExternalData(fColumnNameIndex, fTranslation);
    //sumColumnsData();
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
    case 6:
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
        fModel->setData(row, 7, fModel->data(row, 6, Qt::EditRole).toDouble()
                        - fModel->data(row, 5, Qt::EditRole).toDouble()
                        + fModel->data(row, 2, Qt::EditRole).toDouble());
        break;
    }
}

void CR5ConsumptionBySales::makeOutput(bool v)
{
    Q_UNUSED(v);
    QMap<int, double> goodsSale, goodsLost, goodsOver;
    QList<QList<QVariant> > &rows = fModel->fRawData;
    for (int i = 0; i < rows.count(); i++) {
        double qty = rows[i][4].toDouble() - rows[i][5].toDouble() - rows[i][3].toDouble();
        double qtyLost = rows[i][5].toDouble() - rows[i][6].toDouble() - rows[i][2].toDouble();
        double qtyOver = 0.0;
        if (qtyLost < 0.0001) {
            qtyOver = -1 * qtyLost;
            qtyLost = 0.0;
        }
        if (rows[i][7].toDouble() > rows[i][2].toDouble()) {
            qty = 0;
        }
        if (qtyOver > 0.0001 && rows[i][7].toDouble() <= rows[i][2].toDouble()) {
            qty -= qtyOver;
        }
        if (qty > 0.0001) {
            goodsSale[rows[i][0].toInt()] = qty;
        }
        if (qtyLost > 0.0001) {
            goodsLost[rows[i][0].toInt()] = qtyLost;
        }
        if (qtyOver > 0.0001) {
            goodsOver[rows[i][0].toInt()] = qtyOver;
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
        db[":f_id"] = 0;
        int rec = db.insert("a_store_draft");
        db[":f_document"] = docid;
        db[":f_goods"] = it.key();
        db[":f_qty"] = it.value();
        db[":f_price"] = 0;
        db[":f_total"] = 0;
        db[":f_reason"] = reason;
        db.update("a_store_draft", where_id(rec));
    }
    C5StoreDoc *sd = __mainWindow->createTab<C5StoreDoc>(fDBParams);
    if (!sd->openDoc(docid)) {
        __mainWindow->removeTab(sd);
    }
}
