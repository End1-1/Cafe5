#include "cr5goodsmovement.h"
#include "cr5goodsmovementfilter.h"
#include "c5mainwindow.h"
#include "c5storedoc.h"
#include "c5tablemodel.h"

CR5GoodsMovement::CR5GoodsMovement(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/goods.png";
    fLabel = tr("Movements in the store");
    fSimpleQuery = false;

    fMainTable = "a_store s";
    fLeftJoinTables << "inner join a_type_sign st on st.f_id=s.f_type [st]"
                    << "inner join c_storages ss on ss.f_id=s.f_store [ss]"
                    << "inner join c_goods g on g.f_id=s.f_goods [g]"
                    << "inner join c_groups gg on gg.f_id=g.f_group [gg]"
                    << "inner join c_units u on u.f_id=g.f_unit [u]"
                    << "inner join a_header a on a.f_id=s.f_document [a]"
                    << "inner join "
                       ;

    fColumnsFields << "s.f_document"
                   << "a.f_userid"
                   << "a.f_date"
                   << "s.f_userid"
                   << "ss.f_name as f_store"
                   << "st.f_name as f_type"
                   << "gg.f_name as f_group"
                   << "g.f_name as f_goods"
                   << "g.f_scancode"
                   << "g.f_saleprice"
                   << "g.f_saleprice2"
                   << "sum(s.f_qty) as f_qty"
                   << "u.f_name as f_unit"
                   << "s.f_price"
                   << "sum(s.f_total) as f_total"
                      ;

    fColumnsGroup << "s.f_document"
                  << "a.f_userid"
                  << "a.f_date"
                  << "s.f_userid"
                  << "st.f_name as f_type"
                  << "ss.f_name as f_store"
                  << "gg.f_name as f_group"
                  << "g.f_name as f_goods"
                  << "g.f_scancode"
                  << "g.f_saleprice"
                  << "g.f_saleprice2"
                  << "s.f_price"
                  << "u.f_name as f_unit"
                  << "g.f_name as f_goods"
                      ;

    fColumnsSum << "f_qty"
                << "f_total"
                      ;

    fTranslation["f_userid"] = tr("Document");
    fTranslation["f_date"] = tr("Date");
    fTranslation["f_document"]  = tr("Op.num.");
    fTranslation["f_type"] = tr("In/Out");
    fTranslation["f_store"] = tr("Storage");
    fTranslation["f_group"] = tr("Group");
    fTranslation["f_goods"] = tr("Goods");
    fTranslation["f_scancode"] = tr("Scancode");
    fTranslation["f_saleprice"] = tr("Retail price");
    fTranslation["f_saleprice2"] = tr("Whosale price");
    fTranslation["f_qty"] = tr("Qty");
    fTranslation["f_unit"] = tr("Unit");
    fTranslation["f_price"] = tr("Price");
    fTranslation["f_total"] = tr("Amount");;

    fColumnsVisible["a.f_date"] = true;
    fColumnsVisible["a.f_userid"] = true;
    fColumnsVisible["s.f_document"] = false;
    fColumnsVisible["st.f_name as f_type"] = true;
    fColumnsVisible["ss.f_name as f_store"] = true;
    fColumnsVisible["gg.f_name as f_group"] = true;
    fColumnsVisible["g.f_name as f_goods"] = true;
    fColumnsVisible["g.f_scancode"] = true;
    fColumnsVisible["g.f_saleprice"] = true;
    fColumnsVisible["g.f_saleprice2"] = false;
    fColumnsVisible["sum(s.f_qty) as f_qty"] = true;
    fColumnsVisible["u.f_name as f_unit"] = true;
    fColumnsVisible["s.f_price"] = true;
    fColumnsVisible["sum(s.f_total) as f_total"] = true;

    restoreColumnsVisibility();
    fFilterWidget = new CR5GoodsMovementFilter(fDBParams);
}

QToolBar *CR5GoodsMovement::toolBar()
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

bool CR5GoodsMovement::tblDoubleClicked(int row, int column, const QList<QVariant> &values)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    if (!fColumnsVisible["s.f_document"]) {
        C5Message::info(tr("Document id column must be included in the report"));
        return true;
    }
    C5StoreDoc *sd = __mainWindow->createTab<C5StoreDoc>(fDBParams);
    if (!sd->openDoc(values.at(fModel->indexForColumnName("f_document")).toString())) {
        __mainWindow->removeTab(sd);
    }
    return true;
}

void CR5GoodsMovement::restoreColumnsWidths()
{
    C5ReportWidget::restoreColumnsWidths();
    int idx = fModel->indexForColumnName("f_document");
    if (idx != -1) {
        fTableView->setColumnWidth(idx, 0);
    }
}
