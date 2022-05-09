#include "dlglistofshifts.h"
#include "ui_dlglistofshifts.h"
#include "datadriver.h"

DlgListOfShifts::DlgListOfShifts(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgListOfShifts)
{
    ui->setupUi(this);
    QListWidgetItem *item = new QListWidgetItem(ui->lst);
    item->setSizeHint(QSize(50, 50));
    item->setText(tr("All"));
    item->setData(Qt::UserRole, 0);
    ui->lst->addItem(item);
    for (int id: dbshift->list()) {
        item = new QListWidgetItem(ui->lst);
        item->setSizeHint(QSize(50, 50));
        item->setText(dbshift->name(id));
        item->setData(Qt::UserRole, id);
        ui->lst->addItem(item);
    }
    item = new QListWidgetItem(ui->lst);
    item->setSizeHint(QSize(50, 50));
    item->setText("Cancel");
    item->setIcon(QIcon(":/cancel.png"));
    ui->lst->addItem(item);
}

DlgListOfShifts::~DlgListOfShifts()
{
    delete ui;
}

bool DlgListOfShifts::getShift(int &id, QString &name)
{
    bool result = false;
    DlgListOfShifts *d = new DlgListOfShifts();
    if (d->exec() == QDialog::Accepted) {
        id = d->fShiftId;
        name = d->fShiftName;
        result = true;
    }
    delete d;
    return result;
}

void DlgListOfShifts::on_lst_clicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    if (index.row() == ui->lst->count() - 1) {
        reject();
        return;
    }
    fShiftId = index.data(Qt::UserRole).toInt();
    fShiftName = index.data(Qt::DisplayRole).toString();
    accept();
}
