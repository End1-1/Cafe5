#include "touchselecttaxreport.h"
#include "ui_touchselecttaxreport.h"
#include "printtaxn.h"
#include "c5config.h"

TouchSelectTaxReport::TouchSelectTaxReport() :
    C5Dialog(),
    ui(new Ui::TouchSelectTaxReport)
{
    ui->setupUi(this);
}

TouchSelectTaxReport::~TouchSelectTaxReport()
{
    delete ui;
}

bool TouchSelectTaxReport::getReportType(int &r)
{
    TouchSelectTaxReport d;
    if (d.exec() != QDialog::Rejected) {
        r = d.result();
        return true;
    }
    return false;
}

void TouchSelectTaxReport::on_btnReject_clicked()
{
    reject();
}

void TouchSelectTaxReport::on_btnPrintTaxX_clicked()
{
    done(report_x);
}

void TouchSelectTaxReport::on_btnPrintTaxZ_clicked()
{
    done(report_z);
}
