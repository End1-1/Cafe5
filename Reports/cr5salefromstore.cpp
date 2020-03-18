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
                    << "left join c_partners cp on cp.f_id=gg.f_supplier [cp]"
                    << "left join c_goods_classes gca on gca.f_id=gg.f_group1 [gca]"
                    << "left join c_goods_classes gcb on gcb.f_id=gg.f_group2 [gcb]"
                    << "left join c_goods_classes gcc on gcc.f_id=gg.f_group3 [gcc]"
                    << "left join c_goods_classes gcd on gcd.f_id=gg.f_group4 [gcd]"
                       ;

    fColumnsFields << "oh.f_id as f_header"
                   << "concat(oh.f_prefix, oh.f_hallid) as f_number"
                   << "oh.f_datecash"
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
                   << "sum(og.f_qty*og.f_sign) as f_qty"
                   << "sum(og.f_total*og.f_sign) as f_total"
                      ;

    fColumnsGroup << "oh.f_id as f_header"
                  << "concat(oh.f_prefix, oh.f_hallid) as f_number"
                   << "oh.f_datecash"
                   << "s.f_name as f_storename"
                   << "gg.f_name as f_goodsname"
                   << "gg.f_scancode"
                   << "gr.f_name as f_groodsgroup"
                   << "gu.f_name as f_goodsunit"
                   << "og.f_tax"
                   << "cp.f_taxname"
                   << "gca.f_name as gname1"
                   << "gcb.f_name as gname2"
                   << "gcc.f_name as gname3"
                   << "gcd.f_name as gname4"
                      ;

    fColumnsSum << "f_qty"
                << "f_total"
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
