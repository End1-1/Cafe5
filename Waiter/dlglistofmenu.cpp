#include "dlglistofmenu.h"
#include "ui_dlglistofmenu.h"
#include "c5tabledata.h"

DlgListOfMenu::DlgListOfMenu() :
    C5Dialog(),
    ui(new Ui::DlgListOfMenu)
{
    ui->setupUi(this);
    QJsonArray ja = objs("d_menu_names");

    for(int i = 0; i < ja.size(); i++) {
        const QJsonObject &jo = ja.at(i).toObject();
        QListWidgetItem *item = new QListWidgetItem(ui->lst);
        item->setText(jo["f_name"].toString());
        item->setData(Qt::UserRole, jo["f_id"].toInt());
        ui->lst->addItem(item);
    }

    QListWidgetItem *item = new QListWidgetItem(ui->lst);
    item->setIcon(QIcon(":/cancel.png"));
    item->setText(tr("Cancel"));
    ui->lst->addItem(item);

    for(int i = 0; i < ui->lst->count(); i++) {
        ui->lst->item(i)->setSizeHint(QSize(100, 50));
    }
}

DlgListOfMenu::~DlgListOfMenu()
{
    delete ui;
}

bool DlgListOfMenu::getMenuId(int& id)
{
    bool result = false;
    DlgListOfMenu *d = new DlgListOfMenu();

    if(d->exec() == QDialog::Accepted) {
        result = true;
        id = d->fMenuId;
    }

    delete d;
    return result;
}
void DlgListOfMenu::on_lst_clicked(const QModelIndex &index)
{
    if(!index.isValid()) {
        return;
    }

    if(index.row() == ui->lst->count() - 1) {
        reject();
        return;
    }

    fMenuId = index.data(Qt::UserRole).toInt();
    accept();
}
