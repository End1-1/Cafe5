#include "ce5mfprocess.h"
#include "ui_ce5mfprocess.h"
#include "c5cache.h"

CE5MFProcess::CE5MFProcess(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5MFProcess)
{
    ui->setupUi(this);
    ui->leStage->setSelector(dbParams, ui->leStageName, cache_mf_action_stage);
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
