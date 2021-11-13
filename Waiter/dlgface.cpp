#include "dlgface.h"
#include "ui_dlgface.h"
#include "c5connection.h"
#include "dlgpassword.h"
#include "c5user.h"
#include "dlgorder.h"
#include "c5menu.h"
#include "c5witerconf.h"
#include "dlgexitbyversion.h"
#include "dlgshiftrotation.h"
#include "dlglistofhall.h"
#include "tablewidgetv1.h"
#include "c5utils.h"
#include "datadriver.h"
#include "dlgexitwithmessage.h"
#include "c5halltabledelegate.h"
#include "fileversion.h"
#include "c5cafecommon.h"
#include "c5logtoserverthread.h"
#include <QPushButton>
#include <QCloseEvent>
#include <QDesktopWidget>

#define HALL_COL_WIDTH 175
#define HALL_ROW_HEIGHT 60
#define TIMER_TIMEOUT_INTERVAL 5000

#define view_mode_hall 1
#define view_mode_waiter 2
#define view_mode_preorder 3

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
    ui->lbStaff->setText(user->fullName());
    ui->btnCancel->setVisible(false);
    ui->btnShowHidePreorders->setVisible(fUser->check(cp_t5_preorder));
    fModeJustSelectTable = false;
    connect(&fTimerCheckVersion, SIGNAL(timeout()), this, SLOT(checkVersionTimeout()));
    setViewMode(view_mode_hall);
    viewMode(0);
    //ui->wall->setVisible(false);
    ui->lbTime->setText(QTime::currentTime().toString(FORMAT_TIME_TO_SHORT_STR));
    for (int id: dbhall->list()) {
        QPushButton *btn = new QPushButton(dbhall->name(id));
        btn->setMinimumSize(QSize(140, 60));
        btn->setProperty("id", id);
        connect(btn, &QPushButton::clicked, this, &DlgFace::hallClicked);
        ui->vlHall->addWidget(btn);
    }
    filterHall(__c5config.defaultHall(), 0);
    refreshTables();
    connect(&fTimer, SIGNAL(timeout()), this, SLOT(timeout()));
    fTimer.start(TIMER_TIMEOUT_INTERVAL);
}

DlgFace::~DlgFace()
{
    delete ui;
}

void DlgFace::setup()
{
//    C5SocketHandler *sh = createSocketHandler(SLOT(handleCreditCards(QJsonObject)));
//    sh->bind("cmd", sm_creditcards);
//    sh->send();
//    sh = createSocketHandler(SLOT(handleDishRemoveReason(QJsonObject)));
//    sh->bind("cmd", sm_dishremovereason);
//    sh->send();
//    sh = createSocketHandler(SLOT(handleDishComment(QJsonObject)));
//    sh->bind("cmd", sm_dishcomment);
//    sh->send();

    if (!fModeJustSelectTable) {
        C5LogToServerThread::remember(LOG_WAITER, "", "", "", "", "Program started", "", "");
    }
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
}

void DlgFace::checkVersionTimeout()
{
    C5SocketHandler *sh = createSocketHandler(SLOT(handleVersion(QJsonObject)));
    sh->bind("cmd", sm_version);
    sh->send();
}

void DlgFace::hallClicked()
{
    if (fView != view_mode_hall || fView != view_mode_waiter) {
        setViewMode(view_mode_hall);
    }
    QPushButton *btn = static_cast<QPushButton*>(sender());
    filterHall(btn->property("id").toInt(), 0);
}

void DlgFace::tableClicked(int id)
{
    if (fModeJustSelectTable) {
        fSelectedTable = id;
        accept();
        return;
    }
    C5User *tmp = fUser;
    if (!tmp->check(cp_t5_edit_table)) {
        if (!DlgPassword::getUserAndCheck(dbtable->name(id), tmp, cp_t5_edit_table)) {
            return;
        }
    }
    fTimer.stop();
    DlgOrder::openTable(id, tmp);
    refreshTables();
    fTimer.start(TIMER_TIMEOUT_INTERVAL);
    if (tmp != fUser) {
        delete tmp;
    }
}

