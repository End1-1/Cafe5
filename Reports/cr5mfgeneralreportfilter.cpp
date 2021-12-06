#include "cr5mfgeneralreportfilter.h"
#include "ui_cr5mfgeneralreportfilter.h"
#include "c5cache.h"

CR5MFGeneralReportFilter::CR5MFGeneralReportFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5MFGeneralReportFilter)
{
    ui->setupUi(this);
    ui->leProduct->setSelector(dbParams, ui->leProducName, cache_mf_products);
    ui->leProcess->setSelector(dbParams, ui->leProcess, cache_mf_actions);
    ui->lePosition->setSelector(dbParams, ui->lePositionName, cache_users_groups);
    ui->leWorker->setSelector(dbParams, ui->leWorkerName, cache_users);
}

CR5MFGeneralReportFilter::~CR5MFGeneralReportFilter()
{
    delete ui;
}

QString CR5MFGeneralReportFilter::condition()
{
    QString cond = QString(" p.f_date between '%1' and '%2' ")
            .arg(ui->leStart->date().toString(FORMAT_DATE_TO_STR_MYSQL))
            .arg(ui->leEnd->date().toString(FORMAT_DATE_TO_STR_MYSQL));
    in(cond, "p.f_product", ui->leProduct);
    in(cond, "p.f_process", ui->leProcess);
    in(cond, "p.f_worker", ui->leWorker);
    in(cond, "w.f_group", ui->lePosition);
    return cond;
}
