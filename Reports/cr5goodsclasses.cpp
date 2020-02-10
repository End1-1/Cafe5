#include "cr5goodsclasses.h"
#include "ce5goodsclass.h"

CR5GoodsClasses::CR5GoodsClasses(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/goods.png";
    fLabel = tr("Goods classes");

    fSqlQuery = "select f_id, f_name from c_goods_classes ";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_name"] = tr("Name");

    fEditor = new CE5GoodsClass(dbParams);
}

QToolBar *CR5GoodsClasses::toolBar()
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
