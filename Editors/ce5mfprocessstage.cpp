#include "ce5mfprocessstage.h"
#include "ui_ce5mfprocessstage.h"

CE5MFProcessStage::CE5MFProcessStage(QWidget *parent) :
    CE5Editor(parent),
    ui(new Ui::CE5MFProcessStage)
{
    ui->setupUi(this);
}

CE5MFProcessStage::~CE5MFProcessStage()
{
    delete ui;
}

QString CE5MFProcessStage::title()
{
    return tr("Actions stage");
}

QString CE5MFProcessStage::table()
{
    return "mf_actions_state";
}
