#include "dlglistofhall.h"
#include "ui_dlglistofhall.h"
#include "c5config.h"
#include "c5cafecommon.h"
#include "c5dialog.h"

DlgListOfHall::DlgListOfHall(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgListOfHall)
{
    ui->setupUi(this);
    for (int i = 0; i < C5CafeCommon::fHalls.count(); i++) {
        QJsonObject o = C5CafeCommon::fHalls[i].toObject();
        QListWidgetItem *item = new QListWidgetItem(ui->lst);
        item->setSizeHint(QSize(50, 50));
        item->setText(o["f_name"].toString());
        item->setData(Qt::UserRole, o["f_id"].toString());
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

bool DlgListOfHall::getHall(QString &hall)
{
    DlgListOfHall *d = new DlgListOfHall(__mainWindow);
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
    fHall = index.data(Qt::UserRole).toString();
    accept();
}
