#include "dlgcreditcardlist.h"
#include "ui_dlgcreditcardlist.h"
#include "c5cafecommon.h"
#include "c5config.h"

DlgCreditCardList::DlgCreditCardList(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::DlgCreditCardList)
{
    ui->setupUi(this);
    foreach (CreditCards cc, C5CafeCommon::fCreditCards) {
        QListWidgetItem *item = new QListWidgetItem(ui->lst);
        item->setSizeHint(QSize(100, 50));
        item->setText(cc.name);
        item->setIcon(QIcon(cc.image));
        item->setData(Qt::UserRole, cc.id);
    }
    QListWidgetItem *item = new QListWidgetItem(ui->lst);
    item->setSizeHint(QSize(100, 50));
    item->setText(tr("Cancel"));
    item->setIcon(QIcon(":/cancel.png"));
    item->setData(Qt::UserRole, 0);
}

DlgCreditCardList::~DlgCreditCardList()
{
    delete ui;
}

bool DlgCreditCardList::getCardInfo(int &id, QString &name)
{
    bool result = false;
    DlgCreditCardList *d = new DlgCreditCardList(__mainWindow);
    if (d->exec() == QDialog::Accepted) {
        id = d->fId;
        name = d->fName;
        result = true;
    }
    return result;
}

void DlgCreditCardList::on_lst_clicked(const QModelIndex &index)
{
    if (index.data(Qt::UserRole).toInt() == 0) {
        reject();
        return;
    }
    fId = index.data(Qt::UserRole).toInt();
    fName = index.data(Qt::DisplayRole).toString();
    accept();
}
