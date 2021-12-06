#include "ce5mfprocess.h"
#include "ui_ce5mfprocess.h"

CE5MFProcess::CE5MFProcess(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5MFProcess)
{
    ui->setupUi(this);
}

CE5MFProcess::~CE5MFProcess()
{
    delete ui;
}

QString CE5MFProcess::title()
{
    return tr("Process");
}

QString CE5MFProcess::table()
{
    return "mf_actions";
}
