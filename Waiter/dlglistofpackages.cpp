#include "dlglistofpackages.h"
#include "ui_dlglistofpackages.h"
#include "c5menu.h"

DlgListOfPackages::DlgListOfPackages(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgListOfPackages)
{
    ui->setupUi(this);
    ui->tbl->setRowCount(C5Menu::fPackages.count() + 1);
    ui->tbl->setColumnWidths(3, 0, 300, 100);
    int row = 0;
    for (QMap<QString, QJsonObject>::const_iterator it = C5Menu::fPackages.begin(); it != C5Menu::fPackages.end(); it++) {
        QJsonObject o = it.value();
        ui->tbl->setString(row, 0, o["f_id"].toString());
        ui->tbl->setString(row, 1, o["f_name"].toString());
        ui->tbl->setString(row, 2, o["f_price"].toString());
        row++;
    }
    ui->tbl->setSpan(ui->tbl->rowCount() - 1, 0, 1, 3);
    QTableWidgetItem *item = new QTableWidgetItem();
    item->setSizeHint(QSize(50, 50));
    item->setText("Cancel");
    item->setIcon(QIcon(":/cancel.png"));
    ui->tbl->setItem(ui->tbl->rowCount() - 1, 0, item);
}

DlgListOfPackages::~DlgListOfPackages()
{
    delete ui;
}

bool DlgListOfPackages::package(int &id, QString &name)
{
    id = 0;
    DlgListOfPackages *d = new DlgListOfPackages();
    if (d->exec() == QDialog::Accepted) {
        id = d->fId;
        name = d->fName;
    }
    delete d;
    return id > 0;
}

void DlgListOfPackages::on_tbl_cellClicked(int row, int column)
{
    Q_UNUSED(column);
    if (ui->tbl->getInteger(row, 0) > 0) {
        fId = ui->tbl->getInteger(row, 0);
        fName = ui->tbl->getString(row, 1);
        accept();
    }
    if (row == ui->tbl->rowCount() - 1) {
        reject();
    }
}
