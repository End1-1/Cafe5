#include "c5reportsupload.h"
#include "ui_c5reportsupload.h"

C5ReportsUpload::C5ReportsUpload(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::C5ReportsUpload)
{
    ui->setupUi(this);
    ui->widget->getWatchList(ui->widget);
}

C5ReportsUpload::~C5ReportsUpload()
{
    if (ui->widget->hasChanges()) {
        ui->widget->saveChanges();
    }
    delete ui;
}
