#include "dlgpreorderw.h"
#include "ui_dlgpreorderw.h"
#include "c5orderdriver.h"
#include "dlgpassword.h"
#include "dlgtext.h"
#include "datadriver.h"
#include "dlgface.h"
#include "dlgcl.h"
#include "c5logtoserverthread.h"
#include <QCalendarWidget>
#include <QDateEdit>

DlgPreorder::DlgPreorder(C5OrderDriver *w, C5User *user, const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::DlgPreorder),
    fOrder(w)
{
    ui->setupUi(this);
    fUser = user;
    fGuestCode = 0;
    fOrder = w;
    ui->leCityLedger->setInteger(w->preorder("f_cityledger").toInt());
    ui->leGuestName->setText(w->preorder("f_guestname").toString());
    ui->leDate->setDate(w->preorder("f_datefor").toDate());
    ui->timeEdit->setTime(w->preorder("f_timefor").toTime());
    ui->leGuest->setValue(w->preorder("f_guests").toInt());
    ui->leCash->setDouble(w->preorder("f_prepaidcash").toDouble());
    ui->leCard->setDouble(w->preorder("f_prepaidcard").toDouble());
    ui->lePayX->setDouble(w->preorder("f_prepaidpayx").toDouble());
    if (w->preorder("f_fortable").toInt() > 0) {
        ui->leTable->setText(dbtable->name(w->preorder("f_fortable").toInt()));
    }
    ui->leDate->setMinimumDate(QDate::fromString(__c5config.getValue(param_date_cash), FORMAT_DATE_TO_STR_MYSQL));
    ui->leDateCheckout->setMinimumDate(QDate::fromString(__c5config.getValue(param_date_cash), FORMAT_DATE_TO_STR_MYSQL));
    ui->leDateCheckout->setDate(ui->leDate->date().addDays(1));
    dbpartner->refresh();
    setHotelMode(false);
}

DlgPreorder &DlgPreorder::setHotelMode(bool v)
{
    fHotelMode = v;
    ui->lbCheckout->setVisible(v);
    ui->lbDays->setVisible(v);
    ui->btnCheckDown->setVisible(v);
    ui->btnCheckoutUp->setVisible(v);
    ui->leDateCheckout->setVisible(v);
    ui->btnDaysUp->setVisible(v);
    ui->btnDaysDown->setVisible(v);
    ui->leDays->setVisible(v);
    ui->lbRoomRate->setVisible(v);
    ui->leRoomRate->setVisible(v);
    ui->btnEditRate->setVisible(v);
    ui->lbPreorder->setText(v ? tr("Reservation") : tr("Preorder"));
    ui->lbTable->setText(v ? tr("Room") : tr("Table"));
    fGuestCode = fOrder->headerHotel("f_guest").toInt();
    if (v) {
        disconnect(ui->leDate, &QDateEdit::dateChanged, this, &DlgPreorder::on_leDate_dateChanged);
        disconnect(ui->leDateCheckout, &QDateEdit::dateChanged, this, &DlgPreorder::on_leDateCheckout_dateChanged);
        disconnect(ui->leDays, SIGNAL(valueChanged(int)), this, SLOT(on_leDays_valueChanged(int)));
        ui->leGuest->setValue(fOrder->headerHotel("f_guestcount").toInt());
        ui->leDateCheckout->setDate(fOrder->headerHotel("f_checkout").toDate());
        ui->leTable->setText(dbtable->name(fOrder->headerValue("f_table").toInt()));
        ui->leDate->setDate(fOrder->headerHotel("f_checkin").toDate());
        ui->leRoomRate->setDouble(fOrder->headerHotel("f_roomrate").toDouble());
        if (fGuestCode > 0) {
            ui->leGuestName->setText(dbpartner->contact(fGuestCode));
        }
        on_leDate_dateChanged(fOrder->headerHotel("f_checkin").toDate());
        connect(ui->leDate, &QDateEdit::dateChanged, this, &DlgPreorder::on_leDate_dateChanged);
        connect(ui->leDateCheckout, &QDateEdit::dateChanged, this, &DlgPreorder::on_leDateCheckout_dateChanged);
        connect(ui->leDays, SIGNAL(valueChanged(int)), this, SLOT(on_leDays_valueChanged(int)));
        if (fOrder->headerValue("f_state").toInt() == ORDER_STATE_OPEN) {
            ui->btnCancelReserve->setEnabled(false);
            ui->btnSelectTable->setEnabled(false);
            ui->btnDateDown->setEnabled(false);
            ui->btnDateUp->setEnabled(false);
            ui->leDate->setEnabled(false);
            ui->timeEdit->setEnabled(false);
            ui->btnTimeDown->setEnabled(false);
            ui->btnTimeUp->setEnabled(false);
            ui->btnCheckin->setEnabled(false);
        }
    }
    return *this;
}

