#include "dlgreportparams.h"
#include "ui_dlgreportparams.h"
#include "wplast30sessions.h"

DlgReportParams::DlgReportParams(C5User *user, const QString &params)
    : C5Dialog(user),
      ui(new Ui::DlgReportParams)
{
    ui->setupUi(this);

    if(params == "last-30-sessions") {
        mWPWidget = new WPLast30Sessions(user, this);
        ui->vlcontainer->insertWidget(0, mWPWidget);
    }
}

DlgReportParams::~DlgReportParams()
{
    delete ui;
}

QJsonObject DlgReportParams::getParams()
{
    return mWPWidget ? mWPWidget->mParams : QJsonObject();
}

void DlgReportParams::showEvent(QShowEvent *e)
{
    C5Dialog::showEvent(e);

    if(mWPWidget) {
        mWPWidget->setup();
    }
}

void DlgReportParams::on_btnBack_clicked()
{
    reject();
}

void DlgReportParams::on_btnPrint_clicked()
{
    if(mWPWidget->mParams.isEmpty()) {
        return;
    }

    accept();
}
