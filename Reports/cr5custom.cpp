#include "cr5custom.h"
#include "c5customfilter.h"
#include "c5user.h"
#include "c5database.h"

QStringList rnames;
QStringList rsqls;

CR5Custom::CR5Custom(QWidget *parent) :
    C5ReportWidget(parent)
{
    fLabel = tr("Custom reports");
    fIcon = ":/constructor.png";
    rnames.clear();
    rsqls.clear();
    C5Database db;
    db[":f_access"] = mUser->group();
    db.exec("select f_name, f_sql from s_custom_reports where f_access=-1 or f_access=:f_access");

    while(db.nextRow()) {
        rnames.append(db.getString("f_name"));
        rsqls.append(db.getString("f_sql"));
    }
}

QToolBar* CR5Custom::toolBar()
{
    if(!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbNew
            << ToolBarButtons::tbFilter
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        fToolBar = createStandartToolbar(btn);
    }

    return fToolBar;
}

void CR5Custom::setSearchParameters()
{
    C5CustomFilter f;
    f.setQueries(rnames, rsqls);

    if(f.exec() == QDialog::Accepted) {
        fSqlQuery = f.sql();
        refreshData();
    }
}
