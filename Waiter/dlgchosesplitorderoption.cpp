#include "dlgchosesplitorderoption.h"
#include "ui_dlgchosesplitorderoption.h"

DlgChoseSplitOrderOption::DlgChoseSplitOrderOption() :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::DlgChoseSplitOrderOption)
{
    ui->setupUi(this);
}

DlgChoseSplitOrderOption::~DlgChoseSplitOrderOption()
{
    delete ui;
}

int DlgChoseSplitOrderOption::getOption()
{
    DlgChoseSplitOrderOption d;
    return d.exec();
}

void DlgChoseSplitOrderOption::on_btnCancel_clicked()
{
    done(JOIN_CANCEL);
}

void DlgChoseSplitOrderOption::on_btnSplit_clicked()
{
    done(JOIN_JOIN);
}

void DlgChoseSplitOrderOption::on_btnAppend_clicked()
{
    done(JOIN_APPEND);
}
