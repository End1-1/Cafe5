#include "dlgface.h"
#include "ui_dlgface.h"
#include "c5connection.h"
#include "c5serverhandler.h"
#include "dlgpassword.h"
#include "c5user.h"
#include "dlgorder.h"
#include "c5menu.h"
#include "c5witerconf.h"
#include "dlgexitbyversion.h"
#include "c5waiterserver.h"
#include "dlgshiftrotation.h"
#include "dlglistofhall.h"
#include "c5utils.h"
#include "dlgreports.h"
#include "dlgexitwithmessage.h"
#include "c5halltabledelegate.h"
#include "fileversion.h"
#include "c5cafecommon.h"
#include "c5logtoserverthread.h"
#include <QTcpSocket>
#include <QCloseEvent>

#define HALL_COL_WIDTH 175
#define HALL_ROW_HEIGHT 60
#define TIMER_TIMEOUT_INTERVAL 5000

DlgFace::DlgFace() :
    C5Dialog(C5Config::dbParams()),
    ui(new Ui::DlgFace)
{
    ui->setupUi(this);
    fCanClose = false;
    ui->btnCancel->setVisible(false);
    fModeJustSelectTable = false;
    connect(&fTimerCheckVersion, SIGNAL(timeout()), this, SLOT(checkVersionTimeout()));
    fTimerCheckVersion.start(60000);
}

DlgFace::~DlgFace()
{
    delete ui;
}

