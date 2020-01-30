#include "dbconnection.h"
#include "ui_dbconnection.h"

DbConnection::DbConnection(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DbConnection)
{
    ui->setupUi(this);
    ui->widget->getWatchList(ui->widget);
}

DbConnection::~DbConnection()
{
    if (ui->widget->hasChanges()) {
        ui->widget->saveChanges();
    }
    delete ui;
}
