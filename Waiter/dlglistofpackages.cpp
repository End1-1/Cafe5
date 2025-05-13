#include "dlglistofpackages.h"
#include "ui_dlglistofpackages.h"
#include "datadriver.h"

DlgListOfPackages::DlgListOfPackages(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgListOfPackages)
{
    ui->setupUi(this);
    ui->tbl->setRowCount(dbmenupackage->list().count() + 1);
    ui->tbl->configColumns({3, 0, 300, 100});
    int row = 0;
    for (int id : dbmenupackage->list()) {
        DbMenuPackages p(id);
        if (!p.isEnabled()) {
            continue;
        }
        ui->tbl->setInteger(row, 0, id);
        ui->tbl->setString(row, 1, p.name());
        ui->tbl->setDouble(row, 2, p.price());
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
