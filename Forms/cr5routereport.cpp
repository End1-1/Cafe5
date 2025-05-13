#include "cr5routereport.h"
#include "ui_cr5routereport.h"
#include "c5database.h"
#include "dlgdriverroutedate.h"

CR5RouteReport::CR5RouteReport(const QStringList &dbParams, QWidget *parent) :
    C5Document(dbParams, parent),
    ui(new Ui::CR5RouteReport)
{
    ui->setupUi(this);
    fLabel = tr("Edit route");
    fIcon = ":/route.png";
}

CR5RouteReport::~CR5RouteReport()
{
    delete ui;
}

QToolBar *CR5RouteReport::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbNew;
        fToolBar = createStandartToolbar(btn);
    }
    return fToolBar;
}

void CR5RouteReport::newRow()
{
    QDate d;
    int driver;
    if (!DlgDriverRouteDate::getDate(fDBParams, d, driver, this)) {
        return;
    }
    int day = d.dayOfWeek();
    C5Database db(fDBParams);
    db[":f_" + QString::number(day)];
    db.exec(QString("select f_partner from o_route where f_%1=1").arg(day));
    QString sql;
    while (db.nextRow()) {
        if (!sql.isEmpty()) {
            sql += ",";
        }
        sql += QString("(%1,'%2',%3,%4)").arg(db.getString("f_partner"), d.toString(FORMAT_DATE_TO_STR_MYSQL), "0",
                                              QString::number(driver));
    }
    db[":f_date"] = d;
    db[":f_driver"] = driver;
    db.exec("delete from o_route_exec where f_date=:f_date and f_action=0 and f_driver=:f_driver");
    sql = QString("insert into o_route_exec (f_partner, f_date, f_action, f_driver) values %1").arg(sql);
    db.exec(sql);
}