//void DlgFace::handleMenu(const QJsonObject &obj)
//{
//    sender()->deleteLater();
//    C5Menu::fMenu.clear();;
//    C5Menu::fMenuNames.clear();
//    C5Menu::fPart2Color.clear();
//    C5Menu::fDishSpecial.clear();
//    C5Menu::fPackages.clear();
//    C5Menu::fPackagesList.clear();
//    C5Menu::fStopList.clear();
//    sender()->deleteLater();
//    QJsonArray jMenu = obj["menu"].toArray();
//    C5Menu::fMenuVersion = obj["version"].toString();
//    for (int i = 0, count = jMenu.count(); i < count; i++) {
//        QJsonObject o = jMenu.at(i).toObject();
//        C5Menu::fMenu[o["menu_name"].toString()]
//                [o["part1"].toString()]
//                [o["part2"].toString()]
//                .append(o);
//        C5Menu::fPart2Color[o["part2"].toString()] = o["type_color"].toString().toInt();
//    }
//    C5Menu::fPart2Color[""] = -1;
//    for (int i = 0; i < obj["menunames"].toArray().count(); i++) {
//        C5Menu::fMenuNames[obj["menunames"].toArray().at(i).toObject()["f_id"].toString()] = obj["menunames"].toArray().at(i).toObject()["f_name"].toString();
//    }
//    QJsonArray ja = obj["dishspecial"].toArray();
//    for (int i = 0; i < ja.count(); i++) {
//        QJsonObject o = ja.at(i).toObject();
//        C5Menu::fDishSpecial[o["f_dish"].toString()].append(o["f_comment"].toString());
//    }
//    ja = obj["packages"].toArray();
//    for (int i = 0; i < ja.count(); i++) {
//        QJsonObject o = ja.at(i).toObject();
//        C5Menu::fPackages[o["f_name"].toString()] = o;
//    }
//    ja = obj["packageslist"].toArray();
//    for (int i = 0; i < ja.count(); i++) {
//        QJsonObject o = ja.at(i).toObject();
//        C5Menu::fPackagesList[o["f_package"].toString().toInt()].append(o);
//    }
//    ja = obj["stoplist"].toArray();
//    for (int i = 0; i < ja.count(); i++) {
//        QJsonObject o = ja.at(i).toObject();
//        C5Menu::fStopList[o["f_dish"].toString().toInt()] = o["f_qty"].toString().toDouble();
//    }
//    fTimerCheckVersion.start(2000);
//}

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

void DlgFace::handleDishRemoveReason(const QJsonObject &obj)
{
    sender()->deleteLater();
    if (obj["reply"].toInt() == 0) {
        C5Message::error(obj["msg"].toString());
        return;
    }
    C5CafeCommon::fDishRemoveReason.clear();
    QJsonArray jr = obj["reasons"].toArray();
    for (int i = 0; i < jr.count(); i++) {
        C5CafeCommon::fDishRemoveReason << jr.at(i).toObject()["f_name"].toString();
    }
}

void DlgFace::handleDishComment(const QJsonObject &obj)
{
    sender()->deleteLater();
    if (obj["reply"].toInt() == 0) {
        C5Message::error(obj["msg"].toString());
        return;
    }
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
                fTimer.stop();
                DlgExitByVersion::exit(version, o["version"].toString());
            }
        }
        //TODO: UPDATE MENU
//        if (o["app"].toString() == "menu") {
//            if (o["version"].toString() != C5Menu::fMenuVersion) {
//                refreshMenu();
//            }
//        }
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
    QRect scr = qApp->desktop()->screenGeometry();
    int cc = scr.width() > 1024 ? 4 : 3;
    int col = 0;
    int row = 0;
    for (int id: dbtable->list()) {
        if (hall > 0) {
            if (dbtable->hall(id) != hall) {
                continue;
            }
        }
        if (staff > 0) {
            if (dboheader->staff(dboheader->fTableOrder[id]) != staff) {
                continue;
            }
        }
        TableWidgetV1 *t = new TableWidgetV1();
        connect(t, &TableWidgetV1::clicked, this, &DlgFace::tableClicked);
        t->config(id);
        t->configOrder(dboheader->fTableOrder.contains(id) ? dboheader->fTableOrder[id] : "");
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
        QPushButton *btn = dynamic_cast<QPushButton*>(ui->vlHall->itemAt(i)->widget());
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
    ui->tblReserved->setVisible(m == 1);
}

