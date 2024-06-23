#include "dlglistdishspecial.h"
#include "ui_dlglistdishspecial.h"
#include "c5tabledata.h"

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

bool DlgListDishSpecial::getSpecial(int dish, QString &special)
{
    if (!__d) {
        __d = new DlgListDishSpecial(QStringList());
    }
    __d->fResult = "";
    QJsonArray sp = objs("d_special");
    __d->ui->lst->clear();
    for (int i = 0; i < sp.size(); i++) {
        const QJsonObject &jo = sp.at(i).toObject();
        if (jo["f_dish"].toInt() == dish) {
            __d->ui->lst->addItem(jo["f_comment"].toString());
        }
    }
    if (__d->ui->lst->count() > 0) {
        QListWidgetItem *item = new QListWidgetItem(__d->ui->lst);
        item->setIcon(QIcon(":/cancel.png"));
        item->setText(tr("Cancel"));
        __d->ui->lst->addItem(item);
        for (int i = 0; i < __d->ui->lst->count(); i++) {
            __d->ui->lst->item(i)->setSizeHint(QSize(100, 50));
        }
        if (__d->exec() == QDialog::Accepted) {
            special = __d->fResult.trimmed();
            return special.length() > 0;
        }
    }
    return true;
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
