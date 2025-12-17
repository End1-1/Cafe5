#include "dlgprecheckoptions.h"
#include "ui_dlgprecheckoptions.h"

DlgPrecheckOptions::DlgPrecheckOptions() :
    C5Dialog(),
    ui(new Ui::DlgPrecheckOptions)
{
    ui->setupUi(this);
}

DlgPrecheckOptions::~DlgPrecheckOptions()
{
    delete ui;
}

int DlgPrecheckOptions::precheck()
{
    DlgPrecheckOptions d;
    return d.exec();
}

void DlgPrecheckOptions::on_btnCancel_clicked()
{
    done(PRECHECK_NONE);
}

void DlgPrecheckOptions::on_btnRepeatPrecheck_clicked()
{
    done(PRECHECK_REPEAT);
}

void DlgPrecheckOptions::on_btnCancelPrecheck_clicked()
{
    done(PRECHECK_CANCEL);
}
