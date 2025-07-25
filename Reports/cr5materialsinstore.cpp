#include "cr5materialsinstore.h"
#include "cr5materialinstorefilter.h"
#include "c5tablemodel.h"
#include "c5mainwindow.h"
#include "c5database.h"
#include "c5storebarcode.h"
#include "ce5goods.h"

CR5MaterialsInStore::CR5MaterialsInStore(QWidget *parent) :
    C5ReportWidget( parent)
{
    fIcon = ":/goods.png";
    fLabel = tr("Materials in the store");
    fSimpleQuery = false;
    fFilterWidget = new CR5MaterialInStoreFilter();
    fFilter = static_cast<CR5MaterialInStoreFilter *>(fFilterWidget);
    if (fFilter->showDrafts()) {
        prepareDrafts();
    } else {
        prepareNoDrafts();
    }
}

QToolBar *CR5MaterialsInStore::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbFilter
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        fToolBar = createStandartToolbar(btn);
        fToolBar->addAction(QIcon(":/barcode.png"), tr("Print\nbarcode"), this, SLOT(printBarcode()));
    }
    return fToolBar;
}

void CR5MaterialsInStore::refreshData()
{
    C5Grid::refreshData();
    setColors();
    completeRefresh();
}

void CR5MaterialsInStore::prepareDrafts()
{
    fMainTable = "a_store_draft s";
    fLeftJoinTables.clear();
    fColumnsFields.clear();
    fColumnsGroup.clear();
    fColumnsSum.clear();
    fTranslation.clear();
    fColumnsVisible.clear();
    fLeftJoinTables << "inner join a_header h on h.f_id=s.f_document [h]"
                    << "inner join c_storages ss on ss.f_id=s.f_store [ss]"
                    << "left join c_units u on u.f_id=g.f_unit [u]"
                    << "inner join c_groups gg on gg.f_id=g.f_group [gg]"
                    << "left join c_goods g on g.f_id=s.f_goods [g]"
                    << "LEFT JOin c_goods_prices gpr on gpr.f_goods=g.f_id [gpr]"
                    << "left join c_partners p on p.f_id=h.f_partner [p]"
                    << "left join a_store_valid v on v.f_id=s.f_id [v]"
                    ;
    fColumnsFields << "g.f_id as f_code"
                   << "ss.f_name as f_storage"
                   << "gg.f_name as f_group"
                   << "g.f_name as f_goods"
                   << "g.f_scancode"
                   << "p.f_taxname"
                   << "round(sum(s.f_qty*s.f_type),3) as f_qty"
                   << "u.f_name as f_unit"
                   << "g.f_lastinputprice"
                   << "sum(g.f_lastinputprice*s.f_type*s.f_qty) as f_total"
                   << "g.f_lowlevel"
                   << "gpr.f_price1"
                   << "grp.f_price1disc"
                   << "sum(s.f_qty*s.f_type)*gpr.f_price1 as f_totalsale"
                   << "gpr.f_price2"
                   << "grp.f_price2dics"
                   << "sum(s.f_qty*s.f_type)*gpr.f_price2 as f_totalsale2"
                   << "sum(s.f_qty*s.f_type)*if(gpr.f_price1disc>0,gpr.f_price1disc,gpr.f_price1) as f_totalretaildiscounted"
                   << "sum(s.f_qty*s.f_type)*if(gpr.f_price2disc>0,gpr.f_price2disc,gpr.f_price2) as f_totalsale2discount"
                   ;
    fColumnsGroup << "g.f_id as f_code"
                  << "ss.f_name as f_storage"
                  << "gg.f_name as f_group"
                  << "g.f_lastinputprice"
                  << "u.f_name as f_unit"
                  << "g.f_name as f_goods"
                  << "g.f_lowlevel"
                  << "gpr.f_price1"
                  << "gpr.f_price2"
                  << "gpr.f_price1disc"
                  << "gpr.f_price2disc"
                  << "p.f_taxname"
                  << "g.f_scancode"
                  << "v.f_date as f_validdate"
                  ;
    fColumnsSum << "f_qty"
                << "f_total"
                << "f_totalsale"
                << "f_totalsale2"
                << "f_totalretaildiscounted"
                << "f_totalsale2discount"
                ;
    if (fFilter->showZero()) {
        fHavindCondition = "";
    } else {
        fHavindCondition = " having sum(s.f_qty*s.f_type) <> 0 ";
    }
    fTranslation["f_code"] = tr("Code");
    fTranslation["f_storage"] = tr("Storage");
    fTranslation["f_group"] = tr("Group");
    fTranslation["f_taxname"] = tr("Partner");
    fTranslation["f_goods"] = tr("Goods");
    fTranslation["f_scancode"] = tr("Scancode");
    fTranslation["f_qty"] = tr("Qty");
    fTranslation["f_unit"] = tr("Unit");
    fTranslation["f_lastinputprice"] = tr("Price");
    fTranslation["f_total"] = tr("Amount");
    fTranslation["f_lowlevel"] = tr("Warning");
    fTranslation["f_price1"] = tr("Retail price");
    fTranslation["f_totalsale"] = tr("Retail amount");
    fTranslation["f_price1disc"] = tr("Retail discounted");
    fTranslation["f_price2disc"] = tr("Whosale discounted");
    fTranslation["f_totalretaildiscounted"] = tr("Total retail discounted");
    fTranslation["f_totalsale2discount"] = tr("Total whosale discounted");
    fTranslation["f_price2"] = tr("Whosale price");
    fTranslation["f_totalsale2"] = tr("Whosale amount");
    fTranslation["f_validdate"] = tr("Valid date");
    fColumnsVisible["g.f_id as f_code"] = true;
    fColumnsVisible["ss.f_name as f_storage"] = true;
    fColumnsVisible["gg.f_name as f_group"] = true;
    fColumnsVisible["g.f_name as f_goods"] = true;
    fColumnsVisible["g.f_scancode"] = true;
    fColumnsVisible["round(sum(s.f_qty*s.f_type),3) as f_qty"] = true;
    fColumnsVisible["u.f_name as f_unit"] = true;
    fColumnsVisible["g.f_lastinputprice"] = true;
    fColumnsVisible["sum(g.f_lastinputprice*s.f_type*s.f_qty) as f_total"] = true;
    fColumnsVisible["g.f_lowlevel"] = true;
    fColumnsVisible["gpr.f_price1"] = false;
    fColumnsVisible["gpr.f_price1disc"] = false;
    fColumnsVisible["sum(s.f_qty*s.f_type)*gpr.f_price1 as f_totalsale"] = false;
    fColumnsVisible["sum(s.f_qty*s.f_type)*if(gpr.f_price1disc>0,gpr.f_price1disc,gpr.f_price1) as f_totalretaildiscounted"]
        = false;
    fColumnsVisible["gpr.f_price2"] = false;
    fColumnsVisible["gpr.f_price2disc"] = false;
    fColumnsVisible["sum(s.f_qty*s.f_type)*gpr.f_price2 as f_totalsale2"] = false;
    fColumnsVisible["sum(s.f_qty*s.f_type)*if(gpr.f_price2disc>0,gpr.f_price2disc,gpr.f_price2) as f_totalsale2discount"] =
        false;
    fColumnsVisible["p.f_taxname"] = false;
    fColumnsVisible["v.f_date as f_validdate"] = false;
    restoreColumnsVisibility();
}

