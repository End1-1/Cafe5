#include "c5salefromstoreorder.h"
#include "ui_c5salefromstoreorder.h"
#include "c5mainwindow.h"
#include "c5storedraftwriter.h"
#include <QMenu>

C5SaleFromStoreOrder::C5SaleFromStoreOrder(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::C5SaleFromStoreOrder)
{
    ui->setupUi(this);
    ui->tblData->setColumnWidths(ui->tblData->columnCount(), 0, 0, 300, 80, 80, 80, 80, 0);
    ui->leID->setVisible(false);
    //ui->btnRemove->setVisible(pr(fDBParams, cp_t5_refund_goods));
}

C5SaleFromStoreOrder::~C5SaleFromStoreOrder()
{
    delete ui;
}

void C5SaleFromStoreOrder::openOrder(const QStringList &dbParams, const QString &id)
{
    C5SaleFromStoreOrder *d = new C5SaleFromStoreOrder(dbParams);
    d->loadOrder(id);
    d->exec();
    delete d;
}

void C5SaleFromStoreOrder::loadOrder(const QString &id)
{
    ui->leID->setText(id);
    C5Database db(fDBParams);
    db[":f_id"] = id;
    db.exec("select f_datecash, f_timeclose, concat(f_prefix, f_hallid) as f_userid, f_amountcash, f_amountcard, f_source from o_header where f_id=:f_id");
    if (db.nextRow()) {
        ui->deDate->setDate(db.getDate(0));
        ui->teTime->setText(db.getString(1));
        ui->leUserId->setText(db.getString("f_userid"));
        ui->leTotalCash->setDouble(db.getDouble("f_amountcash"));
        ui->leTotalCard->setDouble(db.getDouble("f_amountcard"));
        ui->btnRemove->setVisible(db.getInt("f_source") > 0);
    } else {
        C5Message::error(tr("No such order"));
        return;
    }
    db[":f_id"] = id;
    db.exec("select og.f_id, og.f_goods, g.f_name, og.f_qty, gu.f_name, og.f_price, og.f_total, "
            "og.f_store "
            "from o_goods og "
            "left join c_goods g on g.f_id=og.f_goods "
            "left join c_units gu on gu.f_id=g.f_unit "
            "where og.f_header=:f_id");
    while (db.nextRow()) {
        int row = ui->tblData->addEmptyRow();
        ui->tblData->setString(row, 0, db.getString(0));
        ui->tblData->setInteger(row, 1, db.getInt(1));
        ui->tblData->setString(row, 2, db.getString(2));
        ui->tblData->setDouble(row, 3, db.getDouble(3));
        ui->tblData->setString(row, 4, db.getString(4));
        ui->tblData->setDouble(row, 5, db.getDouble(5));
        ui->tblData->setDouble(row, 6, db.getDouble(6));
        ui->tblData->setInteger(row, 7, db.getInt(7));
    }
}

