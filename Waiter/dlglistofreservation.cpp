#include "dlglistofreservation.h"
#include "dlgorder.h"
#include "dlgpreorderw.h"
#include "ui_dlglistofreservation.h"

#define mode_reservation_list 1
#define mode_history 2
#define mode_forecast 3

DlgListOfReservation::DlgListOfReservation(QWidget *parent)
    : C5Dialog(__c5config.dbParams())
    , ui(new Ui::DlgListOfReservation)
{
    ui->setupUi(this);
    if (!C5Config::isAppFullScreen()) {
        setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint);
    } else {
        setWindowState(Qt::WindowFullScreen);
    }
    ui->leDate1->setDate(QDate::fromString(__c5config.dateCash(), FORMAT_DATE_TO_STR_MYSQL));
    ui->leDate2->setDate(ui->leDate1->date());
    fMode = mode_reservation_list;
    on_btnRefreshReservations_clicked();
}

DlgListOfReservation::~DlgListOfReservation()
{
    delete ui;
}

void DlgListOfReservation::openReservationResponse(const QJsonObject &jdoc)
{
    fHttp->httpQueryFinished(sender());
    DlgPreorder d(jdoc);
    d.setHotelMode(true);
    d.exec();
}

void DlgListOfReservation::listResponse(const QJsonObject &jdoc)
{
    QJsonArray ja = jdoc["out"].toArray();
    ui->tbl->setRowCount(ja.size());
    QStringList h;
    h << tr("UUID") << tr("Table") << tr("State") << tr("Checkin") << tr("Checkout") << tr("Guest") << tr("Guest count")
      << tr("Phone");
    ui->tbl->setColumnCount(h.count());
    ui->tbl->setHorizontalHeaderLabels(h);
    for (int i = 0; i < ja.size(); i++) {
        const QJsonObject &j = ja.at(i).toObject();
        ui->tbl->setItem(i, 0, new QTableWidgetItem(j["f_id"].toString()));
        ui->tbl->item(i, 0)->setData(Qt::UserRole, j["f_state"].toInt());
        ui->tbl->item(i, 0)->setData(Qt::UserRole + 1, j["f_table"].toInt());
        ui->tbl->setItem(i, 1, new QTableWidgetItem(j["f_tableforname"].toString()));
        ui->tbl->setItem(i, 2, new QTableWidgetItem(j["f_statename"].toString()));
        ui->tbl->setItem(i, 3, new QTableWidgetItem(QDate::fromString(j["f_checkin"].toString(),
                         FORMAT_DATE_TO_STR_MYSQL).toString(FORMAT_DATE_TO_STR)));
        ui->tbl->setItem(i, 4, new QTableWidgetItem(QDate::fromString(j["f_checkout"].toString(),
                         FORMAT_DATE_TO_STR_MYSQL).toString(FORMAT_DATE_TO_STR)));
        ui->tbl->setItem(i, 5, new QTableWidgetItem(j["f_guestname"].toString()));
        ui->tbl->setItem(i, 6, new QTableWidgetItem(QString::number(j["f_guestcount"].toInt())));
        ui->tbl->setItem(i, 7, new QTableWidgetItem(j["f_phone"].toString()));
    }
    ui->tbl->resizeColumnsToContents();
    ui->tbl->setColumnWidth(0, 0);
    fMode = mode_reservation_list;
    fHttp->httpQueryFinished(sender());
}

void DlgListOfReservation::responseHistory(const QJsonObject &jdoc)
{
    qDebug() << jdoc;
    QJsonObject jout = jdoc["out"].toObject();
    QStringList keys = jout.keys();
    ui->tbl->setRowCount(2);
    ui->tbl->clearContents();
    QStringList h = keys;
    h.append(tr("Total"));
    double total = 0;
    int guests = 0;
    ui->tbl->setColumnCount(h.length());
    ui->tbl->setHorizontalHeaderLabels(h);
    for (int i = 0; i < keys.length(); i++) {
        const QJsonObject &j = jout[keys.at(i)].toObject();
        ui->tbl->setItem(0, i, new QTableWidgetItem(float_str(j["revenue"].toDouble(), 1)));
        ui->tbl->setItem(1, i, new QTableWidgetItem(QString::number(j["guests"].toInt()), 1));
        total += j["revenue"].toDouble();
        guests += j["guests"].toInt();
    }
    ui->tbl->setItem(0, ui->tbl->columnCount() - 1, new QTableWidgetItem(float_str(total, 1)));
    ui->tbl->setItem(1, ui->tbl->columnCount() - 1, new QTableWidgetItem(QString::number(guests)));
    ui->tbl->resizeColumnsToContents();
    fHttp->httpQueryFinished(sender());
}

