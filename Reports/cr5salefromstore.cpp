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
    fLabel = tr("Sales by goods");
    fSimpleQuery = false;

    fMainTable = "o_goods og";
    fLeftJoinTables << "left join o_header oh on oh.f_id=og.f_header [oh]"
                    << "left join o_header_flags ohf on ohf.f_id=oh.f_id [ohf]"
                    << "left join c_storages s on s.f_id=og.f_store [s]"
                    << "left join c_goods gg on gg.f_id=og.f_goods [gg]"
                    << "left join c_groups gr on gr.f_id=gg.f_group [gr]"
                    << "left join c_units gu on gu.f_id=gg.f_unit [gu]"
                    << "left join c_partners cp on cp.f_id=gg.f_supplier [cp]"
                    << "left join c_partners cpb on cpb.f_id=oh.f_partner [cpb]"
                    << "left join c_goods_classes gca on gca.f_id=gg.f_group1 [gca]"
                    << "left join c_goods_classes gcb on gcb.f_id=gg.f_group2 [gcb]"
                    << "left join c_goods_classes gcc on gcc.f_id=gg.f_group3 [gcc]"
                    << "left join c_goods_classes gcd on gcd.f_id=gg.f_group4 [gcd]"
                    << "left join a_store_draft ad on ad.f_id=og.f_storerec [ad]"
                    << "left join h_halls hl on hl.f_id=oh.f_hall [hl]"
                       ;

    fColumnsFields << "oh.f_id as f_header"
                   << "concat(oh.f_prefix, oh.f_hallid) as f_number"
                   << "oh.f_datecash"
                   << "hl.f_name as f_hallname"
                   << "s.f_name as f_storename"
                   << "cp.f_taxname"
                   << "gr.f_name as f_goodsgroup"
                   << "gg.f_name as f_goodsname"
                   << "gg.f_scancode"
                   << "gu.f_name as f_goodsunit"
                   << "og.f_tax"
                   << "gca.f_name as gname1"
                   << "gcb.f_name as gname2"
                   << "gcc.f_name as gname3"
                   << "gcd.f_name as gname4"
                   << "cpb.f_taxname as f_buyer"
                   << "cpb.f_taxcode as f_buyertaxcode"
                   << "og.f_discountfactor*100 as f_discountfactor"
                   << "gg.f_saleprice"
                   << "gg.f_saleprice2"
                   << "sum(gg.f_saleprice*og.f_qty) as f_totalsaleprice"
                   << "sum(gg.f_saleprice2*og.f_qty) as f_totalsaleprice2"
                   << "sum(og.f_discountamount) as f_discamount"
                   << "sum(og.f_qty*og.f_sign) as f_qty"
                   << "sum(og.f_total*og.f_sign) as f_total"
                   << "sum(ad.f_total*og.f_sign) as f_selfcost"
                      ;

    fColumnsGroup << "oh.f_id as f_header"
                  << "concat(oh.f_prefix, oh.f_hallid) as f_number"
                   << "oh.f_datecash"
                   << "hl.f_name as f_hallname"
                   << "s.f_name as f_storename"
                   << "gg.f_name as f_goodsname"
                   << "gg.f_scancode"
                   << "gr.f_name as f_goodsgroup"
                   << "gu.f_name as f_goodsunit"
                   << "og.f_tax"
                   << "cp.f_taxname"
                   << "cpb.f_taxname as f_buyer"
                   << "cpb.f_taxcode as f_buyertaxcode"
                   << "gca.f_name as gname1"
                   << "gcb.f_name as gname2"
                   << "gcc.f_name as gname3"
                   << "gcd.f_name as gname4"
                   << "og.f_discountfactor*100 as f_discountfactor"
                   << "gg.f_saleprice"
                   << "gg.f_saleprice2"
                      ;

    fColumnsSum << "f_qty"
                << "f_total"
                << "f_selfcost"
                << "f_discamount"
                << "gg.f_saleprice"
                << "gg.f_saleprice2"
                << "f_totalsaleprice"
                << "f_totalsaleprice2"
                      ;

    fTranslation["f_header"] = tr("UUID");
    fTranslation["f_number"] = tr("Number");
    fTranslation["f_datecash"] = tr("Date, cash");
    fTranslation["f_storename"] = tr("Store");
    fTranslation["f_goodsname"] = tr("Goods");
    fTranslation["f_scancode"] = tr("Scancode");
    fTranslation["f_goodsgroup"] = tr("Group");
    fTranslation["f_goodsunit"] = tr("Unit");
    fTranslation["f_tax"] = tr("TAX");
    fTranslation["f_qty"] = tr("Qty");
    fTranslation["f_total"] = tr("Total");
    fTranslation["gname1"] = tr("Class 1");
    fTranslation["gname2"] = tr("Class 2");
    fTranslation["gname3"] = tr("Class 3");
    fTranslation["gname4"] = tr("Class 4");
    fTranslation["f_taxname"] = tr("Supplier");
    fTranslation["f_groupname"] = tr("Group");
    fTranslation["f_selfcost"] = tr("Selfcost");
    fTranslation["f_hallname"] = tr("Hall");
    fTranslation["f_buyer"] = tr("Buyer");
    fTranslation["f_buyertaxcode"] = tr("Buyer taxcode");
    fTranslation["f_discountfactor"] = tr("Discount factor");
    fTranslation["f_discamount"] = tr("Discount amount");
    fTranslation["f_saleprice"] = tr("Sale price");
    fTranslation["f_saleprice2"] = tr("Whosale price");
    fTranslation["f_totalsaleprice"] = tr("Total of retail price");
    fTranslation["f_totalsaleprice2"] = tr("Total of whosale price");

    fColumnsVisible["oh.f_id as f_header"] = true;
    fColumnsVisible["concat(oh.f_prefix, oh.f_hallid) as f_number"] = true;
    fColumnsVisible["oh.f_datecash"] = true;
    fColumnsVisible["cp.f_taxname"] = true;
    fColumnsVisible["s.f_name as f_storename"] = true;
    fColumnsVisible["gg.f_name as f_goodsname"] = true;
    fColumnsVisible["gg.f_scancode"] = false;
    fColumnsVisible["gr.f_name as f_goodsgroup"] = true;
    fColumnsVisible["gu.f_name as f_goodsunit"] = true;
    fColumnsVisible["og.f_tax"] = true;
    fColumnsVisible["sum(og.f_qty*og.f_sign) as f_qty"] = true;
    fColumnsVisible["sum(og.f_total*og.f_sign) as f_total"] = true;
    fColumnsVisible["gca.f_name as gname1"] = true;
    fColumnsVisible["gcb.f_name as gname2"] = true;
    fColumnsVisible["gcc.f_name as gname3"] = true;
    fColumnsVisible["gcd.f_name as gname4"] = true;
    fColumnsVisible["sum(ad.f_total*og.f_sign) as f_selfcost"] = true;
    fColumnsVisible["hl.f_name as f_hallname"] = true;
    fColumnsVisible["cpb.f_taxname as f_buyer"] = false;
    fColumnsVisible["cpb.f_taxcode as f_buyertaxcode"] = false;
    fColumnsVisible["og.f_discountfactor*100 as f_discountfactor"] = false;
    fColumnsVisible["sum(og.f_discountamount) as f_discamount"] = false;
    fColumnsVisible["gg.f_saleprice"] = false;
    fColumnsVisible["gg.f_saleprice2"] = false;
    fColumnsVisible["sum(gg.f_saleprice*og.f_qty) as f_totalsaleprice"] = false;
    fColumnsVisible["sum(gg.f_saleprice2*og.f_qty) as f_totalsaleprice2"] = false;

    restoreColumnsVisibility();
    fFilterWidget = new CR5SaleFromStoreFilter(fDBParams);
    fFilter = static_cast<CR5SaleFromStoreFilter*>(fFilterWidget);
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

bool CR5SaleFromStore::tblDoubleClicked(int row, int column, const QList<QVariant> &values)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    if (!fColumnsVisible["oh.f_id as f_header"]) {
        C5Message::info(tr("Column 'Header' must be checked in filter"));
        return true;
    }
    if (values.count() == 0) {
        return true;
    }
    C5SaleFromStoreOrder::openOrder(fDBParams, values.at(fModel->indexForColumnName("f_header")).toString());
    return true;
}
