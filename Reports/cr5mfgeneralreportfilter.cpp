#include "cr5mfgeneralreportfilter.h"
#include "ui_cr5mfgeneralreportfilter.h"
#include "c5cache.h"
#include "format_date.h"

CR5MFGeneralReportFilter::CR5MFGeneralReportFilter(QWidget *parent) :
    C5FilterWidget(parent),
    ui(new Ui::CR5MFGeneralReportFilter)
{
    ui->setupUi(this);
    ui->leProduct->setSelector(ui->leProducName, cache_mf_products);
    ui->leProcess->setSelector(ui->leProcessName, cache_mf_actions);
    ui->lePosition->setSelector(ui->lePositionName, cache_users_groups);
    ui->leWorker->setSelector(ui->leWorkerName, cache_users);
    ui->leTeamlead->setSelector(ui->leTeamleadName, cache_users);
    ui->leWorkerState->setSelector(ui->leWorkerStateName, cache_users_states);
}

CR5MFGeneralReportFilter::~CR5MFGeneralReportFilter()
{
    delete ui;
}

QString CR5MFGeneralReportFilter::condition()
{
    QString cond = QString(" p.f_date between '%1' and '%2' ")
                   .arg(ui->leStart->date().toString(FORMAT_DATE_TO_STR_MYSQL), ui->leEnd->date().toString(FORMAT_DATE_TO_STR_MYSQL));
    in(cond, "p.f_product", ui->leProduct);
    in(cond, "p.f_process", ui->leProcess);
    in(cond, "p.f_worker", ui->leWorker);
    in(cond, "w.f_group", ui->lePosition);
    in(cond, "w.f_teamlead", ui->leTeamlead);
    in(cond, "w.f_state", ui->leWorkerState);
    return cond;
}

QString CR5MFGeneralReportFilter::conditionText()
{
    QString text;
    text += QString("%1 %2 - %3").arg(tr("Date range"), ui->leStart->text(), ui->leEnd->text());

    if(!ui->leTeamlead->isEmpty()) {
        text += QString("%1: %2").arg(tr("Teamlead"), ui->leTeamleadName->text());
    }

    if(!ui->leWorkerState->isEmpty()) {
        text += " ";
    }

    return text;
}

QString CR5MFGeneralReportFilter::d1ms() const
{
    return ui->leStart->date().toString(FORMAT_DATE_TO_STR_MYSQL);
}

QString CR5MFGeneralReportFilter::d2ms() const
{
    return ui->leEnd->date().toString(FORMAT_DATE_TO_STR_MYSQL);
}

QString CR5MFGeneralReportFilter::d1s() const
{
    return ui->leStart->text();
}

QString CR5MFGeneralReportFilter::d2s() const
{
    return ui->leEnd->text();
}
