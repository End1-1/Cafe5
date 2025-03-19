#include "cr5salefromstore.h"
#include "cr5salefromstorefilter.h"
#include "c5tablemodel.h"
#include "c5mainwindow.h"
#include "c5saledoc.h"
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
                    << "left join o_tax ot on ot.f_id=oh.f_id [ot]"
                    << "left join o_header_flags ohf on ohf.f_id=oh.f_id [ohf]"
                    << "left join c_storages s on s.f_id=og.f_store [s]"
                    << "left join c_goods gg on gg.f_id=og.f_goods [gg]"
                    << "left join c_groups gr on gr.f_id=gg.f_group [gr]"
                    << "left join c_units gu on gu.f_id=gg.f_unit [gu]"
                    << "left join c_goods_prices gpr on gpr.f_goods=gg.f_id [gpr]"
                    << "left join c_partners cpb on cpb.f_id=oh.f_partner [cpb]"
                    << "left join a_store_draft asd on asd.f_id=og.f_storerec [asd]"
                    << "left join h_halls hl on hl.f_id=oh.f_hall [hl]"
                    << "left join o_goods og2 on og2.f_id=og.f_returnfrom [og2]"
                    << "left join o_goods_return_reason grr on grr.f_id=og2.f_return [grr]"
                    << "left join e_currency_cross_rate cr on cr.f_currency1=ah.f_currency and cr.f_currency2=1 [cr]"
                    ;
    fColumnsFields << "oh.f_id as f_header"
                   << "ot.f_receiptnumber"
                   << "concat(oh.f_prefix, oh.f_hallid) as f_number"
                   << "oh.f_datecash"
                   << "hl.f_name as f_hallname"
                   << "s.f_name as f_storename"
                   << "gr.f_name as f_goodsgroup"
                   << "gr.f_class as f_goodsclass"
                   << "gg.f_name as f_goodsname"
                   << "gg.f_scancode"
                   << "grr.f_name as f_returnreason"
                   << "gu.f_name as f_goodsunit"
                   << "concat_ws(', ', cpb.f_name, cpb.f_taxname, cpb.f_address) as f_buyer"
                   << "cpb.f_taxcode as f_buyertaxcode"
                   << "og.f_discountfactor*100 as f_discountfactor"
                   << "og.f_price"
                   << "gpr.f_price1"
                   << "gpr.f_price2"
                   << "sum(gpr.f_price1*og.f_qty) as f_totalsaleprice"
                   << "sum(gpr.f_price2*og.f_qty) as f_totalsaleprice2"
                   << "sum(og.f_discountamount) as f_discamount"
                   << "sum(og.f_qty*og.f_sign) as f_qty"
                   << "sum(og.f_total*og.f_sign) as f_total"
                   << "asd.f_price as f_selfcost"
                   << "sum(asd.f_price*og.f_qty*og.f_sign) as f_selfcosttotal"
                   ;
    fColumnsGroup << "oh.f_id as f_header"
                  << "ot.f_receiptnumber"
                  << "concat(oh.f_prefix, oh.f_hallid) as f_number"
                  << "oh.f_datecash"
                  << "hl.f_name as f_hallname"
                  << "gr.f_class"
                  << "grr.f_name as f_returnreason"
                  << "s.f_name as f_storename"
                  << "gg.f_name as f_goodsname"
                  << "gg.f_scancode"
                  << "gr.f_name as f_goodsgroup"
                  << "gu.f_name as f_goodsunit"
                  << "concat_ws(', ', cpb.f_name, cpb.f_taxname, cpb.f_address) as f_buyer"
                  << "cpb.f_taxcode as f_buyertaxcode"
                  << "og.f_discountfactor*100 as f_discountfactor"
                  << "og.f_price"
                  << "gpr.f_price1"
                  << "gpr.f_price2"
                  << "asd.f_price as f_selfcost"
                  ;
    fColumnsSum << "f_qty"
                << "f_total"
                << "f_discamount"
                << "f_totalsaleprice"
                << "f_totalsaleprice2"
                << "f_selfcosttotal"
                ;
    fTranslation["f_header"] = tr("UUID");
    fTranslation["f_receiptnumber"] = tr("Tax receipt");
    fTranslation["f_number"] = tr("Number");
    fTranslation["f_datecash"] = tr("Date, cash");
    fTranslation["f_storename"] = tr("Store");
    fTranslation["f_goodsname"] = tr("Goods");
    fTranslation["f_goodsclass"] = tr("Class");
    fTranslation["f_scancode"] = tr("Scancode");
    fTranslation["f_goodsgroup"] = tr("Group");
    fTranslation["f_returnreason"] = tr("Return reason");
    fTranslation["f_goodsunit"] = tr("Unit");
    fTranslation["f_qty"] = tr("Qty");
    fTranslation["f_total"] = tr("Total");
    fTranslation["f_groupname"] = tr("Group");
    fTranslation["f_hallname"] = tr("Hall");
    fTranslation["f_buyer"] = tr("Buyer");
    fTranslation["f_buyertaxcode"] = tr("Buyer taxcode");
    fTranslation["f_discountfactor"] = tr("Discount factor");
    fTranslation["f_discamount"] = tr("Discount amount");
    fTranslation["f_price"] = tr("Price");
    fTranslation["f_price1"] = tr("Sale price");
    fTranslation["f_price2"] = tr("Whosale price");
    fTranslation["f_totalsaleprice"] = tr("Total of retail price");
    fTranslation["f_totalsaleprice2"] = tr("Total of whosale price");
    fTranslation["f_selfcost"] = tr("Selfcost");
    fTranslation["f_selfcosttotal"] = tr("Selfcost total");
    fColumnsVisible["oh.f_id as f_header"] = true;
    fColumnsVisible["ot.f_receiptnumber"] = true;
    fColumnsVisible["concat(oh.f_prefix, oh.f_hallid) as f_number"] = true;
    fColumnsVisible["oh.f_datecash"] = true;
    fColumnsVisible["s.f_name as f_storename"] = true;
    fColumnsVisible["gr.f_class as f_goodsclass"] = false;
    fColumnsVisible["gg.f_name as f_goodsname"] = true;
    fColumnsVisible["gg.f_scancode"] = false;
    fColumnsVisible["gr.f_name as f_goodsgroup"] = true;
    fColumnsVisible["gu.f_name as f_goodsunit"] = true;
    fColumnsVisible["grr.f_name as f_returnreason"] = false;
    fColumnsVisible["sum(og.f_qty*og.f_sign) as f_qty"] = true;
    fColumnsVisible["sum(og.f_total*og.f_sign) as f_total"] = true;
    fColumnsVisible["hl.f_name as f_hallname"] = true;
    fColumnsVisible["concat_ws(', ', cpb.f_name, cpb.f_taxname, cpb.f_address) as f_buyer"] = false;
    fColumnsVisible["cpb.f_taxcode as f_buyertaxcode"] = false;
    fColumnsVisible["og.f_discountfactor*100 as f_discountfactor"] = false;
    fColumnsVisible["sum(og.f_discountamount) as f_discamount"] = false;
    fColumnsVisible["og.f_price"] = false;
    fColumnsVisible["gpr.f_price1"] = false;
    fColumnsVisible["gpr.f_price2"] = false;
    fColumnsVisible["sum(gpr.f_price1*og.f_qty) as f_totalsaleprice"] = false;
    fColumnsVisible["sum(gpr.f_price2*og.f_qty) as f_totalsaleprice2"] = false;
    fColumnsVisible["asd.f_price as f_selfcost"] = false;
    fColumnsVisible["sum(asd.f_price*og.f_qty*og.f_sign) as f_selfcosttotal"] = false;
    restoreColumnsVisibility();
    fFilterWidget = new CR5SaleFromStoreFilter(fDBParams);
    fFilter = static_cast<CR5SaleFromStoreFilter *>(fFilterWidget);
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

bool CR5SaleFromStore::tblDoubleClicked(int row, int column, const QJsonArray &values)
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
    auto *doc = __mainWindow->createTab<C5SaleDoc>(fDBParams);
    doc->openDoc(values.at(fModel->indexForColumnName("f_header")).toString());
    return true;
}
