#include "dlglist2.h"
#include "ui_dlglist2.h"

DlgList2::DlgList2(const QStringList &dbParams, QWidget *parent) :
    C5Dialog(dbParams, parent),
    ui(new Ui::DlgList2)
{
    ui->setupUi(this);
}

DlgList2::~DlgList2()
{
    delete ui;
}

int DlgList2::indexOfList(const QString &title, const QStringList &dbParams, const QStringList &list)
{
   DlgList2 d(dbParams);
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

void DlgList2::on_btnCancel_clicked()
{
    reject();
}

void DlgList2::on_btnOK_clicked()
{
    if (ui->lst->currentRow() > -1) {
        accept();
    }
}
