#include "cr5goodsstorages.h"
#include "ce5storage.h"

CR5GoodsStorages::CR5GoodsStorages(QWidget *parent) :
    C5ReportWidget( parent)
{
    fIcon = ":/goods.png";
    fLabel = tr("Storages");

    fSqlQuery = "select f_id, f_name, f_inputcounter, f_outcounter, f_movecounter, f_complectcounter, f_partner from c_storages ";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_name"] = tr("Name");
    fTranslation["f_inputcounter"] = tr("Input counter");
    fTranslation["f_outcounter"] = tr("Output counter");
    fTranslation["f_movecounter"] = tr("Movement counter");
    fTranslation["f_complectcounter"] = tr("Complectation counter");
    fTranslation["f_partner"] = tr("Partner");

    fEditor = new CE5Storage();
}

QToolBar *CR5GoodsStorages::toolBar()
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
