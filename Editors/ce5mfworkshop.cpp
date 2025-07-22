#include "ce5mfworkshop.h"
#include "ui_ce5mfworkshop.h"

CE5MFWorkshop::CE5MFWorkshop(QWidget *parent) :
    CE5Editor(parent),
    ui(new Ui::CE5MFWorkshop)
{
    ui->setupUi(this);
}

CE5MFWorkshop::~CE5MFWorkshop()
{
    delete ui;
}

QString CE5MFWorkshop::title()
{
    return tr("Workshop");
}

QString CE5MFWorkshop::table()
{
    return "mf_stage";
}
