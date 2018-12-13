#include "ce5client.h"
#include "ui_ce5client.h"

CE5Client::CE5Client(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5Client)
{
    ui->setupUi(this);
}

CE5Client::~CE5Client()
{
    delete ui;
}
