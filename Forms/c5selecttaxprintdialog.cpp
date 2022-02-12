#include "c5selecttaxprintdialog.h"
#include "ui_c5selecttaxprintdialog.h"
#include "threadsendmessage.h"
#include "chatmessage.h"
#include "c5cache.h"

C5SelectTaxPrintDialog::C5SelectTaxPrintDialog(const QString &id, const QStringList &dbParams) :
    ui(new Ui::C5SelectTaxPrintDialog),
    C5Dialog(dbParams),
    fId(id)
{
    C5Database db(fDBParams);
    QString sql = QString("select sn.f_id, sn.f_name, sv.f_key, sv.f_value "
                        "from f_settings_values sv "
                        "left join f_settings name sn on s.f_settings=sv.f_settings "
                        "where sv.f_key in (%1, %2, %3) ")
            .arg(param_default_hall)
            .arg(param_default_table)
            .arg(param_default_store);
    db.exec(sql);
    C5Cache *hall = C5Cache::cache(fDBParams, cache_halls);
    C5Cache *tables = C5Cache::cache(fDBParams, cache_tables);
    C5Cache *store = C5Cache::cache(fDBParams, cache_goods_store);
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
            ui->tbl->item(row, 0)->setData(Qt::UserRole, db.getDate("f_id"));
        }
        switch (db.getInt("f_key")) {
        case param_default_hall:
            ui->tbl->setString(row, 0, hall->getValuesForId(db.getInt("f_value")).at(1).toString());
            break;
        case param_default_table:
            ui->tbl->setString(row, 1, tables->getValuesForId(db.getInt("f_value")).at(1).toString());
            break;
        case param_default_store:
            ui->tbl->item(row, 1)->setData(Qt::UserRole, db.getValue("f_value"));
            ui->tbl->setString(row, 2, store->getValuesForId(db.getInt("f_value")).at(1).toString());
            break;
        }
    }
}

C5SelectTaxPrintDialog::~C5SelectTaxPrintDialog()
{
    delete ui;
}

void C5SelectTaxPrintDialog::on_pushButton_clicked()
{
    int row = ui->tbl->currentRow();
    if (row < 0) {
        C5Message::error(tr("Nothing was selected"));
        return;
    }
    int store = ui->tbl->item(row, 1)->data(Qt::UserRole).toInt();
    QJsonObject jo;
    jo["action"] = MSG_GOODS_RESERVE;
    jo["usermessage"] = fId;
    QJsonDocument jdoc(jo);
    ThreadSendMessage t;
    t.send(store, jdoc.toJson(QJsonDocument::Compact));
}
