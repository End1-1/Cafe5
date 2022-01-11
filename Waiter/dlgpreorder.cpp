#include "dlgpreorder.h"
#include "ui_dlgpreorder.h"
#include "c5orderdriver.h"
#include "dlgpassword.h"
#include "dlgtext.h"
#include "datadriver.h"
#include "dlgface.h"
#include "dlgcl.h"
#include <QCalendarWidget>

DlgPreorder::DlgPreorder(C5OrderDriver *w, C5User *user, const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::DlgPreorder),
    fOrder(w)
{
    ui->setupUi(this);
    fUser = user;
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
    ui->leDate->setMinimumDate(QDate::currentDate());
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
    if (fOrder->preorder("f_fortable").toInt() == 0) {
        C5Message::error(tr("Select table for reservation"));
        return;
    }
    fOrder->setHeader("f_state", fOrder->dishesCount() == 0 ? ORDER_STATE_PREORDER_EMPTY : ORDER_STATE_PREORDER_WITH_ORDER);
    fOrder->setPreorder("f_datefor", ui->leDate->date());
    fOrder->setPreorder("f_timefor", ui->timeEdit->time());
    fOrder->setPreorder("f_guests", ui->leGuest->value());
    fOrder->setPreorder("f_prepaidcash", ui->leCash->getDouble());
    fOrder->setPreorder("f_prepaidcard", ui->leCard->getDouble());
    fOrder->setPreorder("f_prepaidpayx", ui->lePayX->getDouble());
    fOrder->setPreorder("f_guestname", ui->leGuestName->text());
    fOrder->setPreorder("f_cityledger", ui->leCityLedger->getInteger());
    fOrder->save();
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
