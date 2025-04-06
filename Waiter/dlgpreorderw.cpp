#include "dlgpreorderw.h"
#include "ui_dlgpreorderw.h"
#include "dlgtext.h"
#include "dlgface.h"
#include "c5tabledata.h"
#include "c5config.h"
#include "c5database.h"
#include "c5utils.h"
#include "c5message.h"
#include "c5tabledata.h"
#include "axreporting.h"
#include "dlgreceiptlanguage.h"
#include <QClipboard>
#include <QCalendarWidget>
#include <QDateEdit>
#include <QCompleter>
#include <QFile>
#include <QDir>

DlgPreorder::DlgPreorder(const QJsonObject &jdoc) :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::DlgPreorder),
    fDoc(jdoc)
{
    ui->setupUi(this);
    ui->leDate->setMinimumDate(QDate::currentDate());
    ui->leDate->setDate(QDate::fromString(__c5config.getValue(param_date_cash), FORMAT_DATE_TO_STR_MYSQL));
    if (jdoc["header"].toObject().isEmpty() == false) {
        openDoc();
    } else {
        ui->leTable->setText(tds("h_tables", "f_name", jdoc["f_table"].toInt()));
        ui->leTable->setProperty("id", jdoc["f_table"].toInt());
    }
}

DlgPreorder::~DlgPreorder()
{
    delete ui;
}

void DlgPreorder::openReservationResponse(const QJsonObject &jdoc)
{
    fHttp->httpQueryFinished(sender());
    AXReporting a(this);
    int r = DlgReceiptLanguage::receipLanguage();
    if (r < 0) {
        return;
    }
    a.printReservation(jdoc, r);
}

void DlgPreorder::checkinResponse(const QJsonObject &jdoc)
{
    fHttp->httpQueryFinished(sender());
    accept();
}

void DlgPreorder::cancelResponse(const QJsonObject &jdoc)
{
    fHttp->httpQueryFinished(sender());
    C5Message::info(tr("Canceled"));
    setProperty("canceled", true);
    accept();
}

void DlgPreorder::saveResponse(const QJsonObject &jdoc)
{
    fDoc = jdoc;
    fHttp->httpQueryFinished(sender());
    C5Message::info(tr("Saved"));
    if (fDoc["withcheckin"].toBool()) {
        accept();
    } else {
        setState();
    }
}

void DlgPreorder::on_btnCancel_clicked()
{
    reject();
}

void DlgPreorder::on_btnOK_clicked()
{
    if (ui->leTable->property("id").toInt() == 0) {
        C5Message::error(tr("Select table for reservation"));
        return;
    }
    save(false);
}

void DlgPreorder::on_btnEditGuestname_clicked()
{
    QString txt;
    if (DlgText::getText(tr("Guest name"), txt)) {
        ui->leGuestName->setText(txt);
        C5Database db(__c5config.dbParams());
        db[":f_contact"] = ui->leGuestName->text();
    }
}

void DlgPreorder::on_btnCancelReserve_clicked()
{
    if (C5Message::question(tr("Are you sure to cancel this booking?")) == QDialog::Accepted) {
        fHttp->createHttpQuery("/engine/waiter/reservation-cancel.php",
        QJsonObject{{"id", fDoc["header"].toObject()["f_id"].toString()}}, SLOT(
            cancelResponse(QJsonObject)));
    }
}

void DlgPreorder::on_btnSelectTable_clicked()
{
    int tableId;
    if (!DlgFace::getTable(tableId, __c5config.defaultHall(), __user)) {
        return;
    }
    ui->leTable->setProperty("id", tableId);
    ui->leTable->setText(tds("h_tables", "f_name", tableId));
}

void DlgPreorder::on_btnCheckin_clicked()
{
    save(true);
}

void DlgPreorder::on_btnPrintReservation_clicked()
{
    fHttp->createHttpQuery("/engine/waiter/reservation-open.php",
    QJsonObject{{"id", fDoc["header"].toObject()["f_id"].toString()}},
    SLOT(openReservationResponse(QJsonObject)));
}

void DlgPreorder::openDoc()
{
    QJsonObject jheader = fDoc["header"].toObject();
    QJsonObject jpreorder = fDoc["preorder"].toObject();
    QJsonObject jhotel = fDoc["hoteldata"].toObject();
    ui->leTable->setProperty("id", jheader["f_table"].toInt());
    ui->leTable->setText(tds("h_tables", "f_name", jheader["f_table"].toInt()));
    ui->lePhone->setText(jpreorder["f_phone"].toString());
    ui->leGuestName->setText(jpreorder["f_guestname"].toString());
    ui->leGuest->setValue(jhotel["f_guestcount"].toInt());
    ui->leRoomRate->setDouble(jhotel["f_roomrate"].toDouble());
    ui->leDate->setDate(QDate::fromString(jhotel["f_checkin"].toString(), "yyyy-MM-dd"));
    ui->timeEdit->setTime(QTime::fromString(jpreorder["f_timefor"].toString()));
    ui->leEmail->setText(jpreorder["f_email"].toString());
}

void DlgPreorder::save(bool withcheckin)
{
    QJsonObject j = fDoc["header"].toObject();
    if (j.isEmpty()) {
        fDoc["id"] = "";
        j["f_state"] = ORDER_STATE_PREORDER_EMPTY;
        j["f_hall"] = fDoc["f_hall"];
    } else {
        fDoc["id"] = j["f_id"].toString();
    }
    j["f_table"] = ui->leTable->property("id").toInt();
    j["f_guests"] = ui->leGuest->value();
    fDoc["header"] = j;
    j = QJsonObject();
    j["f_datefor"] = ui->leDate->date().toString(FORMAT_DATE_TO_STR_MYSQL);
    j["f_timefor"] = ui->timeEdit->time().toString(FORMAT_TIME_TO_STR);
    j["f_fortable"] = ui->leTable->property("id").toInt();
    j["f_guests"] = ui->leGuest->value();
    j["f_guestname"] = ui->leGuestName->text();
    j["f_phone"] = ui->lePhone->text();
    j["f_email"] = ui->leEmail->text();
    fDoc["preorder"] = j;
    j = QJsonObject();
    j["f_guest"] = ui->leGuestName->property("id").toInt();
    j["f_guestcount"] = ui->leGuest->value();
    j["f_checkin"] = ui->leDate->date().toString(FORMAT_DATE_TO_STR_MYSQL);
    fDoc["hoteldata"] = j;
    fDoc["checkin"] = withcheckin;
    fHttp->createHttpQuery("/engine/waiter/reservation-save.php", fDoc, SLOT(saveResponse(QJsonObject)));
}

void DlgPreorder::setState()
{
    const QJsonObject &jheader = fDoc["header"].toObject();
    const QJsonObject &jhotel = fDoc["hoteldata"].toObject();
    ui->btnCheckin->setEnabled(jheader["f_state"].toInt() == ORDER_STATE_PREORDER_EMPTY
                               || jheader["f_state"].toInt() == ORDER_STATE_PREORDER_WITH_ORDER);
    QDate checkin = QDate::fromString(jhotel["f_checkin"].toString(), FORMAT_DATE_TO_STR_MYSQL);
    QDate workingday = QDate::fromString(__c5config.dateCash(), FORMAT_DATE_TO_STR_MYSQL);
    ui->btnCheckin->setEnabled(ui->btnCheckin->isEnabled() && (checkin == workingday));
}

void DlgPreorder::on_btnCopyID_clicked()
{
    qApp->clipboard()->setText(fDoc["header"].toObject()["f_id"].toString());
}
