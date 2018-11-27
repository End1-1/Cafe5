#include "cr5goods.h"
#include "ce5goods.h"

CR5Goods::CR5Goods(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/goods.png";
    fLabel = tr("Goods");

    fSqlQuery = "select gg.f_id, g.f_name as f_groupname, u.f_name as f_unitname, gg.f_name \
                from c_goods gg \
                left join c_groups g on g.f_id=gg.f_group \
                left join c_units u on u.f_id=gg.f_unit ";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_groupname"] = tr("Group");
    fTranslation["f_unitname"] = tr("Unit");
    fTranslation["f_name"] = tr("Name");;

    fEditor = new CE5Goods(dbParams);
}

QToolBar *CR5Goods::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbNew
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
            createStandartToolbar(btn);
    }
    return fToolBar;
}
