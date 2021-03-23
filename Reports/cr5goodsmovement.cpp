#include "cr5goodsmovement.h"
#include "cr5goodsmovementfilter.h"
#include "c5mainwindow.h"
#include "c5storedoc.h"
#include "c5changedocinputprice.h"
#include "c5tablemodel.h"

CR5GoodsMovement::CR5GoodsMovement(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/goods.png";
    fLabel = tr("Movements in the store");
    fSimpleQuery = false;

    fMainTable = "a_store s";
    fLeftJoinTables << "left join a_type_sign st on st.f_id=s.f_type [st]"
                    << "left join a_type dt on dt.f_id=a.f_type [dt]"
                    << "left join c_storages ss on ss.f_id=s.f_store [ss]"
                    << "left join c_goods g on g.f_id=s.f_goods [g]"
                    << "left join c_groups gg on gg.f_id=g.f_group [gg]"
                    << "left join c_units u on u.f_id=g.f_unit [u]"
                    << "left join a_header a on a.f_id=s.f_document [a]"
                    << "left join c_goods_classes gca on gca.f_id=g.f_group1 [gca]"
                    << "left join c_goods_classes gcb on gca.f_id=g.f_group2 [gcb]"
                    << "left join c_goods_classes gcc on gca.f_id=g.f_group3 [gcc]"
                    << "left join c_goods_classes gcd on gca.f_id=g.f_group4 [gcd]"
                    << "left join a_store_draft asd on asd.f_id=s.f_draft [asd]"
                       ;

    fColumnsFields << "s.f_document"
                   << "s.f_id as f_storerec"
                   << "dt.f_name as f_docname"
                   << "a.f_date"
                   << "a.f_userid"
                   << "ss.f_name as f_store"
                   << "st.f_name as f_type"
                   << "gg.f_name as f_group"
                   << "g.f_name as f_goods"
                   << "g.f_scancode"
                   << "gca.f_name as f_class1"
                   << "gcb.f_name as f_class2"
                   << "gcc.f_name as f_class3"
                   << "gcd.f_name as f_class4"
                   << "asd.f_comment as f_storecomment"
                   << "a.f_comment as f_doccomment"
                   << "sum(s.f_qty) as f_qty"
                   << "u.f_name as f_unit"
                   << "s.f_price"
                   << "sum(s.f_total) as f_total"
                   << "g.f_saleprice"
                   << "sum(g.f_saleprice*s.f_qty) as f_salepricetotal"
                   << "g.f_saleprice2"
                   << "sum(g.f_saleprice2*s.f_qty) as f_salepricetotal2"
                      ;

    fColumnsGroup << "s.f_document"
                  << "s.f_id as f_storerec"
                  << "dt.f_name as f_docname"
                  << "a.f_userid"
                  << "a.f_date"
                  << "a.f_userid"
                  << "st.f_name as f_type"
                  << "ss.f_name as f_store"
                  << "gg.f_name as f_group"
                  << "g.f_name as f_goods"
                  << "g.f_scancode"
                  << "gca.f_name as f_class1"
                  << "gcb.f_name as f_class2"
                  << "gcc.f_name as f_class3"
                  << "gcd.f_name as f_class4"
                  << "asd.f_comment as f_storecomment"
                  << "a.f_comment as f_doccomment"
                  << "g.f_saleprice"
                  << "g.f_saleprice2"
                  << "s.f_price"
                  << "u.f_name as f_unit"
                  << "g.f_name as f_goods"
                      ;

    fColumnsSum << "f_qty"
                << "f_total"
                << "f_salepricetotal"
                << "f_salepricetotal2"
                      ;

    fTranslation["f_userid"] = tr("Document");
    fTranslation["f_storerec"] = tr("Store record");
    fTranslation["f_docname"] = tr("Type");
    fTranslation["f_date"] = tr("Date");
    fTranslation["f_document"]  = tr("Op.num.");
    fTranslation["f_type"] = tr("In/Out");
    fTranslation["f_store"] = tr("Storage");
    fTranslation["f_group"] = tr("Group");
    fTranslation["f_goods"] = tr("Goods");
    fTranslation["f_scancode"] = tr("Scancode");
    fTranslation["f_class1"] = tr("Class 1");
    fTranslation["f_class2"] = tr("Class 2");
    fTranslation["f_class3"] = tr("Class 3");
    fTranslation["f_class4"] = tr("Class 4");
    fTranslation["f_storecomment"] = tr("Record comment");
    fTranslation["f_doccomment"] = tr("Document comment");
    fTranslation["f_saleprice"] = tr("Retail price");
    fTranslation["f_saleprice2"] = tr("Whosale price");
    fTranslation["f_qty"] = tr("Qty");
    fTranslation["f_unit"] = tr("Unit");
    fTranslation["f_price"] = tr("Price");
    fTranslation["f_total"] = tr("Amount");
    fTranslation["f_salepricetotal"] = tr("Total retail");
    fTranslation["f_salepricetotal2"] = tr("Total whosale");

    fColumnsVisible["a.f_date"] = true;
    fColumnsVisible["s.f_id as f_storerec"] = false;
    fColumnsVisible["dt.f_name as f_docname"] = true;
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
    fColumnsVisible["gca.f_name as f_class1"] = false;
    fColumnsVisible["gcb.f_name as f_class2"] = false;
    fColumnsVisible["gcc.f_name as f_class3"] = false;
    fColumnsVisible["gcd.f_name as f_class4"] = false;
    fColumnsVisible["asd.f_comment as f_storecomment"] = false;
    fColumnsVisible["a.f_comment as f_doccomment"] = false;
    fColumnsVisible["sum(g.f_saleprice*s.f_qty) as f_salepricetotal"] = true;
    fColumnsVisible["sum(g.f_saleprice2*s.f_qty) as f_salepricetotal2"] = false;

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
        fToolBar->addAction(QIcon(":/pricing.png"), tr("Change price"), this, SLOT(changePrice()));
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
    QString e;
    if (!sd->openDoc(values.at(fModel->indexForColumnName("f_document")).toString(), e)) {
        __mainWindow->removeTab(sd);
        C5Message::error(e);
    }
    return true;
}

void CR5GoodsMovement::changePrice()
{
    if (!fColumnsVisible["s.f_id as f_storerec"]) {
        C5Message::info(tr("The store record column must be included in the report"));
        return;
    }
    QModelIndexList ml = this->fTableView->selectionModel()->selectedIndexes();
    QSet<int> rows;
    foreach (QModelIndex m, ml) {
        rows << m.row();
    }
    if (rows.count() > 1) {
        C5Message::error(tr("Select only one row"));
        return;
    }
    if (rows.count() == 0) {
        C5Message::error(tr("Nothing was selected"));
        return;
    }
    C5ChangeDocInputPrice::changePrice(fDBParams, fModel->data(rows.toList().at(0), fModel->indexForColumnName("f_storerec"), Qt::EditRole).toString());
}

void CR5GoodsMovement::restoreColumnsWidths()
{
    C5ReportWidget::restoreColumnsWidths();
    int idx = fModel->indexForColumnName("f_document");
    if (idx != -1) {
        fTableView->setColumnWidth(idx, 0);
    }
}