DlgPreorder::~DlgPreorder()
{
    delete ui;
}

void DlgPreorder::on_btnCancel_clicked()
{
    reject();
}

void DlgPreorder::on_btnOK_clicked()
{
    if (fHotelMode) {
    } else {
        if (fOrder->preorder("f_fortable").toInt() == 0) {
            C5Message::error(tr("Select table for reservation"));
            return;
        }
    }

    int state = fOrder->dishesCount() == 0 ? ORDER_STATE_PREORDER_EMPTY : ORDER_STATE_PREORDER_WITH_ORDER;
    if (fOrder->headerValue("f_state").toInt() == ORDER_STATE_OPEN) {
        state = ORDER_STATE_OPEN;
    }
    fOrder->setHeader("f_state", state);
    fOrder->setPreorder("f_datefor", ui->leDate->date());
    fOrder->setPreorder("f_timefor", ui->timeEdit->time());
    fOrder->setPreorder("f_guests", ui->leGuest->value());
    fOrder->setPreorder("f_prepaidcash", ui->leCash->getDouble());
    fOrder->setPreorder("f_prepaidcard", ui->leCard->getDouble());
    fOrder->setPreorder("f_prepaidpayx", ui->lePayX->getDouble());
    fOrder->setPreorder("f_guestname", ui->leGuestName->text());
    fOrder->setPreorder("f_cityledger", ui->leCityLedger->getInteger());
    fOrder->setHeaderHotel("f_guest", fGuestCode);
    fOrder->setHeaderHotel("f_guestcount", ui->leGuest->value());
    fOrder->setHeaderHotel("f_checkin", ui->leDate->date());
    fOrder->setHeaderHotel("f_checkout", ui->leDateCheckout->date());
    fOrder->setHeaderHotel("f_roomrate", ui->leRoomRate->getDouble());
    fOrder->save();
    if (fHotelMode) {
        C5Database db(__c5config.dbParams());
        db[":f_header"] = fOrder->headerValue("f_id");
        db.exec("delete from o_header_hotel_date where f_header=:f_header");
        db[":f_header"] = fOrder->headerValue("f_id");
        db[":f_date"] = ui->leDate->date();
        db[":f_1"] = 0;
        db[":f_2"] = 1;
        db.insert("o_header_hotel_date");
        for (int i = 0, count = ui->leDate->date().daysTo(ui->leDateCheckout->date()); i < count; i++) {
            db[":f_header"] = fOrder->headerValue("f_id");
            db[":f_date"] = ui->leDate->date().addDays(i + 1);
            db[":f_1"] = 1;
            db[":f_2"] = i == count - 1 ? 0 : 1;
            db.insert("o_header_hotel_date");
        }
    }
    done(RESULT_OK);
}

void DlgPreorder::on_btnDatePopup_clicked()
{
    ui->leDate->calendarWidget()->showFullScreen();
}

void DlgPreorder::on_btnTimeUp_clicked()
{
    switch (ui->timeEdit->currentSection()) {
    case QTimeEdit::HourSection:
        ui->timeEdit->setTime(ui->timeEdit->time().addSecs(3600));
        break;
    case QTimeEdit::MinuteSection:
        ui->timeEdit->setTime(ui->timeEdit->time().addSecs(60));
        break;
    }
}

void DlgPreorder::on_btnTimeDown_clicked()
{
    switch (ui->timeEdit->currentSection()) {
    case QTimeEdit::HourSection:
        ui->timeEdit->setTime(ui->timeEdit->time().addSecs(-3600));
        break;
    case QTimeEdit::MinuteSection:
        ui->timeEdit->setTime(ui->timeEdit->time().addSecs(-60));
        break;
    }
}

void DlgPreorder::on_btnDateUp_clicked()
{
    switch (ui->leDate->currentSection()) {
    case QDateEdit::DaySection:
        ui->leDate->setDate(ui->leDate->date().addDays(1));
        break;
    case QDateEdit::MonthSection:
        ui->leDate->setDate(ui->leDate->date().addMonths(1));
        break;
    case QDateEdit::YearSection:
        ui->leDate->setDate(ui->leDate->date().addYears(1));
        break;
    }
}

void DlgPreorder::on_btnDateDown_clicked()
{
    switch (ui->leDate->currentSection()) {
    case QDateEdit::DaySection:
        ui->leDate->setDate(ui->leDate->date().addDays(-1));
        break;
    case QDateEdit::MonthSection:
        ui->leDate->setDate(ui->leDate->date().addMonths(-1));
        break;
    case QDateEdit::YearSection:
        ui->leDate->setDate(ui->leDate->date().addYears(-1));
        break;
    }
}

