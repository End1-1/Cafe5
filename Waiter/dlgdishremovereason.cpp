#include "dlgdishremovereason.h"
#include "ui_dlgdishremovereason.h"
#include "c5cafecommon.h"
#include "c5config.h"
#include "c5utils.h"
#include "c5message.h"

DlgDishRemoveReason::DlgDishRemoveReason(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgDishRemoveReason)
{
    ui->setupUi(this);
    foreach (QString s, C5CafeCommon::fDishRemoveReason) {
        QListWidgetItem *item = new QListWidgetItem(ui->lst);
        item->setSizeHint(QSize(50, 50));
        item->setText(s);
        ui->lst->addItem(item);
    }
    QListWidgetItem *item = new QListWidgetItem(ui->lst);
    item->setSizeHint(QSize(50, 50));
    item->setText(tr("Other"));
    ui->lst->addItem(item);
    item = new QListWidgetItem(ui->lst);
    item->setSizeHint(QSize(50, 50));
    item->setText("Cancel");
    item->setIcon(QIcon(":/cancel.png"));
    ui->lst->addItem(item);
}

DlgDishRemoveReason::~DlgDishRemoveReason()
{
    delete ui;
}

bool DlgDishRemoveReason::getReason(QString &reason, int &state)
{
    DlgDishRemoveReason *d = new DlgDishRemoveReason();
    bool result = d->exec() == QDialog::Accepted;
    if (result) {
        state = d->fState;
        reason = d->fName;
    }
    delete d;
    return result;
}

void DlgDishRemoveReason::on_lst_clicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    if (index.row() == ui->lst->count() - 1) {
        reject();
        return;
    }
    fName = index.data(Qt::DisplayRole).toString();
}

void DlgDishRemoveReason::on_btnMistake_clicked()
{
    if (ui->lst->currentRow() < 0) {
        C5Message::error(tr("Reason is not selected"));
        return;
    }
    fState = DISH_STATE_MISTAKE;
    accept();
}

void DlgDishRemoveReason::on_btnVoid_clicked()
{
    if (ui->lst->currentRow() < 0) {
        C5Message::error(tr("Reason is not selected"));
        return;
    }
    fState = DISH_STATE_VOID;
    accept();
}
