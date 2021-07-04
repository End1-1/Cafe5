#include "dlglistofhall.h"
#include "ui_dlglistofhall.h"
#include "c5config.h"
#include "c5cafecommon.h"
#include "datadriver.h"
#include "c5dialog.h"

DlgListOfHall::DlgListOfHall(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgListOfHall)
{
    ui->setupUi(this);
    for (int id: dbhall->list()) {
        QListWidgetItem *item = new QListWidgetItem(ui->lst);
        item->setSizeHint(QSize(50, 50));
        item->setText(dbhall->name(id));
        item->setData(Qt::UserRole, id);
        ui->lst->addItem(item);
    }
    QListWidgetItem *item = new QListWidgetItem(ui->lst);
    item->setSizeHint(QSize(50, 50));
    item->setText("Cancel");
    item->setIcon(QIcon(":/cancel.png"));
    ui->lst->addItem(item);
}

DlgListOfHall::~DlgListOfHall()
{
    delete ui;
}

bool DlgListOfHall::getHall(int &hall)
{
    DlgListOfHall *d = new DlgListOfHall();
    bool result = d->exec() == QDialog::Accepted;
    if (result) {
        hall = d->fHall;
    }
    delete d;
    return result;
}

void DlgListOfHall::on_lst_clicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    if (index.row() == ui->lst->count() - 1) {
        reject();
        return;
    }
    fHall = index.data(Qt::UserRole).toInt();
    accept();
}
