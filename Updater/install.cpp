#include "install.h"
#include "ui_install.h"

Install::Install(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::Install)
{
    ui->setupUi(this);
}

Install::~Install()
{
    delete ui;
}
