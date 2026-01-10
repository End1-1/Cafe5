#include "goodsreturnreason.h"
#include "ui_goodsreturnreason.h"
#include "c5config.h"
#include "c5database.h"
#include <QListWidgetItem>

GoodsReturnReason::GoodsReturnReason(C5User *user) :
    C5Dialog(user),
    ui(new Ui::GoodsReturnReason)
{
    ui->setupUi(this);
    C5Database db;
    db.exec("select f_id, f_name from o_goods_return_reason");

    while(db.nextRow()) {
        QListWidgetItem *item = new QListWidgetItem(ui->lst);
        item->setText(db.getString("f_name"));
        item->setData(Qt::UserRole, db.getInt("f_id"));
        item->setSizeHint(QSize(100, 50));
        ui->lst->addItem(item);
    }

    QListWidgetItem *item = new QListWidgetItem(ui->lst);
    item->setText(tr("Cancel"));
    item->setData(Qt::UserRole, 0);
    item->setSizeHint(QSize(100, 50));
    ui->lst->addItem(item);
    fReason = 0;
}

GoodsReturnReason::~GoodsReturnReason()
{
    delete ui;
}

void GoodsReturnReason::on_lst_itemClicked(QListWidgetItem *item)
{
    if(!item) {
        return;
    }

    fReason = item->data(Qt::UserRole).toInt();
    fReasonName = item->text();
    accept();
}