void CR5MaterialsInStore::prepareNoDrafts()
{
    fMainTable = "a_store s";
    fLeftJoinTables.clear();
    fColumnsFields.clear();
    fColumnsGroup.clear();
    fColumnsSum.clear();
    fTranslation.clear();
    fColumnsVisible.clear();
    fLeftJoinTables << "inner join a_header h on h.f_id=s.f_document [h]"
                    << "inner join c_storages ss on ss.f_id=s.f_store [ss]"
                    << "left join c_units u on u.f_id=g.f_unit [u]"
                    << "inner join c_groups gg on gg.f_id=g.f_group [gg]"
                    << "left join c_goods g on g.f_id=s.f_goods [g]"
                    << "left join c_goods_prices gpr on gpr.f_goods=g.f_id [gpr]"
                    << "LEFT JOin c_goods_prices gpr on gpr.f_goods=g.f_id [gpr]"
                    << "left join partners p on p.f_id=h.f_partner [p]"
                    << "left join a_store_valid v on v.f_id=s.f_id [v]"
                    ;
    fColumnsFields << "g.f_id as f_code"
                   << "ss.f_name as f_storage"
                   << "gg.f_name as f_group"
                   << "g.f_name as f_goods"
                   << "g.f_scancode"
                   << "v.f_date as f_validdate"
                   << "round(sum(s.f_qty*s.f_type),3) as f_qty"
                   << "u.f_name as f_unit"
                   << "s.f_price"
                   << "sum(s.f_total*s.f_type) as f_total"
                   << "g.f_lowlevel"
                   << "gpr.f_price1"
                   << "gpr.f_price1disc"
                   << "sum(s.f_qty*s.f_type)*gpr.f_price1 as f_totalsale"
                   << "gpr.f_price2"
                   << "gpr.f_price2disc"
                   << "sum(s.f_qty*s.f_type)*gpr.f_price2 as f_totalsale2"
                   << "sum(s.f_qty*s.f_type)*if(gpr.f_price1disc>0,gpr.f_price1disc,gpr.f_price1) as f_totalretaildiscounted"
                   << "sum(s.f_qty*s.f_type)*if(gpr.f_price2disc>0,gpr.f_price2disc,gpr.f_price2) as f_totalsale2discount"
                   ;
    fColumnsGroup << "g.f_id as f_code"
                  << "ss.f_name as f_storage"
                  << "gg.f_name as f_group"
                  << "s.f_price"
                  << "u.f_name as f_unit"
                  << "g.f_name as f_goods"
                  << "g.f_lowlevel"
                  << "gpr.f_price1"
                  << "gpr.f_price2"
                  << "gpr.f_price1disc"
                  << "gpr.f_price2dics"
                  << "v.f_date as f_validdate"
                  ;
    fColumnsSum << "f_qty"
                << "f_total"
                << "f_totalsale"
                << "f_totalsale2"
                << "f_totalretaildiscounted"
                << "f_totalsale2discount"
                ;
    if (fFilter->showZero()) {
        fHavindCondition = "";
    } else {
        fHavindCondition = " having sum(s.f_qty*s.f_type) > 0.01 ";
    }
    fTranslation["f_code"] = tr("Code");
    fTranslation["f_storage"] = tr("Storage");
    fTranslation["f_group"] = tr("Group");
    fTranslation["f_goods"] = tr("Goods");
    fTranslation["f_scancode"] = tr("Scancode");
    fTranslation["f_qty"] = tr("Qty");
    fTranslation["f_unit"] = tr("Unit");
    fTranslation["f_price"] = tr("Price");
    fTranslation["f_total"] = tr("Amount");
    fTranslation["f_lowlevel"] = tr("Warning");
    fTranslation["f_price1"] = tr("Retail price");
    fTranslation["f_totalsale"] = tr("Retail amount");
    fTranslation["f_price2"] = tr("Whosale price");
    fTranslation["f_totalsale2"] = tr("Whosale amount");
    fTranslation["f_price1disc"] = tr("Retail discounted");
    fTranslation["f_price2disc"] = tr("Whosale discounted");
    fTranslation["f_totalretaildiscounted"] = tr("Total retail discounted");
    fTranslation["f_totalsale2discount"] = tr("Total whosale discounted");
    fTranslation["f_taxname"] = tr("Partner");
    fTranslation["f_validdate"] = tr("Valid date");
    fColumnsVisible["g.f_id as f_code"] = true;
    fColumnsVisible["ss.f_name as f_storage"] = true;
    fColumnsVisible["gg.f_name as f_group"] = true;
    fColumnsVisible["g.f_name as f_goods"] = true;
    fColumnsVisible["g.f_scancode"] = true;
    fColumnsVisible["round(sum(s.f_qty*s.f_type),3) as f_qty"] = true;
    fColumnsVisible["u.f_name as f_unit"] = true;
    fColumnsVisible["s.f_price"] = true;
    fColumnsVisible["sum(s.f_total*s.f_type) as f_total"] = true;
    fColumnsVisible["g.f_lowlevel"] = true;
    fColumnsVisible["gpr.f_price1"] = false;
    fColumnsVisible["gpr.f_price1disc"] = false;
    fColumnsVisible["sum(s.f_qty*s.f_type)*gpr.f_price1 as f_totalsale"] = false;
    fColumnsVisible["sum(s.f_qty*s.f_type)*if(gpr.f_price1disc>0,gpr.f_price1disc,gpr.f_price1) as f_totalretaildiscounted"]
        = false;
    fColumnsVisible["gpr.f_price2"] = false;
    fColumnsVisible["gpr.f_price2disc"] = false;
    fColumnsVisible["sum(s.f_qty*s.f_type)*gpr.f_price2 as f_totalsale2"] = false;
    fColumnsVisible["sum(s.f_qty*s.f_type)*if(gpr.f_price2disc>0,gpr.f_price2disc,gpr.f_price2) as f_totalsale2discount"] =
        false;
    fColumnsVisible["gca.f_name as f_class1"] = false;
    fColumnsVisible["gcb.f_name as f_class2"] = false;
    fColumnsVisible["gcc.f_name as f_class3"] = false;
    fColumnsVisible["gcd.f_name as f_class4"] = false;
    fColumnsVisible["p.f_taxname"] = false;
    fColumnsVisible["v.f_date as f_validdate"] = true;
    restoreColumnsVisibility();
}

