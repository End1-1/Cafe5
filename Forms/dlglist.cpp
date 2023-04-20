#include "dlglist.h"
#include "ui_dlglist.h"

DlgList::DlgList(const QStringList &dbParams, QWidget *parent) :
    C5Dialog(dbParams, parent),
    ui(new Ui::DlgList)
{
    ui->setupUi(this);
}

DlgList::~DlgList()
{
    delete ui;
}

int DlgList::indexOfList(const QString &title, const QStringList &dbParams, const QStringList &list)
{
   DlgList d(dbParams);
   d.setWindowTitle(title);
   for (int i = 0; i < list.count(); i++) {
    QListWidgetItem *item = new QListWidgetItem();
    item->setText(list.at(i));
    item->setSizeHint(QSize(200, 40));
    d.ui->lst->addItem(item);
   }
   if (d.exec() == QDialog::Accepted) {
       return d.ui->lst->currentRow();
   }
   return -1;
}

void DlgList::on_btnCancel_clicked()
{
    reject();
}

void DlgList::on_btnOK_clicked()
{
    if (ui->lst->currentRow() > -1) {
        accept();
    }
}