void DlgFace::setup()
{
    ui->tblHall->setItemDelegate(new C5HallTableDelegate());
    ui->tblHall->horizontalHeader()->setDefaultSectionSize(HALL_COL_WIDTH);
    ui->tblHall->setColumnCount(ui->tblHall->width() / HALL_COL_WIDTH);
    if (ui->tblHall->columnCount() == 0) {
        ui->tblHall->setColumnCount(1);
    }
    int delta = ui->tblHall->width() - (ui->tblHall->columnCount() * HALL_COL_WIDTH) - 5;
    delta /= ui->tblHall->columnCount();
    ui->tblHall->horizontalHeader()->setDefaultSectionSize(HALL_COL_WIDTH + delta);
    ui->tblHall->verticalHeader()->setDefaultSectionSize(HALL_ROW_HEIGHT);
    connect(&fTcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
    fTcpServer.listen(QHostAddress::Any, 1000);
    C5SocketHandler *sh = createSocketHandler(SLOT(handleHall(QJsonObject)));
    sh->bind("cmd", sm_hall);
    sh->bind("hall", C5Config::hallList());
    sh->send();
    sh = createSocketHandler(SLOT(handleMenu(QJsonObject)));
    sh->bind("cmd", sm_menu);
    sh->send();
    sh = createSocketHandler(SLOT(handleConf(QJsonObject)));
    sh->bind("cmd", sm_waiterconf);
    sh->bind("station", hostinfo);
    sh->send();
    sh = createSocketHandler(SLOT(handleCreditCards(QJsonObject)));
    sh->bind("cmd", sm_creditcards);
    sh->send();
    sh = createSocketHandler(SLOT(handleDishRemoveReason(QJsonObject)));
    sh->bind("cmd", sm_dishremovereason);
    sh->send();
    sh = createSocketHandler(SLOT(handleDishComment(QJsonObject)));
    sh->bind("cmd", sm_dishcomment);
    sh->send();
    connect(&fTimer, SIGNAL(timeout()), this, SLOT(timeout()));
    fTimer.start(TIMER_TIMEOUT_INTERVAL);
    connect(&fTimer, SIGNAL(timeout()), this, SLOT(confTimeout()));
    fConfTimer.start(TIMER_TIMEOUT_INTERVAL * 12);
    if (!fModeJustSelectTable) {
        C5LogToServerThread::remember(LOG_WAITER, "", "", "", "", "Program started", "", "");
    }
}

void DlgFace::accept()
{
    if (fCanClose) {
        C5Dialog::accept();
    }
}

void DlgFace::reject()
{
    on_btnExit_clicked();
}

bool DlgFace::getTable(int &tableId, const QString &hall)
{
    bool result = false;
    DlgFace *df = new DlgFace();
    df->ui->lbDate->setVisible(false);
    df->ui->btnConnection->setVisible(false);
    df->ui->btnReports->setVisible(false);
    df->ui->btnExit->setVisible(false);
    df->ui->btnCancel->setVisible(true);
    df->fModeJustSelectTable = true;
    df->showFullScreen();
    df->hide();
    df->setup();
    df->fCurrentHall = hall;
    df->filterHall(hall);
    result = df->exec() == QDialog::Accepted;
    if (result) {
        tableId = df->fSelectedTable["f_id"].toString().toInt();
    }
    delete df;
    return result;
}

void DlgFace::timeout()
{
    C5SocketHandler *sh = createSocketHandler(SLOT(handleHall(QJsonObject)));
    sh->bind("hall", C5Config::hallList());
    sh->bind("cmd", sm_hall);
    sh->bind("hall", C5Config::hallList());
    sh->send();
    ui->lbDate->setText(QString("%1 %2").arg(QDate::fromString(__c5config.dateCash(), FORMAT_DATE_TO_STR_MYSQL).toString(FORMAT_DATE_TO_STR)).arg(QTime::currentTime().toString("HH:mm")));
}

void DlgFace::checkVersionTimeout()
{
    C5SocketHandler *sh = createSocketHandler(SLOT(handleVersion(QJsonObject)));
    sh->bind("cmd", sm_version);
    sh->send();
}

void DlgFace::confTimeout()
{
    C5SocketHandler *sh = createSocketHandler(SLOT(handleConf(QJsonObject)));
    sh->bind("cmd", sm_waiterconf);
    sh->bind("station", hostinfo);
    sh->send();
}

void DlgFace::newConnection()
{
    QTcpSocket *s = fTcpServer.nextPendingConnection();
    C5SocketHandler *sh = new C5SocketHandler(s, this);
    connect(sh, SIGNAL(handleCommand(QJsonObject)), this, SLOT(handleSocket(QJsonObject)));
}

void DlgFace::handleHall(const QJsonObject &obj)
{
    sender()->deleteLater();
    C5CafeCommon::fHalls = obj["halls"].toArray();
    C5CafeCommon::fTables = obj["tables"].toArray();
    C5CafeCommon::fShifts = obj["shifts"].toArray();
    if (fCurrentHall.isEmpty()) {
        fCurrentHall = C5Config::defaultHall();
    }
    filterHall(fCurrentHall);
}

void DlgFace::handleMenu(const QJsonObject &obj)
{
    sender()->deleteLater();
    C5Menu::fMenu.clear();;
    C5Menu::fMenuNames.clear();
    C5Menu::fPart2Color.clear();
    C5Menu::fDishSpecial.clear();
    C5Menu::fPackages.clear();
    C5Menu::fPackagesList.clear();
    sender()->deleteLater();
    QJsonArray jMenu = obj["menu"].toArray();
    C5Menu::fMenuVersion = obj["version"].toString();
    for (int i = 0, count = jMenu.count(); i < count; i++) {
        QJsonObject o = jMenu.at(i).toObject();
        C5Menu::fMenu[o["menu_name"].toString()]
                [o["part1"].toString()]
                [o["part2"].toString()]
                .append(o);
        C5Menu::fPart2Color[o["part2"].toString()] = o["type_color"].toString().toInt();
    }
    C5Menu::fPart2Color[""] = -1;
    for (int i = 0; i < obj["menunames"].toArray().count(); i++) {
        C5Menu::fMenuNames[obj["menunames"].toArray().at(i).toObject()["f_id"].toString()] = obj["menunames"].toArray().at(i).toObject()["f_name"].toString();
    }
    QJsonArray ja = obj["dishspecial"].toArray();
    for (int i = 0; i < ja.count(); i++) {
        QJsonObject o = ja.at(i).toObject();
        C5Menu::fDishSpecial[o["f_dish"].toString()].append(o["f_comment"].toString());
    }
    ja = obj["packages"].toArray();
    for (int i = 0; i < ja.count(); i++) {
        QJsonObject o = ja.at(i).toObject();
        C5Menu::fPackages[o["f_name"].toString()] = o;
    }
    ja = obj["packageslist"].toArray();
    for (int i = 0; i < ja.count(); i++) {
        QJsonObject o = ja.at(i).toObject();
        C5Menu::fPackagesList[o["f_package"].toString().toInt()].append(o);
    }
}

void DlgFace::handleConf(const QJsonObject &obj)
{
    sender()->deleteLater();
    if (obj["reply"].toInt() == 0) {
        return;
    }
    if (!__c5config.autoDateCash() && !__c5config.dateCash().isEmpty()) {
        if (__c5config.dateCash() != obj["date_cash"].toString() || __c5config.dateShift() != obj["date_shift"].toString().toInt()) {
            DlgExitWithMessage::openDialog(tr("Working date was changed, application now will quit"));
            return;
        }
    }
    __c5config.setValue(param_date_cash_auto, obj["date_auto"].toString());
    __c5config.setValue(param_date_cash, obj["date_cash"].toString());
    __c5config.setValue(param_date_cash_shift, obj["date_shift"].toString());
    QStringList keys = obj["conf"].toObject().keys();
    foreach (QString k, keys) {
        __c5config.setValue(k.toInt(), obj["conf"].toObject()[k].toString());
    }
    QJsonArray ja = obj["otherconf"].toArray();
    C5CafeCommon::fHallConfigs.clear();
    for (int i = 0; i < ja.count(); i++) {
        C5CafeCommon::fHallConfigs[ja.at(i).toObject()["f_settings"].toString().toInt()][ja.at(i).toObject()["f_key"].toString().toInt()] = ja.at(i).toObject()["f_value"].toString();
    }
}

void DlgFace::handleCreditCards(const QJsonObject &obj)
{
    sender()->deleteLater();
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

void DlgFace::handleDishRemoveReason(const QJsonObject &obj)
{
    sender()->deleteLater();
    C5CafeCommon::fDishRemoveReason.clear();
    QJsonArray jr = obj["reasons"].toArray();
    for (int i = 0; i < jr.count(); i++) {
        C5CafeCommon::fDishRemoveReason << jr.at(i).toObject()["f_name"].toString();
    }
}

void DlgFace::handleDishComment(const QJsonObject &obj)
{
    sender()->deleteLater();
    C5CafeCommon::fDishComments.clear();
    QJsonArray ja = obj["comments"].toArray();
    for (int i = 0; i < ja.count(); i++) {
        C5CafeCommon::fDishComments << ja.at(i).toObject()["f_name"].toString();
    }
}

void DlgFace::handleVersion(const QJsonObject &obj)
{
    if (obj["reply"].toInt() != 0) {
        C5Message::error(obj["msg"].toString());
        return;
    }
    QJsonArray arr = obj["versions"].toArray();
    for (int i = 0; i < arr.count(); i++) {
        QJsonObject o = arr.at(i).toObject();
        if (o["app"].toString() == _MODULE_) {
            QString version = FileVersion::getVersionString(qApp->applicationFilePath());
            if (o["version"].toString() != version) {
                DlgExitByVersion::exit(version, o["version"].toString());
            }
        }
        if (o["app"].toString() == "menu") {
            if (o["version"].toString() != C5Menu::fMenuVersion) {
                DlgExitByVersion::exit(tr("Menu was updated"));
            }
        }
    }
}

void DlgFace::handleSocket(const QJsonObject &obj)
{
    QJsonObject no(obj);
    C5SocketHandler *sh = static_cast<C5SocketHandler*>(sender());
    C5WaiterServer ws(no, sh->fSocket);
    QJsonObject o;
    ws.reply(o);
    o["cmd"] = ws.cmd();
    sh->send(o);
    sh->close();
    sh->deleteLater();
}

void DlgFace::on_btnConnection_clicked()
{
    go<C5Connection>(C5Config::dbParams());
}

void DlgFace::on_btnExit_clicked()
{
    if (fModeJustSelectTable) {
        C5Dialog::reject();
        return;
    }
    if (C5Message::question(tr("Are you sure to close application")) == QDialog::Accepted) {
        fCanClose = true;
        qApp->quit();
    }
}

void DlgFace::filterHall(const QString &hall)
{
    ui->tblHall->clearContents();
    ui->tblHall->setRowCount(0);
    int c = 0, r = 0;
    for (int i = 0; i < C5CafeCommon::fTables.count(); i++)  {
        if (hall != "*" && !hall.isEmpty()) {
            if (C5CafeCommon::fTables.at(i).toObject()["f_hall"].toString() != hall) {
                continue;
            }
        }
        if (c == ui->tblHall->columnCount()) {
            c = 0;
            r++;
        }
        if (r > ui->tblHall->rowCount() - 1) {
            ui->tblHall->setRowCount(r + 1);
            for (int j = 0; j < ui->tblHall->columnCount(); j++) {
                ui->tblHall->setItem(r, j, new QTableWidgetItem());
            }
        }
        QTableWidgetItem *item = ui->tblHall->item(r, c++);
        item->setData(Qt::UserRole, C5CafeCommon::fTables.at(i).toObject());
    }
    ui->tblHall->setRowCount(r + 1);
}

void DlgFace::on_tblHall_itemClicked(QTableWidgetItem *item)
{
    if (!item) {
        return;
    }
    QJsonObject o = item->data(Qt::UserRole).toJsonObject();
    if (o["f_id"].toString().toInt() == 0) {
        return;
    }
    if (fModeJustSelectTable) {
        fSelectedTable = o;
        fCanClose = true;
        accept();
        return;
    }
    C5User user;
    if (!DlgPassword::getUser(o["f_name"].toString(), &user)) {
        return;
    }
    fTimer.stop();
    DlgOrder::openTable(o, &user);
    C5SocketHandler *sh = createSocketHandler(SLOT(handleHall(QJsonObject)));
    sh->bind("cmd", sm_hall);
    sh->bind("hall", C5Config::hallList());
    sh->send();
    fTimer.start(TIMER_TIMEOUT_INTERVAL);
}

void DlgFace::on_btnHallFilter_clicked()
{
    QString hall;
    if (!DlgListOfHall::getHall(hall)) {
        return;
    }
    fCurrentHall = hall;
    filterHall(hall);
}

void DlgFace::on_btnReports_clicked()
{
    C5User user;
    if (!DlgPassword::getUser(tr("Reports"), &user)) {
        return;
    }
    DlgReports::openReports(&user);
}

void DlgFace::on_btnCancel_clicked()
{
    on_btnExit_clicked();
}

void DlgFace::on_btnClearDroid_clicked()
{
    if (C5Message::question(tr("Are you sure to clear droids?")) != QDialog::Accepted) {
        return;
    }
    C5Database db(C5Config::dbParams());
    db[":msg"] = 1;
    db.insert("droid_message", false);
}

void DlgFace::on_btnSetSession_clicked()
{
    C5User user;
    if (!DlgPassword::getUser(tr("Shift rotation"), &user)) {
        return;
    }
    if (user.check(cp_t5_shift_rotation)) {
        DlgShiftRotation *d = new DlgShiftRotation(fDBParams);
        d->exec();
        delete d;
    }
}