void CR5MaterialsInStore::setColors()
{
    if (fColumnsVisible["g.f_lowlevel"] && fColumnsVisible["sum(s.f_qty*s.f_type) as f_qty"]) {
        int colQty = fModel->indexForColumnName("f_qty");
        int colLow = fModel->indexForColumnName("f_lowlevel");
        for (int i = 0; i < fModel->rowCount(); i++) {
            if (fModel->data(i, colLow, Qt::EditRole).toDouble() < 0.0001) {
                continue;
            }
            if (fModel->data(i, colQty, Qt::EditRole).toDouble() < fModel->data(i, colLow, Qt::EditRole).toDouble()) {
                fModel->setRowColor(i, QColor::fromRgb(255, 165, 165));
            }
        }
    }
}

void CR5MaterialsInStore::printBarcode()
{
    C5Database db;
    db[":f_id"] = fFilter->currency().isEmpty() ? __c5config.getValue(param_default_currency) : fFilter->currency();
    db.exec("select f_symbol from e_currency where f_id=:f_id");
    db.nextRow();
    QString s = db.getString("f_symbol");
    C5StoreBarcode *b = __mainWindow->createTab<C5StoreBarcode>();
    b->fCurrencyName = s;
    for (int i = 0; i < fModel->rowCount(); i++) {
        if (fFilter->unit().isEmpty()) {
            b->addRow(fModel->data(i, fModel->indexForColumnName("f_goods"), Qt::EditRole).toString(),
                      fModel->data(i, fModel->indexForColumnName("f_scancode"), Qt::EditRole).toString(),
                      fModel->data(i, fModel->indexForColumnName("f_qty"), Qt::EditRole).toInt(), fFilter->currency().toInt(), "");
        } else if (fFilter->unit() == "10") {
            db[":f_qty"] = fModel->data(i, fModel->indexForColumnName("f_qty"), Qt::EditRole);
            db[":f_base"] = fModel->data(i, fModel->indexForColumnName("f_code"), Qt::EditRole);
            db.exec("select g.f_name, g.f_id, g.f_scancode, c.f_qty*:f_qty as f_qty from c_goods_complectation c "
                    "left join c_goods g on g.f_id=c.f_goods "
                    "where c.f_base=:f_base ");
            while (db.nextRow()) {
                b->addRow(db.getString("f_name"),
                          db.getString("f_scancode"),
                          db.getDouble("f_qty"), fFilter->currency().toInt(),
                          fModel->data(i, fModel->indexForColumnName("f_group"), Qt::EditRole).toString());
            }
        }
    }
}

void CR5MaterialsInStore::buildQuery()
{
    if (fFilter->showDrafts()) {
        prepareDrafts();
    } else {
        prepareNoDrafts();
    }
    C5Grid::buildQuery();
}

bool CR5MaterialsInStore::on_tblView_doubleClicked(const QModelIndex &index)
{
    if (!fColumnsVisible["g.f_id as f_code"]) {
        C5Message::info(tr("Code column must be included in report"));
        return false;
    }
    CE5Goods *ep = new CE5Goods();
    C5Editor *e = C5Editor::createEditor(ep, fModel->data(index.row(), fModel->indexForColumnName("f_code"),
                                         Qt::EditRole).toInt());
    QList<QMap<QString, QVariant> > data;
    if(e->getResult(data)) {
    }
    return C5ReportWidget::on_tblView_doubleClicked(index);
}
