#include "dlglist.h"
#include "ui_dlglist.h"

DlgList::DlgList() :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::DlgList)
{
    ui->setupUi(this);
}

DlgList::~DlgList()
{
    delete ui;
}

bool DlgList::getValue(const QStringList &names, int &index)
{
    DlgList d;
    for (int i = 0; i < names.count(); i++) {
        QListWidgetItem *item = new QListWidgetItem(d.ui->lst);
        item->setSizeHint(QSize(50, 50));
        item->setText(names.at(i));
        d.ui->lst->addItem(item);
    }
    QListWidgetItem *item = new QListWidgetItem(d.ui->lst);
    item->setSizeHint(QSize(50, 50));
    item->setText("Cancel");
    item->setIcon(QIcon(":/cancel.png"));
    d.ui->lst->addItem(item);
    if (d.exec() == QDialog::Accepted) {
        index = d.fResult;
        return true;
    }
    return false;
}

void DlgList::on_lst_clicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    if (index.row() == ui->lst->count() - 1) {
        reject();
    }
    fResult = index.row();
    if (fResult < ui->lst->count() - 1) {
        accept();
    } else {
        reject();
    }
}
