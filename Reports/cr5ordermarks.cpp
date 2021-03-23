#include "cr5ordermarks.h"
#include "ce5ordermark.h"

CR5OrderMarks::CR5OrderMarks(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fLabel = tr("Orders marks");
    fIcon = ":/flag.png";
    fSimpleQuery = true;
    fSqlQuery = "select f_id, f_active, f_name from b_marks";
    fTranslation["f_id"] = tr("ID");
    fTranslation["f_active"] = tr("Active");
    fTranslation["f_name"] = tr("Name");
    fEditor = new CE5OrderMark(dbParams);
}

QToolBar *CR5OrderMarks::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbNew
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        fToolBar = createStandartToolbar(btn);
    }
    return fToolBar;
}
