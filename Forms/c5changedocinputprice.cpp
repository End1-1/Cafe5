#include "c5changedocinputprice.h"
#include "ui_c5changedocinputprice.h"
#include "c5cache.h"
#include "c5message.h"
#include "c5database.h"
#include "c5config.h"
#include "c5user.h"

C5ChangeDocInputPrice::C5ChangeDocInputPrice(const QStringList &dbParams, const QString &uuid) :
    C5Dialog(dbParams),
    ui(new Ui::C5ChangeDocInputPrice)
{
    ui->setupUi(this);
    ui->leUuid->setText(uuid);
    ui->leGoods->setSelector(dbParams, ui->leGoodsName, cache_goods, 1, 3);
    C5Database db(dbParams);
    db[":f_id"] = uuid;
    db.exec("select f_goods, f_price from a_store where f_id=:f_id");
    if (db.nextRow()) {
        ui->leGoods->setValue(db.getString("f_goods"));
        ui->lePrice->setDouble(db.getDouble("f_price"));
        ui->leGoods->fixValue();
        ui->lePrice->fixValue();
    } else {
        C5Message::error(tr("This record not exists"));
        ui->btnChange->setEnabled(false);
    }
}

C5ChangeDocInputPrice::~C5ChangeDocInputPrice()
{
    delete ui;
}

void C5ChangeDocInputPrice::changePrice(const QStringList &dbParams, const QString &uuid)
{
    auto *d = new C5ChangeDocInputPrice(dbParams, uuid);
    d->exec();
    delete d;
}

void C5ChangeDocInputPrice::on_btnCancel_clicked()
{
    reject();
}

void C5ChangeDocInputPrice::on_btnChange_clicked()
{
    if (ui->leGoods->getInteger() == 0) {
        C5Message::error(tr("Goods is not selected"));
        return;
    }
    C5Database db(fDBParams);
    db[":f_id"] = ui->leUuid->text();
    db.exec("select f_base from a_store where f_id=:f_id");
    db.nextRow();
    QString base = db.getString("f_base");
    db[":f_base"] = base;
    db[":f_goods"] = ui->leGoods->getInteger();
    db[":f_price"] = ui->lePrice->getDouble();
    db.exec("update a_store set f_goods=:f_goods, f_price=:f_price, f_total=f_qty*:f_price where f_base=:f_base");
    db[":f_base"] = base;
    db.exec("select f_draft, f_document from a_store where f_base=:f_base");
    QSet<QString> docs;
    QStringList drafts;
    while (db.nextRow()) {
        drafts.append(db.getString("f_draft"));
        docs.insert(db.getString("f_document"));
    }
    for (const QString &s : drafts) {
        db[":f_id"] = s;
        db[":f_goods"] = ui->leGoods->getInteger();
        db[":f_price"] = ui->lePrice->text();
        db.exec("update a_store_draft set f_goods=:f_goods, f_price=:f_price, f_total=:f_price*f_qty where f_id=:f_id");
    }
    for (const QString &s : docs) {
        db[":f_id"] = s;
        db.exec("update a_header set f_amount=(select sum(f_total) from a_store_draft where f_document=:f_id) where f_id=:f_id");
    }
    db[":f_date"] = QDateTime::currentDateTime();
    db[":f_user"] = __user->id();
    db[":f_base"] = base;
    db[":f_old_goods"] = ui->leGoods->old();
    db[":f_new_goods"] = ui->leGoods->text();
    db[":f_old_price"] = ui->lePrice->old();
    db[":f_new_price"] = ui->lePrice->text();
    db.insert("s_log_store_price", false);
    accept();
}
