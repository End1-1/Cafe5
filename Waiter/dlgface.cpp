#include "dlgface.h"
#include "dlgmanagertools.h"
#include "jsons.h"
#include "ui_dlgface.h"
#include "dlgpassword.h"
#include "c5user.h"
#include "dlgorder.h"
#include "dlgexitbyversion.h"
#include "dlgpreorderw.h"
#include "c5tabledata.h"
#include "tablewidgetv1.h"
#include "dlgguest.h"
#include "c5utils.h"
#include "datadriver.h"
#include "tablewidget.h"
#include "dlgexitwithmessage.h"
#include "c5socketmessage.h"
#include "fileversion.h"
#include "c5cafecommon.h"
#include "c5logtoserverthread.h"
#include <QPushButton>
#include <QCloseEvent>
#include <QScreen>

#define HALL_COL_WIDTH 175
#define HALL_ROW_HEIGHT 60
#define TIMER_TIMEOUT_INTERVAL 5000

#define view_mode_hall 1
#define view_mode_waiter 2

DlgFace::DlgFace(C5User *user) :
    C5Dialog(C5Config::dbParams()),
    ui(new Ui::DlgFace),
    fUser(user)

{
    ui->setupUi(this);
    if (!C5Config::isAppFullScreen()) {
        setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint);
    } else {
        setWindowState(Qt::WindowFullScreen);
    }
    timeout();
    ui->lbStaff->setText(user->fullName());
    ui->btnCancel->setVisible(false);
    fModeJustSelectTable = false;
    setViewMode(view_mode_hall);
    viewMode(0);
    //ui->wall->setVisible(false);
    ui->lbTime->setText(QTime::currentTime().toString(FORMAT_TIME_TO_SHORT_STR));
    connect( &fTimer, SIGNAL(timeout()), this, SLOT(timeout()));
    fTimer.start(TIMER_TIMEOUT_INTERVAL);
    ui->btnGuests->setVisible(user->check(cp_t5_pay_transfer_to_room));
    ui->btnTools->setVisible(__user->check(cp_t1_login_to_manager));
}

DlgFace::~DlgFace()
{
    delete ui;
}

bool DlgFace::getTable(int &tableId, int hall, C5User *user)
{
    bool result = false;
    DlgFace *df = new DlgFace(user);
    df->ui->btnExit->setVisible(false);
    df->ui->btnCancel->setVisible(true);
    df->ui->btnExit->setVisible(false);
    df->fModeJustSelectTable = true;
    df->fCurrentHall = hall;
    df->filterHall(hall, 0);
    result = df->exec() == QDialog::Accepted;
    if (result) {
        tableId = df->fSelectedTable;
    }
    delete df;
    return result;
}

void DlgFace::initResponse(const QJsonObject &jdoc)
{
    QJsonObject jo = jdoc["data"].toObject();
    QString version = FileVersion::getVersionString(qApp->applicationFilePath());
    if (jo["waiterversion"].toString() != version) {
        fTimer.stop();
        DlgExitByVersion::exit(version, jo["waiterversion"].toString());
    }
    if (jo["nochanges"].toBool() == false) {
        C5TableData::instance()->setData(jdoc["data"].toObject());
    }
    fHttp->httpQueryFinished(sender());
}

void DlgFace::refreshResponse(const QJsonObject &jdoc)
{
    fHallState = jdoc["data"].toObject();
    for (int i = 0; i < ui->sglHall->count(); i++) {
        TableWidget *tw = dynamic_cast<TableWidget *>(ui->sglHall->itemAt(i)->widget());
        if (tw) {
            QJsonObject to;
            if (fHallState.contains(QString::number(tw->fTable))) {
                to = fHallState[QString::number(tw->fTable)].toObject();
            }
            tw->configOrder(to);
        }
    }
    while (QLayoutItem *item = ui->vlStaff->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }
    // for (QMap<int, int>::const_iterator it = dboheader->fStaffTable.constBegin(); it != dboheader->fStaffTable.constEnd();
    //      it++) {
    //     QPushButton *btn = new QPushButton(dbuser->fullShort(it.key()));
    //     btn->setMinimumSize(QSize(150, 50));
    //     connect(btn, &QPushButton::clicked, this, &DlgFace::filterStaffClicked);
    //     btn->setProperty("id", it.key());
    //     ui->vlStaff->addWidget(btn);
    // }
    ui->vlStaff->addStretch(0);
}

