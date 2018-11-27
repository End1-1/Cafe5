#include "ce5partner.h"
#include "ui_ce5partner.h"

CE5Partner::CE5Partner(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5Partner)
{
    ui->setupUi(this);
}

CE5Partner::~CE5Partner()
{
    delete ui;
}

QString CE5Partner::title()
{
    return tr("Partner");
}

QString CE5Partner::table()
{
    return "c_partners";
}
