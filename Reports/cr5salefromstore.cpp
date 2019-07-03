#include "cr5salefromstore.h"
#include "cr5salefromstorefilter.h"
#include "c5database.h"
#include "c5tablemodel.h"
#include "c5mainwindow.h"
#include "c5storedoc.h"
#include "c5salefromstoreorder.h"
#include <QJsonObject>
#include <QJsonDocument>

CR5SaleFromStore::CR5SaleFromStore(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{

    fIcon = ":/graph.png";
    fLabel = tr("Sales, common");
    fSimpleQuery = false;

    fMainTable = "o_goods og";
    fLeftJoinTables << "left join o_header oh on oh.f_id=og.f_header [oh]"
                    << "left join c_storages s on s.f_id=og.f_store [s]"
                    << "left join c_goods gg on gg.f_id=og.f_goods [gg]"
                    << "left join c_groups gr on gr.f_id=gg.f_group [gr]"
                    << "left join c_units gu on gu.f_id=gg.f_unit [gu]"
                       ;

    fColumnsFields << "oh.f_id as f_header"
                   << "concat(oh.f_prefix, oh.f_hallid) as f_number"
                   << "oh.f_datecash"
                   << "s.f_name as f_storename"
                   << "gg.f_name as f_goodsname"
                   << "gr.f_name as f_goodsgroup"
                   << "gu.f_name as f_goodsunit"
                   << "og.f_tax"
                   << "sum(og.f_qty*og.f_sign) as f_qty"
                   << "sum(og.f_total*og.f_sign) as f_total"
                      ;

    fColumnsGroup << "oh.f_id as f_header"
                  << "concat(oh.f_prefix, oh.f_hallid) as f_number"
                   << "oh.f_datecash"
                   << "s.f_name as f_storename"
                   << "gg.f_name as f_goodsname"
                   << "gr.f_name as f_groodsgroup"
                   << "gu.f_name as f_goodsunit"
                   << "og.f_tax"
                      ;

    fColumnsSum << "f_qty"
                << "f_total"
                      ;

    fTranslation["f_header"] = tr("UUID");
    fTranslation["f_number"] = tr("Number");
    fTranslation["f_datecash"] = tr("Date, cash");
    fTranslation["f_storename"] = tr("Store");
    fTranslation["f_goodsname"] = tr("Goods");
    fTranslation["f_goodsgroup"] = tr("Group");
    fTranslation["f_goodsunit"] = tr("Unit");
    fTranslation["f_tax"] = tr("TAX");
    fTranslation["f_qty"] = tr("Qty");
    fTranslation["f_total"] = tr("Total");

    fColumnsVisible["oh.f_id as f_header"] = true;
    fColumnsVisible["concat(oh.f_prefix, oh.f_hallid) as f_number"] = true;
    fColumnsVisible["oh.f_datecash"] = true;
    fColumnsVisible["s.f_name as f_storename"] = true;
    fColumnsVisible["gg.f_name as f_goodsname"] = true;
    fColumnsVisible["gr.f_name as f_goodsgroup"] = true;
    fColumnsVisible["gu.f_name as f_goodsunit"] = true;
    fColumnsVisible["og.f_tax"] = true;
    fColumnsVisible["sum(og.f_qty*og.f_sign) as f_qty"] = true;
    fColumnsVisible["sum(og.f_total*og.f_sign) as f_total"] = true;

    restoreColumnsVisibility();
    fFilterWidget = new CR5SaleFromStoreFilter(fDBParams);
    fFilter = static_cast<CR5SaleFromStoreFilter*>(fFilterWidget);

    connect(this, SIGNAL(tblDoubleClicked(int,int,QList<QVariant>)), this, SLOT(tblClick(int,int,QList<QVariant>)));
}

QToolBar *CR5SaleFromStore::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbFilter
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        fToolBar = createStandartToolbar(btn);
        QAction *aout = new QAction(QIcon(":/storehouse.png"), tr("Make output"), this);
        connect(aout, SIGNAL(triggered(bool)), this, SLOT(makeOutput(bool)));
        fToolBar->addAction(aout);
    }
    return fToolBar;
}

void CR5SaleFromStore::restoreColumnsWidths()
{
    C5Grid::restoreColumnsWidths();
    if (fColumnsVisible["oh.f_id as f_header"]) {
        fTableView->setColumnWidth(fModel->fColumnNameIndex["f_header"], 0);
    }
}

void CR5SaleFromStore::makeOutput(bool v)
{
    Q_UNUSED(v);
    C5Database db(fDBParams);
    C5Database db1(fDBParams);
    db[":f_datecash1"] = fFilter->d1();
    db[":f_datecash2"] = fFilter->d2();
    db[":f_state"] = 0;
    db.exec("select og.f_store, og.f_goods, sum(og.f_qty * og.f_sign) as f_qty "
            "from o_goods og "
            "left join o_header oh on oh.f_id=og.f_header "
            "where oh.f_datecash between :f_datecash1 and :f_datecash2 "
            "and oh.f_state=:f_state "
            "group by 1, 2 "
            "order by 1");
    int store = 0;
    QString docid;
    bool nostorewarning = false;
    while (db.nextRow()) {
        if (db.getDouble(2) < 0.0001) {
            continue;
        }
        if (store == 0 || store != db.getInt(0)) {
            if (!docid.isEmpty()) {
                C5StoreDoc *sd = __mainWindow->createTab<C5StoreDoc>(fDBParams);
                if (!sd->openDoc(docid)) {
                    __mainWindow->removeTab(sd);
                }
            }
            store = db.getInt(0);
            if (store == 0) {
                nostorewarning = true;
                continue;
            }
            QJsonObject jo;
            jo["f_storein"] = "";
            jo["f_storeout"] = db.getString(0);
            QJsonDocument jd(jo);
            docid = C5Database::uuid();
            db1[":f_state"] = DOC_STATE_DRAFT;
            db1[":f_type"] = DOC_TYPE_STORE_OUTPUT;
            db1[":f_operator"] = __userid;
            db1[":f_date"] = fFilter->d2();
            db1[":f_createDate"] = QDate::currentDate();
            db1[":f_createTime"] = QTime::currentTime();
            db1[":f_partner"] = 0;
            db1[":f_amount"] = 0;
            db1[":f_comment"] = "";
            db1[":f_raw"] = jd.toJson();
            db1[":f_id"] = docid;
            db1.insert("a_header", false);
        }
        db1[":f_id"] = C5Database::uuid();
        db1[":f_document"] = docid;
        db1[":f_goods"] = db.getInt(1);
        db1[":f_qty"] = db.getDouble(2);
        db1[":f_price"] = 0;
        db1[":f_total"] = 0;
        db1[":f_reason"] = DOC_REASON_SALE;
        db1.insert("a_store_draft");
    }
    C5StoreDoc *sd = __mainWindow->createTab<C5StoreDoc>(fDBParams);
    if (!sd->openDoc(docid)) {
        __mainWindow->removeTab(sd);
    }
    if (nostorewarning) {
        C5Message::error(tr("Not for all items store defined"));
    }
}

void CR5SaleFromStore::tblClick(int row, int column, const QList<QVariant> &values)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    if (!fColumnsVisible["oh.f_id as f_header"]) {
        C5Message::info(tr("Column 'Header' must be checked in filter"));
        return;
    }
    if (values.count() == 0) {
        return;
    }
    C5SaleFromStoreOrder::openOrder(fDBParams, values.at(fModel->indexForColumnName("f_header")).toString());
}