void DlgPreorder::on_btnGuestUp_clicked()
{
    ui->leGuest->setValue(ui->leGuest->value() + 1);
}

void DlgPreorder::on_btnGuestDown_clicked()
{
    ui->leGuest->setValue(ui->leGuest->value() - 1);
}

void DlgPreorder::on_btnEditCash_clicked()
{
    double max = 999999999;
    if (!DlgPassword::getAmount(tr("Cash mount"), max, false)) {
        return;
    }
    ui->leCash->setText(float_str(max, 2));
}

void DlgPreorder::on_btnEditCard_clicked()
{
    double max = 999999999;
    if (!DlgPassword::getAmount(tr("Card amount"), max, false)) {
        return;
    }
    ui->leCard->setText(float_str(max, 2));
}

void DlgPreorder::on_btnEditPayX_clicked()
{
    double max = 999999999;
    if (!DlgPassword::getAmount(tr("PayX amount"), max, false)) {
        return;
    }
    ui->lePayX->setText(float_str(max, 2));
}

void DlgPreorder::on_btnEditGuestname_clicked()
{
    QString txt;
    if (DlgText::getText(tr("Guest name"), txt)) {
        ui->leGuestName->setText(txt);
        C5Database db(__c5config.dbParams());
        db[":f_contact"] = ui->leGuestName->text();
        fGuestCode = db.insert("c_partners");
    }

}

void DlgPreorder::on_btnEditCLCode_clicked()
{
    QString clCode, clName;
    if (DlgCL::getCL(clCode, clName)) {
        fOrder->setHeader("f_otherid", PAYOTHER_CL);
        fOrder->setCL(clCode, clName);
        fOrder->setPreorder("f_cityledger", clCode.toInt());
        fOrder->setPreorder("f_guestname", clName);
    }
}

void DlgPreorder::on_btnCancelReserve_clicked()
{
    if (C5Message::question(tr("Are you sure to cancel this booking?")) == QDialog::Accepted) {
        done(RESULT_CANCELED);
    }
}

void DlgPreorder::on_btnSelectTable_clicked()
{
    int tableId;
    if (!DlgFace::getTable(tableId, __c5config.defaultHall(), fUser)) {
        return;
    }
    fOrder->setPreorder("f_fortable", tableId);
    ui->leTable->setText(dbtable->name(tableId));
}

void DlgPreorder::on_leDateCheckout_dateChanged(const QDate &date)
{
    ui->leDays->setValue(ui->leDate->date().daysTo(date));
}

void DlgPreorder::on_leDate_dateChanged(const QDate &date)
{
    ui->leDays->setValue(date.daysTo(ui->leDateCheckout->date()));
}

void DlgPreorder::on_leDays_valueChanged(int arg1)
{
    ui->leDateCheckout->setDate(ui->leDate->date().addDays(arg1));
}

void DlgPreorder::on_btnCheckDown_clicked()
{
    ui->leDateCheckout->setDate(ui->leDateCheckout->date().addDays(-1));
}

void DlgPreorder::on_btnCheckoutUp_clicked()
{
    ui->leDateCheckout->setDate(ui->leDateCheckout->date().addDays(1));
}

void DlgPreorder::on_btnEditRate_clicked()
{
    double max = 999999999;
    if (!DlgPassword::getAmount(tr("Card amount"), max, false)) {
        return;
    }
    ui->leRoomRate->setText(float_str(max, 2));
}

void DlgPreorder::on_btnCheckin_clicked()
{
    C5Database db(__c5config.dbParams());
    if (C5Message::question(tr("Confirm to checkin")) == QDialog::Accepted) {
        db[":f_table"] = fOrder->headerValue("f_table");
        db[":f_state"] = ORDER_STATE_OPEN;
        db.exec("select f_id from o_header where f_state=:f_state and f_table=:f_table");
        if (db.nextRow()) {
            C5Message::error(tr("Cannot checkin, an opened order exists"));
            return;
        }
        db[":f_state"] = ORDER_STATE_OPEN;
        db.update("o_header", "f_id", fOrder->headerValue("f_id"));
        C5LogToServerThread::remember(LOG_WAITER, fUser->fullName(),
                                      fOrder->headerValue("f_id").toString(),
                                      fOrder->headerValue("f_id").toString(),
                                      fOrder->headerValue("f_id").toString(),
                                      tr("Checkin"), "", "");
        accept();
    }
}

void DlgPreorder::on_btnPrintPrepaymentTax_clicked()
{

}
