#include "dlgconfig.h"
#include "ui_dlgconfig.h"
#include <QFileDialog>
#include <QSettings>

DlgConfig::DlgConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgConfig)
{
    ui->setupUi(this);
    QSettings s(_ORGANIZATION_, _APPLICATION_);
    ui->leServerName->setText(s.value("ServerAddress").toString());
    ui->leDestinationFolder->setText(s.value("DestinationFolder").toString());
}

DlgConfig::~DlgConfig()
{
    delete ui;
}

void DlgConfig::on_btnExplore_clicked()
{
    QString dst = QFileDialog::getExistingDirectory(this, "Destination folder");
    if (!dst.isEmpty()) {
        ui->leDestinationFolder->setText(dst);
    }
}


void DlgConfig::on_btnOK_clicked()
{
    QSettings s(_ORGANIZATION_, _APPLICATION_);
    s.setValue("ServerAddress", ui->leServerName->text());
    s.setValue("DestinationFolder", ui->leDestinationFolder->text());
}

