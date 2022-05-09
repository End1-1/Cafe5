#include "dlgdishremovereason.h"
#include "ui_dlgdishremovereason.h"
#include "c5cafecommon.h"
#include "c5config.h"
#include "c5utils.h"
#include "c5message.h"
#include "datadriver.h"

DlgDishRemoveReason::DlgDishRemoveReason(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgDishRemoveReason)
{
    ui->setupUi(this);
    foreach (int id, dbdishremovereason->list()) {
        QListWidgetItem *item = new QListWidgetItem(ui->lst);
        item->setSizeHint(QSize(50, 50));
        item->setText(dbdishremovereason->name(id));
        ui->lst->addItem(item);
    }
    QListWidgetItem *item = new QListWidgetItem(ui->lst);
    item->setSizeHint(QSize(50, 50));
    item->setText(tr("Other"));
    ui->lst->addItem(item);
    connect(ui->kbd, SIGNAL(accept()), this, SLOT(kbdAccept()));
    connect(ui->kbd, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
    connect(ui->kbd, SIGNAL(reject()), this, SLOT(reject()));
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

void DlgDishRemoveReason::kbdAccept()
{
    if (ui->lst->currentRow() < 0) {
        fName = ui->kbd->text();
    } else {
        fName = ui->lst->currentItem()->text();
    }
    if (fName.isEmpty()) {
        C5Message::error(tr("Reason is not selected"));
        return;
    }

    if (ui->btnMistake->isChecked()) {
        fState = DISH_STATE_MISTAKE;
    } else if (ui->btnVoid->isChecked()) {
        fState = DISH_STATE_VOID;
    } else {
        C5Message::error(tr("Void or mistake must be selected"));
        return;
    }
    accept();
}

void DlgDishRemoveReason::textChanged(const QString &text)
{
    Q_UNUSED(text);
    ui->lst->setCurrentRow(-1);
}

void DlgDishRemoveReason::on_btnMistake_clicked()
{
    ui->btnVoid->setChecked(false);

}

void DlgDishRemoveReason::on_btnVoid_clicked()
{
    ui->btnMistake->setChecked(false);
}
