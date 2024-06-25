#include "dlgpreorderw.h"
#include "ui_dlgpreorderw.h"
#include "dlgtext.h"
#include "dlgface.h"
#include "c5tabledata.h"
#include "c5tabledata.h"
#include "axreporting.h"
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
    ui->leDate->setMinimumDate(QDate::fromString(__c5config.getValue(param_date_cash), FORMAT_DATE_TO_STR_MYSQL));
    ui->leDateCheckout->setMinimumDate(QDate::fromString(__c5config.getValue(param_date_cash), FORMAT_DATE_TO_STR_MYSQL));
    ui->leDateCheckout->setDate(ui->leDate->date().addDays(1));
    fPrevRow = -1;
    fPrevColumn = -1;
    setHotelMode(false);
    for (int i = 0; i < ui->tblPayment->columnCount(); i++) {
        ui->tblPayment->setItem(0, i, new QTableWidgetItem());
    }
    ui->tblPayment->setColumnWidth(0, 0);
    ui->tblPayment->setColumnWidth(1, 120);
    setVerticalHeaders();
    if (jdoc["header"].toObject().isEmpty() == false) {
        openDoc();
    } else {
        ui->leTable->setText(tds("h_tables", "f_name", jdoc["f_table"].toInt()));
        ui->leTable->setProperty("id", jdoc["f_table"].toInt());
    }
}

DlgPreorder &DlgPreorder::setHotelMode(bool v)
{
    fHotelMode = v;
    ui->lbCheckout->setVisible(v);
    ui->lbDays->setVisible(v);
    ui->leDateCheckout->setVisible(v);
    ui->leDays->setVisible(v);
    ui->lbRoomRate->setVisible(v);
    ui->leRoomRate->setVisible(v);
    ui->lbPreorder->setText(v ? tr("Reservation") : tr("Preorder"));
    ui->lbTable->setText(v ? tr("Room") : tr("Table"));
    return *this;
}

DlgPreorder::~DlgPreorder()
{
    delete ui;
}

void DlgPreorder::paymentComboboxIndexChanged(int index)
{
    auto *c = static_cast<QComboBox *>(sender());
    int row = c->property("row").toInt();
    if (index > -1) {
        ui->tblPayment->item(row, 1)->setText(c->currentText());
        ui->tblPayment->item(row, 1)->setData(Qt::UserRole, c->currentData());
        if (row == ui->tblPayment->rowCount() - 1) {
            row++;
            ui->tblPayment->setRowCount(ui->tblPayment->rowCount() + 1);
            for (int i = 0; i < ui->tblPayment->columnCount(); i++) {
                ui->tblPayment->setItem(row, i, new QTableWidgetItem());
            }
            setVerticalHeaders();
        }
    }
}

void DlgPreorder::openReservationResponse(const QJsonObject &jdoc)
{
    fHttp->httpQueryFinished(sender());
    AXReporting a(this);
    a.printReservation(jdoc);
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

void DlgPreorder::on_leDateCheckout_dateChanged(const QDate &date)
{
    ui->leDays->setValue(ui->leDate->date().daysTo(date));
}

void DlgPreorder::on_leDate_dateChanged(const QDate &date)
{
    ui->leDays->setValue(date.daysTo(ui->leDateCheckout->date()));
    QJsonObject jhotel = fDoc["hoteldata"].toObject();
    jhotel["f_checkin"] = date.toString(FORMAT_DATE_TO_STR_MYSQL);
    fDoc["hoteldata"] = jhotel;
    setState();
}

void DlgPreorder::on_leDays_valueChanged(int arg1)
{
    ui->leDateCheckout->setDate(ui->leDate->date().addDays(arg1));
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
    ui->leGuest->setValue(jpreorder["f_guests"].toInt());
    ui->leRoomRate->setText(jhotel["f_roomrate"].toString());
    ui->leDate->setDate(QDate::fromString(jhotel["f_checkin"].toString(), "yyyy-MM-dd"));
    ui->leDateCheckout->setDate(QDate::fromString(jhotel["f_checkout"].toString(), "yyyy-MM-dd"));
    ui->timeEdit->setTime(QTime::fromString(jpreorder["f_timefor"].toString()));
    ui->leDays->setValue(ui->leDate->date().daysTo(ui->leDateCheckout->date()));
    ui->cbMealPlan->setCurrentIndex(jhotel["f_mealplan"].toInt() - 1);
    setHotelMode(true);
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
    j["f_guest"] = ui->leGuest->value();
    j["f_guestname"] = ui->leGuestName->text();
    j["f_phone"] = ui->lePhone->text();
    fDoc["preorder"] = j;
    j = QJsonObject();
    j["f_guest"] = ui->leGuestName->property("id").toInt();
    j["f_guestcount"] = ui->leGuest->value();
    j["f_checkin"] = ui->leDate->date().toString(FORMAT_DATE_TO_STR_MYSQL);
    j["f_checkout"] = ui->leDateCheckout->date().toString(FORMAT_DATE_TO_STR_MYSQL);
    j["f_roomrate"] = ui->leRoomRate->getDouble();
    j["f_mealplan"] = ui->cbMealPlan->currentIndex() + 1;
    fDoc["hoteldata"] = j;
    fDoc["checkin"] = withcheckin;
    fHttp->createHttpQuery("/engine/waiter/reservation-save.php", fDoc, SLOT(saveResponse(QJsonObject)));
}

void DlgPreorder::setVerticalHeaders()
{
    QStringList l;
    for (int i = 0; i < ui->tblPayment->rowCount(); i++) {
        l.append(QString::number(i + 1));
    }
    ui->tblPayment->setVerticalHeaderLabels(l);
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

void DlgPreorder::on_tblPayment_cellClicked(int row, int column)
{
    if (fPrevRow > -1) {
        switch (fPrevColumn) {
            case 0:
                break;
            case 1:
                auto *w = ui->tblPayment->cellWidget(fPrevRow, fPrevColumn);
                ui->tblPayment->removeCellWidget(fPrevRow, fPrevColumn);
                if (w) {
                    w->deleteLater();
                }
                break;
        }
    }
    fPrevRow = row;
    fPrevColumn = column;
    if (column == 1) {
        auto *c = new QComboBox();
        c->setFrame(false);
        c->setProperty("row", row);
        c->setEditable(true);
        QMap<QString, int> items = C5TableData::instance()->cashNames();
        for(QMap<QString, int>::const_iterator it = items.constBegin(); it != items.constEnd(); it++) {
            c->addItem(it.key(), it.value());
        }
        auto *cm = c->completer();
        //cm->setCompletionMode(QCompleter::PopupCompletion);
        connect(c, SIGNAL(currentIndexChanged(int)), this, SLOT(paymentComboboxIndexChanged(int)));
        int id = ui->tblPayment->item(row, 1)->data(Qt::UserRole).toInt();
        c->setCurrentIndex(c->findData(id));
        ui->tblPayment->setCellWidget(row, column, c);
        c->setFocus();
    }
}
