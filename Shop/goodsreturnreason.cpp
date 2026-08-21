#include "goodsreturnreason.h"
#include "ui_goodsreturnreason.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QListWidgetItem>
#include "c5message.h"
#include "ninterface.h"

GoodsReturnReason::GoodsReturnReason(C5User *user) :
    C5Dialog(user),
    ui(new Ui::GoodsReturnReason)
{
    ui->setupUi(this);
    fReason = 0;
}

GoodsReturnReason::~GoodsReturnReason()
{
    delete ui;
}

void GoodsReturnReason::setReasons(const QJsonArray &reasons)
{
    ui->lst->clear();
    for (const QJsonValue &v : reasons) {
        const QJsonObject o = v.toObject();
        const int id = o.value(QStringLiteral("f_id")).toInt();
        if (id <= 0) {
            continue;
        }
        auto *item = new QListWidgetItem(ui->lst);
        item->setText(o.value(QStringLiteral("f_name")).toString());
        item->setData(Qt::UserRole, id);
        item->setSizeHint(QSize(100, 50));
        ui->lst->addItem(item);
    }

    auto *cancel = new QListWidgetItem(ui->lst);
    cancel->setText(tr("Cancel"));
    cancel->setData(Qt::UserRole, 0);
    cancel->setSizeHint(QSize(100, 50));
    ui->lst->addItem(cancel);
}

void GoodsReturnReason::on_lst_itemClicked(QListWidgetItem *item)
{
    if (!item) {
        return;
    }

    fReason = item->data(Qt::UserRole).toInt();
    fReasonName = item->text();
    accept();
}
