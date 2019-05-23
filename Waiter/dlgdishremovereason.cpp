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
    if (ui->kbd->text().isEmpty()) {
        return;
    }
    fName = ui->kbd->text();
}

void DlgDishRemoveReason::textChanged(const QString &text)
{
    Q_UNUSED(text);
    ui->lst->setCurrentRow(-1);
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
    if (ui->lst->currentRow() < 0 && ui->kbd->text().isEmpty()) {
        C5Message::error(tr("Reason is not selected"));
        return;
    }
    if (ui->lst->currentRow() < 0) {
        fName = ui->kbd->text();
    }
    fState = DISH_STATE_MISTAKE;
    accept();
}

void DlgDishRemoveReason::on_btnVoid_clicked()
{
    if (ui->lst->currentRow() < 0 && ui->kbd->text().isEmpty()) {
        C5Message::error(tr("Reason is not selected"));
        return;
    }
    if (ui->lst->currentRow() < 0) {
        fName = ui->kbd->text();
    }
    fState = DISH_STATE_VOID;
    accept();
}