void DlgFace::setViewMode(int v)
{
    fView = v;
    ui->btnViewHall->setProperty("stylesheet_button_selected", fView == view_mode_hall ?  true: false);
    ui->btnViewHall->style()->polish(ui->btnViewHall);
    ui->btnViewWaiter->setProperty("stylesheet_button_selected", fView == view_mode_waiter ?  true: false);
    ui->btnViewWaiter->style()->polish(ui->btnViewWaiter);
    ui->btnShowHidePreorders->setProperty("stylesheet_button_selected", fView == view_mode_preorder ?  true: false);
    ui->btnShowHidePreorders->style()->polish(ui->btnShowHidePreorders);
    switch (fView) {
    case view_mode_hall:
        ui->whall->setVisible(true);
        ui->tblReserved->setVisible(false);
        ui->wstaff->setVisible(false);
        break;
    case view_mode_waiter:
        ui->whall->setVisible(true);
        ui->tblReserved->setVisible(false);
        ui->wstaff->setVisible(true);
        break;
    case view_mode_preorder:
        ui->tblReserved->setColumnWidths(7, 0, 50, 180, 150, 150, 100, 100, 0);
        ui->tblReserved->clearContents();
        ui->tblReserved->setRowCount(0);
        C5Database db(__c5config.dbParams());
        db[":f_state1"] = ORDER_STATE_PREORDER_1;
        db[":f_state2"] = ORDER_STATE_PREORDER_2;
        db.exec("select h.f_id, t.f_name as f_tablename, p.f_datefor, p.f_timefor, p.f_prepaidcash, p.f_prepaidcard, p.f_guests, h.f_state "
                "from o_preorder p "
                "left join o_header h on h.f_id=p.f_id "
                "left join h_tables t on t.f_id=h.f_table "
                "where (h.f_state=:f_state1 or h.f_state=:f_state2) "
                "order by 1, 2");
        ui->tblReserved->setRowCount(db.rowCount());
        int row = 0;
        while (db.nextRow()) {
            for (int i = 0; i < db.columnCount(); i++) {
                ui->tblReserved->setItem(row, i, new QTableWidgetItem());
                ui->tblReserved->setData(row, i, db.getValue(i));
            }
            if (db.getInt("f_state") == ORDER_STATE_PREORDER_1) {
                for (int i = 0; i < ui->tblReserved->columnCount(); i++) {
                    ui->tblReserved->item(row, i)->setData(Qt::BackgroundColorRole, QColor::fromRgb(200, 200, 200));
                }
            }
            row++;
        }
        ui->whall->setVisible(false);
        ui->tblReserved->setVisible(true);
        ui->wstaff->setVisible(false);
        break;
    }
}

void DlgFace::refreshTables()
{
    dboheader->refresh();
    for (int i = 0; i < ui->sglHall->count(); i++) {
        TableWidgetV1 *tw = dynamic_cast<TableWidgetV1*>(ui->sglHall->itemAt(i)->widget());
        if (tw) {
            tw->configOrder(dboheader->fTableOrder.contains(tw->fTable) ? dboheader->fTableOrder[tw->fTable] : "");
        }
    }
    while (ui->vlStaff->itemAt(0)) {
        ui->vlStaff->itemAt(0)->widget()->deleteLater();
        ui->vlStaff->removeItem(ui->vlStaff->itemAt(0));
    }
    for (int staff: dboheader->fStaffTable.keys()) {
        QPushButton *btn = new QPushButton(dbuser->fullShort(staff));
        btn->setMinimumSize(QSize(150, 50));
        connect(btn, &QPushButton::clicked, this, &DlgFace::filterStaffClicked);
        btn->setProperty("id", staff);
        ui->vlStaff->addWidget(btn);
    }
    ui->vlStaff->addStretch(0);
}

void DlgFace::on_btnCancel_clicked()
{
    on_btnExit_clicked();
}

void DlgFace::on_btnShowHidePreorders_clicked()
{
    setViewMode(view_mode_preorder);
}

void DlgFace::on_tblReserved_cellClicked(int row, int column)
{
    Q_UNUSED(column);
    QString orderid = ui->tblReserved->item(row, 0)->data(Qt::DisplayRole).toString();
    if (!fUser->isValid()) {
        if (!DlgPassword::getUser(ui->tblReserved->getString(row, 1), fUser)) {
            return;
        }
    }
    if (!fUser->check(cp_t5_edit_reserve)) {
        C5Message::error(fUser->error());
        return;
    }
    fTimer.stop();
    DlgOrder::openTableById(orderid, fUser);
    fTimer.start(TIMER_TIMEOUT_INTERVAL);
}

void DlgFace::on_btnViewHall_clicked()
{
    setViewMode(view_mode_hall);
}

void DlgFace::on_btnViewWaiter_clicked()
{
    setViewMode(view_mode_waiter);
}

void DlgFace::filterStaffClicked()
{
    QPushButton *btn = static_cast<QPushButton*>(sender());
    filterHall(fCurrentHall, btn->property("id").toInt());
}
