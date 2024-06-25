#include "dlgshiftrotation.h"
#include "ui_dlgshiftrotation.h"
#include "datadriver.h"
#include "c5storedraftwriter.h"
#include "c5user.h"
#include "dlgexitwithmessage.h"

DlgShiftRotation::DlgShiftRotation(C5User *user) :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::DlgShiftRotation),
    fUser(user)
{
    ui->setupUi(this);
    ui->de->setDate(QDate::fromString(__c5config.dateCash(), FORMAT_DATE_TO_STR_MYSQL));
    if (!ui->de->date().isValid()) {
        ui->de->setDate(QDate::currentDate());
    }
}

DlgShiftRotation::~DlgShiftRotation()
{
    delete ui;
}

void DlgShiftRotation::on_btnNextDate_clicked()
{
    QDate datecash = QDate::fromString(__c5config.getValue(param_date_cash), FORMAT_DATE_TO_STR_MYSQL);
    if (ui->de->date().addDays(1) > datecash.addDays(1)) {
        C5Message::error(tr("Closing session allowed step by step"));
        return;
    }
    ui->de->setDate(ui->de->date().addDays(1));
}

void DlgShiftRotation::on_btnPrevDate_clicked()
{
    //    QDate datecash = QDate::fromString(__c5config.getValue(param_date_cash), FORMAT_DATE_TO_STR_MYSQL);
    //    if (ui->de->date().addDays(-1) < datecash){
    //        C5Message::error(tr("Closing session not allowed in past"));
    //        return;
    //    }
    ui->de->setDate(ui->de->date().addDays(-1));
}

void DlgShiftRotation::on_btnChange_clicked()
{
    QDate date = QDate::fromString(__c5config.getValue(param_date_cash), FORMAT_DATE_TO_STR_MYSQL);
    if (date == ui->de->date()) {
        C5Message::error(tr("Nothing to change"));
        return;
    }
    if (C5Message::question(tr("Change the shift?")) != QDialog::Accepted) {
        return;
    }
    C5Database db(__c5config.dbParams());
    //Check checkin reservation
    db[":f_checkin"] = date;
    db.exec("select o.f_id, ohh.f_id from o_header_hotel ohh "
            "left join o_header o on o.f_id=ohh.f_id "
            "where o.f_state in (5,6) and ohh.f_checkin=:f_checkin");
    if (db.nextRow()) {
        C5Message::error(tr("Reservations exists on current date."));
        return;
    }
    //Check checkout reservation
    db[":f_checkout"] = date;
    db.exec("select o.f_id, ohh.f_id from o_header_hotel ohh "
            "left join o_header o on o.f_id=ohh.f_id "
            "where o.f_state in (1) and ohh.f_checkout=:f_checkout");
    if (db.nextRow()) {
        C5Message::error(tr("An order exists with end on current date"));
        return;
    }
    db[":f_state"] = ORDER_STATE_OPEN;
    db.exec("select o.f_id, o.f_hall, if(t.f_special_config>0, t.f_special_config, h.f_settings) as f_settings, ohh.f_roomrate "
            "from o_header o "
            "left join h_tables t on t.f_id=o.f_table "
            "left join h_halls h on h.f_id=o.f_hall "
            "left join o_header_hotel ohh on ohh.f_id=o.f_id "
            "where o.f_state=:f_state ");
    C5Database db2(db);
    C5StoreDraftWriter dw(db2);
    int row = 0;
    while (db.nextRow()) {
        db2[":f_settings"] = db.getInt("f_settings");
        db2[":f_key"] = param_waiter_dish_after_day_change;
        db2.exec("select f_value from s_settings_values where f_settings=:f_settings and f_key=:f_key");
        int dishid = 0;
        if (db2.nextRow()) {
            dishid = db2.getString("f_value").toInt();
        }
        if (dishid == 0) {
            C5Message::error(tr("No rooming code defined"));
            return;
        }
        QString id;
        dw.writeOBody(id, db.getString("f_id"), DISH_STATE_OK, dishid,
                      1, 1, db.getDouble("f_roomrate"), db.getDouble("f_roomrate"), 0, 0, 1, "", "",
                      date.toString(FORMAT_DATE_TO_STR), 0, dbdishpart2->adgcode(dbdish->group(dishid)),
                      0, 0, 0, row++, QDateTime::currentDateTime(), "");
    }
    db[":f_value"] = ui->de->date().toString(FORMAT_DATE_TO_STR_MYSQL);
    db[":f_key"] = param_date_cash;
    db.exec("update s_settings_values set f_value=:f_value where f_key=:f_key");
    DlgExitWithMessage::openDialog(tr("Session was changed"));
}

void DlgShiftRotation::on_btnCancel_clicked()
{
    reject();
}
