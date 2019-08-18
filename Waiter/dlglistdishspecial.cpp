#include "dlglistdishspecial.h"
#include "ui_dlglistdishspecial.h"
#include "c5menu.h"

static DlgListDishSpecial *__d = nullptr;

DlgListDishSpecial::DlgListDishSpecial(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::DlgListDishSpecial)
{
    ui->setupUi(this);
}

DlgListDishSpecial::~DlgListDishSpecial()
{
    delete ui;
}

QString DlgListDishSpecial::getSpecial(const QString &dish, const QStringList &dbParams)
{
    if (!__d) {
        __d = new DlgListDishSpecial(dbParams);
    }
    __d->fResult = "";
    if (C5Menu::fDishSpecial.contains(dish)) {
        __d->loadComments(dish);
        __d->exec();
    }
    return __d->fResult;
}

void DlgListDishSpecial::loadComments(const QString &dish)
{
    ui->lst->clear();
    ui->lst->addItems(C5Menu::fDishSpecial[dish]);
    QListWidgetItem *item = new QListWidgetItem(ui->lst);
    item->setIcon(QIcon(":/cancel.png"));
    item->setText(tr("Cancel"));
    ui->lst->addItem(item);
    for (int i = 0; i < ui->lst->count(); i++) {
        ui->lst->item(i)->setSizeHint(QSize(100, 50));
    }
}

void DlgListDishSpecial::on_lst_itemClicked(QListWidgetItem *item)
{
    if (!item) {
        return;
    }
    if (ui->lst->item(ui->lst->count() - 1) != item) {
        fResult = item->text();
    }
    accept();
}