void DlgListOfReservation::responseForecast(const QJsonObject &jdoc)
{
    QJsonArray jout = jdoc["out"].toArray();
    ui->tbl->setRowCount(2);
    ui->tbl->clearContents();
    QStringList h;
    for (int i = 0; i < jout.size(); i++) {
        const QJsonObject &j = jout.at(i).toObject();
        h.append(j["dt"].toString());
    }
    h.append(tr("Total"));
    ui->tbl->setColumnCount(h.size());
    ui->tbl->setHorizontalHeaderLabels(h);
    double total = 0;
    int guests = 0;
    for (int i = 0; i < jout.size(); i++) {
        const QJsonObject &j = jout.at(i).toObject();
        ui->tbl->setItem(0, i, new QTableWidgetItem(float_str(j["total"].toDouble(), 1)));
        ui->tbl->setItem(1, i, new QTableWidgetItem(QString::number(j["guests"].toInt())));
        total += j["total"].toDouble();
        guests += j["guests"].toInt();
    }
    ui->tbl->setColumnCount(h.length());
    ui->tbl->setHorizontalHeaderLabels(h);
    ui->tbl->setItem(0, ui->tbl->columnCount() - 1, new QTableWidgetItem(float_str(total, 1)));
    ui->tbl->setItem(1, ui->tbl->columnCount() - 1, new QTableWidgetItem(QString::number(guests)));
    ui->tbl->resizeColumnsToContents();
    fHttp->httpQueryFinished(sender());
}

void DlgListOfReservation::on_btnRefreshReservations_clicked()
{
    switch (fMode) {
        case mode_reservation_list:
            on_btnAllReservation_clicked();
            break;
        case mode_history:
            on_btnHistory_clicked();
            break;
        case mode_forecast:
            on_btnForecast_clicked();
            break;
    }
}

void DlgListOfReservation::on_btnExit_clicked()
{
    reject();
}

void DlgListOfReservation::on_tbl_cellDoubleClicked(int row, int column)
{
    switch (fMode) {
        case mode_reservation_list:  {
            int state = ui->tbl->item(row, 0)->data(Qt::UserRole).toInt();
            switch (state) {
                case 1: {
                    DlgOrder *d = new DlgOrder(__user);
                    d->openTable(ui->tbl->item(row, 0)->data(Qt::UserRole + 1).toInt(), __user);
                    break;
                }
                case 5:
                case 6: {
                    fHttp->createHttpQuery("/engine/waiter/reservation-open.php", QJsonObject{{"id", ui->tbl->item(row, 0)->text()}}, SLOT(
                        openReservationResponse(
                            QJsonObject)));
                    break;
                }
            }
            break;
        }
    }
}

void DlgListOfReservation::on_btnAllReservation_clicked()
{
    fMode = mode_reservation_list;
    fHttp->createHttpQuery("/engine/waiter/reservation-list.php", QJsonObject{{"id", 0}}, SLOT(listResponse(QJsonObject)));
}

void DlgListOfReservation::on_btnHistory_clicked()
{
    fMode = mode_history;
    fHttp->createHttpQuery("/engine/waiter/reservation-history.php",
    QJsonObject{{"d1", ui->leDate1->date().toString(FORMAT_DATE_TO_STR_MYSQL)},
        {"d2", ui->leDate2->date().toString(FORMAT_DATE_TO_STR_MYSQL)},
        {"groupping", ui->cbGroupping->currentIndex()}},
    SLOT(responseHistory(QJsonObject)));
}

void DlgListOfReservation::on_btnForecast_clicked()
{
    fMode = mode_forecast;
    fHttp->createHttpQuery("/engine/waiter/reservation-forecast.php",
    QJsonObject{{"d1", ui->leDate1->date().toString(FORMAT_DATE_TO_STR_MYSQL)},
        {"d2", ui->leDate2->date().toString(FORMAT_DATE_TO_STR_MYSQL)},
        {"groupping", ui->cbGroupping->currentIndex()}}, SLOT(responseForecast(QJsonObject)));
}
