#include "cr5routedailyfilter.h"
#include "ui_cr5routedailyfilter.h"

CR5RouteDailyFilter::CR5RouteDailyFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5RouteDailyFilter)
{
    ui->setupUi(this);
}

CR5RouteDailyFilter::~CR5RouteDailyFilter()
{
    delete ui;
}

QString CR5RouteDailyFilter::condition()
{
    QString cond = QString("where ro.f_date between %1 and %2")
            .arg(ui->leStart->toMySQLDate(), ui->leEnd->toMySQLDate());
    return cond;
}
