#include "dlgreceiptlanguage.h"
#include "ui_dlgreceiptlanguage.h"

DlgReceiptLanguage::DlgReceiptLanguage(const QStringList &dbParams, QWidget *parent) :
    C5Dialog(dbParams, parent),
    ui(new Ui::DlgReceiptLanguage)
{
    ui->setupUi(this);
    QListWidgetItem *item = new QListWidgetItem(ui->lst);
    item->setSizeHint(QSize(50, 50));
    item->setData(Qt::UserRole, RECEIPT_LANGUAGE_AM);
    item->setText(tr("ARMENIAN"));
    item->setIcon(QIcon(":/armenia.png"));
    item = new QListWidgetItem(ui->lst);
    item->setSizeHint(QSize(50, 50));
    item->setData(Qt::UserRole, RECEIPT_LANGUAGE_US);
    item->setText(tr("ENGLISH"));
    item->setIcon(QIcon(":/usa.png"));
    item = new QListWidgetItem(ui->lst);
    item->setSizeHint(QSize(50, 50));
    item->setData(Qt::UserRole, RECEIPT_LANGUAGE_US);
    item->setText(tr("CANCEL"));
    item->setIcon(QIcon(":/cancel.png"));
}

DlgReceiptLanguage::~DlgReceiptLanguage()
{
    delete ui;
}

int DlgReceiptLanguage::receipLanguage()
{
    DlgReceiptLanguage *d = new DlgReceiptLanguage(C5Config::dbParams(), __mainWindow);
    int result = 0;
    if (d->exec() == QDialog::Accepted) {
        result = d->fResult;
    }
    delete d;
    return result;
}

void DlgReceiptLanguage::on_lst_clicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    switch (index.row()) {
    case 0:
        fResult = RECEIPT_LANGUAGE_AM;
        break;
    case 1:
        fResult = RECEIPT_LANGUAGE_US;
        break;
    default:
        reject();
        return;
    }
    accept();
}
