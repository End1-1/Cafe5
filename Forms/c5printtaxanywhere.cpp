#include "c5printtaxanywhere.h"
#include "ui_c5printtaxanywhere.h"
#include "c5cache.h"
#include "threadsendmessage.h"
#include "chatmessage.h"
#include <QMovie>
#include <QTimer>

C5PrintTaxAnywhere::C5PrintTaxAnywhere(const QStringList &dbParams, const QString &id) :
    C5Dialog(dbParams),
    ui(new Ui::C5PrintTaxAnywhere),
    fId(id),
    fMovie(nullptr),
    fTimeout(0)
{
    ui->setupUi(this);
    C5Database db(fDBParams);
    QString sql = QString("select sn.f_id, sn.f_name, sv.f_key, sv.f_value "
                        "from s_settings_values sv "
                        "left join s_settings_names sn on sn.f_id=sv.f_settings "
                        "where sv.f_key in (%1, %2, %3, %4) ")
            .arg(param_default_hall)
            .arg(param_default_table)
            .arg(param_default_store)
            .arg(param_tax_port);
    db.exec(sql);
    C5Cache *hall = C5Cache::cache(fDBParams, cache_halls);
    C5Cache *tables = C5Cache::cache(fDBParams, cache_tables);
    C5Cache *store = C5Cache::cache(fDBParams, cache_goods_store);
    QList<int> validid;
    while (db.nextRow()) {
        int row = -1;
        for (int i = 0; i < ui->tbl->rowCount(); i++) {
            if (ui->tbl->item(i, 0)->data(Qt::UserRole).toInt() == db.getInt("f_id")) {
                row = i;
                break;
            }
        }
        if (row < 0) {
            row = ui->tbl->addEmptyRow();
            ui->tbl->item(row, 0)->setData(Qt::UserRole, db.getInt("f_id"));
        }
        QList<QVariant> vals;
        switch (db.getInt("f_key")) {
        case param_default_hall:
            vals = hall->getValuesForId(db.getInt("f_value"));
            if (vals.count() > 0) {
                ui->tbl->setString(row, 0, vals.at(1).toString());
            }
            break;
        case param_default_table:
            vals = tables->getValuesForId(db.getInt("f_value"));
            if (vals.count() > 0) {
                ui->tbl->setString(row, 1, vals.at(1).toString());
            }
            break;
        case param_default_store:
            ui->tbl->item(row, 1)->setData(Qt::UserRole, db.getValue("f_value"));
            vals = store->getValuesForId(db.getInt("f_value"));
            if (vals.count() > 0) {
                ui->tbl->setString(row, 2, vals.at(1).toString());
            }
            break;
        case param_tax_port:
            if (db.getInt("f_value") > 0) {
                validid.append(db.getInt("f_id"));
            }
            break;
        }
    }
    for (int i = ui->tbl->rowCount() - 1; i > -1; i--) {
        if (!validid.contains(ui->tbl->item(i, 0)->data(Qt::UserRole).toInt())) {
            ui->tbl->removeRow(i);
        }
    }

    auto *t = new QTimer(this);
    connect(t, &QTimer::timeout, this, &C5PrintTaxAnywhere::timeout);
    t->start(2000);
}

C5PrintTaxAnywhere::~C5PrintTaxAnywhere()
{
    if (fMovie) {
        ui->lbProgress->setMovie(nullptr);
        delete fMovie;
    }
    delete ui;
}

void C5PrintTaxAnywhere::timeout()
{
    auto *t = static_cast<QTimer*>(sender());
    if (fTimeout < 0) {
        t->deleteLater();
        return;
    }
    C5Database db(fDBParams);
    db[":f_id"] = fId;
    db.exec("select f_receiptnumber from o_tax where f_id=:f_id");
    fTimeout++;
    if (db.nextRow()) {
        if (db.getInt("f_receiptnumber") > 0) {
            fReceiptNumber = db.getString("f_receiptnumber");
            t->stop();
            fMovie->stop();
            ui->lbProgress->setMovie(nullptr);
            C5Message::info(tr("Printed"));
            accept();
            return;
        }
    }
    if (fTimeout % 15 == 0) {
        if (C5Message::question(tr("The print job takes longer than usual. Wait?")) != QDialog::Accepted) {
            t->stop();
            fMovie->stop();
            ui->lbProgress->setMovie(nullptr);
            ui->btnPrintTax->setEnabled(true);
        }
    }
}

void C5PrintTaxAnywhere::result(const QJsonObject &jo)
{
    sender()->deleteLater();
    if (jo["status"].toInt() > 0) {
        fTimeout = -1;
        fMovie->stop();
        ui->lbProgress->setMovie(nullptr);
        C5Message::error(jo["data"].toString());
    }
}

void C5PrintTaxAnywhere::on_btnPrintTax_clicked()
{
    int row = ui->tbl->currentRow();
    if (row < 0) {
        C5Message::error(tr("Nothing was selected"));
        return;
    }
    int store = ui->tbl->item(row, 1)->data(Qt::UserRole).toInt();
    C5Database db(fDBParams);
    db[":f_id"] = fId;
    db.exec("select concat(f_prefix, f_hallid), f_amounttotal from o_header where f_id=:f_id");
    QString ordernum;
    if (db.nextRow()) {
        ordernum = db.getString(0);
    }
    QJsonObject jo;
    QJsonObject jmsg;
    jmsg["id"] = fId;
    jmsg["ordernum"] = ordernum;
    jmsg["orderamount"] = float_str(db.getDouble("f_amounttotal"), 2);
    jo["action"] = MSG_PRINT_TAX;
    jo["usermessage"] = jmsg;
    QJsonDocument jdoc(jo);
    auto *t = new ThreadSendMessage();
    connect(t, &ThreadSendMessage::result, this, &C5PrintTaxAnywhere::result);
    t->send(store, jdoc.toJson(QJsonDocument::Compact));
    fMovie = new QMovie(":/progressbar.gif");
    fMovie->start();
    ui->lbProgress->setAttribute(Qt::WA_NoSystemBackground);
    ui->lbProgress->setMovie(fMovie);
    ui->btnPrintTax->setEnabled(false);
}