void DlgFace::openReservationResponse(const QJsonObject &jdoc)
{
    fHttp->httpQueryFinished(sender());
    DlgPreorder dp(jdoc);
    dp.exec();
    refreshTables();
}

void DlgFace::timeout()
{
    ui->lbTime->setText(QTime::currentTime().toString(FORMAT_TIME_TO_SHORT_STR));
    C5Database db(__c5config.dbParams());
    if (__c5config.getValue(param_date_cash_auto).toInt() == 0) {
        db[":f_key"] = param_date_cash;
        db.exec("select f_value from s_settings_values where f_key=:f_key");
        if (db.nextRow()) {
            if (db.getString("f_value") != __c5config.getValue(param_date_cash)) {
                DlgExitWithMessage::openDialog(tr("Working date was changed, application now will quit")
                                               + "<br>" + db.getString("f_value") + "/" + __c5config.getValue(param_date_cash));
                return;
            }
        }
    }
    refreshTables();
    fHttp->createHttpQuery("/engine/waiter/init.php", QJsonObject{{"version", C5TableData::instance()->version()}}, SLOT(
        initResponse(QJsonObject)), "", false);
}

void DlgFace::hallClicked()
{
    if (fView != view_mode_hall || fView != view_mode_waiter) {
        setViewMode(view_mode_hall);
    }
    QPushButton *btn = static_cast<QPushButton *>(sender());
    filterHall(btn->property("id").toInt(), 0);
}

void DlgFace::tableClicked(int id)
{
    if (fModeJustSelectTable) {
        fSelectedTable = id;
        accept();
        return;
    }
    fTimer.stop();
    if (fHallState.contains(QString::number(id))) {
        const QJsonObject &jt = fHallState[QString::number(id)].toObject();
        if (jt["f_state"].toInt() != ORDER_STATE_OPEN) {
            fHttp->createHttpQuery("/engine/waiter/reservation-open.php",
            QJsonObject{{"id", jt["f_id"].toString()}},
            SLOT(openReservationResponse(QJsonObject)));
            fTimer.start(TIMER_TIMEOUT_INTERVAL);
            return;
        }
    }
    auto *d = DlgOrder::openTable(id, fUser);
    connect(d, &DlgOrder::allDone, [this]() {
        refreshTables();
        fTimer.start(TIMER_TIMEOUT_INTERVAL);
    });
}

void DlgFace::handleCreditCards(const QJsonObject &obj)
{
    sender()->deleteLater();
    if (obj["reply"].toInt() == 0) {
        C5Message::error(obj["msg"].toString());
        return;
    }
    C5CafeCommon::fCreditCards.clear();
    QJsonArray jCC = obj["cards"].toArray();
    for (int i = 0; i < jCC.count(); i++) {
        CreditCards cc;
        QJsonObject o = jCC.at(i).toObject();
        cc.id = o["f_id"].toString().toInt();
        cc.name = o["f_name"].toString();
        QString base64 = o["f_image"].toString();
        if (!base64.isEmpty()) {
            cc.image.loadFromData(QByteArray::fromBase64(base64.toLatin1()), "PNG");
        }
        C5CafeCommon::fCreditCards << cc;
    }
}

void DlgFace::on_btnExit_clicked()
{
    if (fModeJustSelectTable) {
        C5Dialog::reject();
        return;
    }
    accept();
}

