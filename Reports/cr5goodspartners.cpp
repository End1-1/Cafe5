#include "cr5goodspartners.h"
#include "c5cache.h"

CR5GoodsPartners::CR5GoodsPartners(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/goods.png";
    fLabel = tr("Partners");
    fSqlQuery = "select * from c_partners ";

    fTranslation["f_id"] = tr("Code");
    fTranslation["f_taxname"] = tr("Name");
    fTranslation["f_contact"] = tr("Contact");
    fTranslation["f_info"] = tr("Info");
    fTranslation["f_phone"] = tr("Phone");
    fTranslation["f_email"] = tr("Email");

    fTableView->setItemDelegateForColumn(1, new C5TextDelegate(fTableView));
    fTableView->setItemDelegateForColumn(2, new C5TextDelegate(fTableView));
    fTableView->setItemDelegateForColumn(3, new C5TextDelegate(fTableView));
    fTableView->setItemDelegateForColumn(4, new C5TextDelegate(fTableView));
    fTableView->setItemDelegateForColumn(5, new C5TextDelegate(fTableView));

    QList<int> colsForUpdate;
    colsForUpdate << 1 << 2 << 3 << 4 << 5;
    setTableForUpdate("c_partners", colsForUpdate);
}

QToolBar *CR5GoodsPartners::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbNew
            << ToolBarButtons::tbSave
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
            createStandartToolbar(btn);
    }
    return fToolBar;
}