void C5SaleFromStoreOrder::on_btnRemove_clicked()
{
    if (C5Message::question(tr("Confirm to callback")) != QDialog::Accepted) {
        return;
    }
    C5Database db(fDBParams);
    db.startTransaction();
    QString newDocId = db.uuid();
    db[":f_id"] = ui->leID->text();
    db.exec("select f_hall, f_table from o_header where f_id=:f_id");
    int defaultHall = 0;
    int defaultTable = 0;
    if (db.nextRow()) {
        defaultHall = db.getInt(0);
        defaultTable = db.getInt(1);
    }

    //Header and goods
    db[":f_id"] = defaultHall;
    db.exec("select f_counter + 1, f_prefix as f_counter from h_halls where f_id=:f_id for update");
    QString pref, hallid;
    if (db.nextRow()) {
        hallid = db.getString(0);
        pref = db.getString(1);
        db[":f_counter"] = db.getInt(0);
        db.update("h_halls", where_id(defaultHall));
    }

    db[":f_id"] = newDocId;
    db[":f_hallid"] = hallid.toInt();
    db[":f_prefix"] = pref;
    db[":f_state"] = ORDER_STATE_CLOSE;
    db[":f_hall"] = defaultHall;
    db[":f_table"] = defaultTable;
    db[":f_dateopen"] = QDate::currentDate();
    db[":f_dateclose"] = QDate::currentDate();
    db[":f_datecash"] = QDate::currentDate();
    db[":f_timeopen"] = QTime::currentTime();
    db[":f_timeclose"] = QTime::currentTime();
    db[":f_staff"] = __userid;
    db[":f_comment"] = "";
    db[":f_print"] = 1;
    db[":f_amountTotal"] = (ui->leTotalCash->getDouble() + ui->leTotalCard->getDouble()) * -1;
    db[":f_amountCash"] = ui->leTotalCash->getDouble() * -1;
    db[":f_amountCard"] = ui->leTotalCard->getDouble() * -1;
    db[":f_amountBank"] = 0;
    db[":f_amountOther"] = 0;
    db[":f_amountService"] = 0;
    db[":f_amountDiscount"] = 0;
    db[":f_serviceMode"] = 0;
    db[":f_serviceFactor"] = 0;
    db[":f_discountFactor"] = 0;
    db[":f_creditCardId"] = 0;
    db[":f_otherId"] = 0;
    db[":f_source"] = -2;
    if (db.insert("o_header", false) == 0) {
        db.rollback();
        db.close();
        C5Message::error(db.fLastError);
        return;
    }
    for (int i = 0; i < ui->tblData->rowCount(); i++) {
        db[":f_id"] = C5Database::uuid();
        db[":f_header"] = newDocId;
        db[":f_store"] = ui->tblData->getInteger(i, 7);
        db[":f_goods"] = ui->tblData->getInteger(i, 1);
        db[":f_qty"] = ui->tblData->getDouble(i, 3);
        db[":f_price"] = ui->tblData->getDouble(i, 5);
        db[":f_total"] = ui->tblData->getDouble(i, 6);
        db[":f_tax"] = 0;
        db[":f_sign"] = -1;
        db[":f_taxdept"] = 0;
        db[":f_row"] = i + 1;
        if (db.insert("o_goods", false) == 0) {
            db.rollback();
            db.close();
            return;
        }
    }

    //Cash
    QList<int> cashList;
    QList<double> amountList;
    db[":f_oheader"] = ui->leID->text();
    db.exec("select f_cash, f_amount from e_cash where f_oheader=:f_oheader");
    while (db.nextRow()) {
        cashList.append(db.getInt(0));
        amountList.append(db.getDouble(1));
    }
    for (int i = 0; i < cashList.count(); i++) {
        db[":f_id"] = DOC_TYPE_CASH;
        if (!db.exec("select f_counter + 1 from a_type where f_id=:f_id for update")) {
            db.rollback();
            C5Message::error(db.fLastError);
            return;
        }
        if (!db.nextRow()) {
            db[":f_id"] = DOC_TYPE_CASH;
            db[":f_counter"] = 1;
            db[":f_name"] = tr("Cash doc");
            if (!db.insert("f_counter", false)) {
                db.rollback();
                C5Message::error(db.fLastError);
                return;
            }
            db.commit();
            db[":f_id"] = DOC_TYPE_CASH;
            if (!db.exec("select f_counter from a_type where f_id=:f_id for update")) {
                db.rollback();
                C5Message::error(db.fLastError);
                return;
            }
            db.nextRow();
        }
        int counter = db.getInt(0);
        db[":f_id"] = DOC_TYPE_CASH;
        db[":f_counter"] = counter;
        if (!db.exec("update a_type set f_counter=:f_counter where f_id=:f_id and f_counter<:f_counter")) {
            db.rollback();
            C5Message::error(db.fLastError);
            return;
        }

        QJsonObject jo;
        jo["cashin"] = cashList.at(i);
        jo["cashout"] = 0;
        jo["storedoc"] = "";
        jo["relation"] = "1";
        QJsonDocument jd;
        jd.setObject(jo);
        QString fUuid = C5Database::uuid();
        db[":f_id"] = fUuid;
        db[":f_operator"] = __userid;
        db[":f_state"] = DOC_STATE_SAVED;
        db[":f_type"] = DOC_TYPE_CASH;
        db[":f_createdate"] = QDate::currentDate();
        db[":f_createtime"] = QTime::currentTime();
        if (!db.insert("a_header", false)) {
            db.rollback();
            C5Message::error(db.fLastError);
            return;
        }

        db[":f_partner"] = 0;
        db[":f_userid"] = counter;
        db[":f_date"] = QDate::currentDate();
        db[":f_amount"] = amountList.at(i) * -1;
        db[":f_comment"] = tr("Rollback") + " " + QDate::currentDate().toString("dd.MM.yyyy");
        db[":f_raw"] = jd.toJson();
        if (!db.update("a_header", where_id(fUuid))) {
            db.rollback();
            C5Message::error(db.fLastError);
            return ;
        }

        db[":f_header"] = fUuid;
        db[":f_sign"] = -1;
        db[":f_cash"] = cashList.at(i);
        db[":f_remarks"] = tr("Rollback") + " " + QDate::currentDate().toString("dd.MM.yyyy");
        db[":f_amount"] = amountList.at(i);
        db[":f_autoinput"] = 1;
        if (!db.insert("e_cash", false)) {
            db.rollback();
            C5Message::error(db.fLastError);
            return;
        }
    }

    //Store
    C5StoreDraftWriter dw(db);
    if (!dw.writeFromShopInput(QDate::currentDate(), ui->leID->text())) {
        db.rollback();
        C5Message::error(dw.fErrorMsg);
        return;
    }

    db[":f_id"] = ui->leID->text();
    db.exec("update o_header set f_source=f_source*-1 where f_id=:f_id");
    db.commit();
    ui->btnRemove->setVisible(false);
    C5Message::info(tr("Roolback complete"));
}