void DlgFace::filterHall(int hall, int staff)
{
    fCurrentHall = hall;
    fCurrentStaff = staff;
    while (ui->sglHall->itemAt(0)) {
        ui->sglHall->itemAt(0)->widget()->deleteLater();
        ui->sglHall->removeItem(ui->sglHall->itemAt(0));
    }
    QRect f = qApp->screens().at(0)->geometry();
    int sw = ui->shall->width() - 20;
    int cc = (sw / 204) - 1;
    int delta = sw - ((cc + 1) * 204);
    delta /= cc;
    int minWidth = 200 + delta;
    qDebug() << sw;
    int col = 0;
    int row = 0;
    for (int id : dbtable->list()) {
        QJsonObject to;
        if (fHallState.contains(QString::number(id))) {
            to = fHallState[QString::number(id)].toObject();
        }
        if (hall > 0) {
            if (dbtable->hall(id) != hall) {
                continue;
            }
        }
        if (staff > 0) {
            if (to["f_staff"].toInt() != staff) {
                continue;
            }
        }
        TableWidgetV1 *t = new TableWidgetV1();
        t->setMinimumWidth(minWidth);
        t->setMaximumWidth(minWidth);
        connect(t, &TableWidgetV1::clicked, this, &DlgFace::tableClicked);
        t->config(id);
        t->configOrder(to);
        ui->sglHall->addWidget(t, row, col++, 1, 1);
        if (col > cc) {
            row++;
            col = 0;
        }
    }
    ui->sglHall->setRowStretch(row + 1, 1);
    colorizeHall();
}

void DlgFace::colorizeHall()
{
    for (int i = 0; i < ui->vlHall->count(); i++) {
        QPushButton *btn = dynamic_cast<QPushButton *>(ui->vlHall->itemAt(i)->widget());
        if (btn) {
            if (btn->property("id").toInt() == fCurrentHall) {
                btn->setProperty("stylesheet_button_selected", true);
                btn->style()->polish(btn);
            } else {
                btn->setProperty("stylesheet_button_selected", false);
                btn->style()->polish(btn);
            }
        }
    }
}

void DlgFace::viewMode(int m)
{
    ui->whall->setVisible(m == 0);
}

void DlgFace::setViewMode(int v)
{
    fView = v;
    ui->btnViewHall->setProperty("stylesheet_button_selected", fView == view_mode_hall ?  true : false);
    ui->btnViewHall->style()->polish(ui->btnViewHall);
    ui->btnViewWaiter->setProperty("stylesheet_button_selected", fView == view_mode_waiter ?  true : false);
    ui->btnViewWaiter->style()->polish(ui->btnViewWaiter);
    switch (fView) {
        case view_mode_hall:
            ui->whall->setVisible(true);
            ui->wstaff->setVisible(false);
            break;
        case view_mode_waiter:
            ui->whall->setVisible(true);
            ui->wstaff->setVisible(true);
            break;
    }
}

void DlgFace::refreshTables()
{
    fHttp->createHttpQuery("/engine/waiter/hall-state.php", QJsonObject{{"date", __c5config.dateCash()}}, SLOT(
        refreshResponse(QJsonObject)), QVariant(),
    false);
}

void DlgFace::on_btnCancel_clicked()
{
    on_btnExit_clicked();
}

void DlgFace::on_btnViewHall_clicked()
{
    setViewMode(view_mode_hall);
}

void DlgFace::on_btnViewWaiter_clicked()
{
    setViewMode(view_mode_waiter);
}

void DlgFace::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);
    if (e->spontaneous()) {
        return;
    }
    for (int id : dbhall->list()) {
        QPushButton *btn = new QPushButton(dbhall->name(id));
        btn->setMinimumSize(QSize(140, 60));
        btn->setProperty("id", id);
        connect(btn, &QPushButton::clicked, this, &DlgFace::hallClicked);
        ui->vlHall->addWidget(btn);
    }
    filterHall(__c5config.defaultHall(), 0);
    refreshTables();
    if (!fModeJustSelectTable) {
        C5LogToServerThread::remember(LOG_WAITER, "", "", "", "", "Program started", "", "");
    }
}

void DlgFace::filterStaffClicked()
{
    QPushButton *btn = static_cast<QPushButton *>(sender());
    filterHall(fCurrentHall, btn->property("id").toInt());
}

void DlgFace::on_btnGuests_clicked()
{
    QString res, inv, room, guest;
    DlgGuest::getGuest(res, inv, room, guest);
}

void DlgFace::on_btnTools_clicked()
{
    fTimer.stop();
    DlgManagerTools(__user).exec();
    fTimer.start(TIMER_TIMEOUT_INTERVAL);
}
