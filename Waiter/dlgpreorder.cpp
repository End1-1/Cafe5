#include "dlgpreorder.h"
#include "ui_dlgpreorder.h"
#include "c5waiterorderdoc.h"
#include "dlgpassword.h"
#include <QCalendarWidget>

DlgPreorder::DlgPreorder(C5WaiterOrderDoc *w, const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::DlgPreorder),
    fOrder(w)
{
    ui->setupUi(this);
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
    fOrder->hSetInt("preorder", 1);
    fOrder->hSetString("date", ui->leDate->date().toString(FORMAT_DATETIME_TO_STR));
    fOrder->hSetString("time", ui->timeEdit->time().toString(FORMAT_TIME_TO_STR));
    fOrder->hSetInt("guest", ui->leGuest->value());
    fOrder->hSetDouble("cash", ui->leCash->getDouble());
    fOrder->hSetDouble("card", ui->leCard->getDouble());
    accept();
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
    if (!DlgPassword::getAmount(tr("Card amount"), max, true)) {
        return;
    }
    ui->leCard->setText(float_str(max, 2));
}
