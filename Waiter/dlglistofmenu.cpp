#include "dlglistofmenu.h"
#include "ui_dlglistofmenu.h"
#include "c5menu.h"

DlgListOfMenu::DlgListOfMenu(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::DlgListOfMenu)
{
    ui->setupUi(this);
    for (int id: dbmenuname->list()) {
        QListWidgetItem *item = new QListWidgetItem(ui->lst);
        item->setText(dbmenuname->name(id));
        item->setData(Qt::UserRole, id);
        ui->lst->addItem(item);
    }
    QListWidgetItem *item = new QListWidgetItem(ui->lst);
    item->setIcon(QIcon(":/cancel.png"));
    item->setText(tr("Cancel"));
    ui->lst->addItem(item);
    for (int i = 0; i < ui->lst->count(); i++) {
        ui->lst->item(i)->setSizeHint(QSize(100, 50));
    }

}

DlgListOfMenu::~DlgListOfMenu()
{
    delete ui;
}

bool DlgListOfMenu::getMenuId(int &id, const QStringList &dbParams)
{
    bool result = false;
    DlgListOfMenu *d = new DlgListOfMenu(dbParams);
    if (d->exec() == QDialog::Accepted) {
        result = true;
        id = d->fMenuId;
    }
    delete d;
    return result;
}
void DlgListOfMenu::on_lst_clicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    if (index.row() == ui->lst->count() - 1) {
        reject();
        return;
    }
    fMenuId = index.data(Qt::UserRole).toInt();
    accept();
}
