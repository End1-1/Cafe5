#include "dlglicenses.h"
#include "ui_dlglicenses.h"
#include <QMessageBox>

DlgLicenses::DlgLicenses(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgLicenses)
{
    ui->setupUi(this);
    ui->tbl->setColumnWidth(0, 20);
    ui->tbl->setColumnWidth(1, 50);
    ui->tbl->setColumnWidth(2, 80);
    ui->tbl->setColumnWidth(3, 80);
    ui->tbl->setColumnWidth(4, 80);
}

DlgLicenses::~DlgLicenses()
{
    delete ui;
}

void DlgLicenses::addLicense(int id, const QString &d1, const QString &d2, int ltype)
{
    ui->tbl->setRowCount(ui->tbl->rowCount() + 1);
    int r = ui->tbl->rowCount() - 1;
    for (int i = 0; i < ui->tbl->columnCount(); i++) {
        ui->tbl->setItem(r, i, new QTableWidgetItem());
    }
    ui->tbl->item(r, 0)->setCheckState(Qt::Unchecked);
    ui->tbl->item(r, 1)->setData(Qt::EditRole, id);
    ui->tbl->item(r, 2)->setData(Qt::EditRole, d1);
    ui->tbl->item(r, 3)->setData(Qt::EditRole, d2);
    QString typeStr = tr("Server");
    if (ltype == 1) {
        typeStr = tr("Demo");
    }
    ui->tbl->item(r, 4)->setData(Qt::EditRole, typeStr);
}

void DlgLicenses::on_tbl_cellClicked(int row, int column)
{
    if (column != 0) {
        return;
    }
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        if (row == i) {
            continue;
        }
        ui->tbl->item(i, 0)->setCheckState(Qt::Unchecked);
    }
}

void DlgLicenses::on_btnClose_clicked()
{
    reject();
}

void DlgLicenses::on_btnActivate_clicked()
{
    int lid = 0;
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        if (ui->tbl->item(i, 0)->checkState() == Qt::Checked) {
            lid = ui->tbl->item(i, 1)->data(Qt::EditRole).toInt();
            break;
        }
    }
    if (lid == 0) {
        QMessageBox::critical(this, tr("Error"), tr("No license selected"));
        return;
    }
    done(lid);
}
