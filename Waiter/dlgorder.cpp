#include "dlgorder.h"
#include "ui_dlgorder.h"
#include "c5cafecommon.h"
#include "c5user.h"
#include "c5witerconf.h"
#include "c5menu.h"
#include "dlgdishremovereason.h"
#include "c5dishtabledelegate.h"
#include "c5ordertabledelegate.h"
#include "c5part2tabledelegate.h"
#include "dlgpayment.h"
#include "dlgsearchinmenu.h"
#include "dlgpassword.h"
#include "dlgguest.h"
#include "c5waiterorderdoc.h"
#include "c5logtoserverthread.h"
#include "dlglistofdishcomments.h"
#include "dlglistdishspecial.h"
#include "stoplist.h"
#include "dlgface.h"
#include "dlglistofpackages.h"
#include "dlgtools.h"
#include "dlgsplitorder.h"
#include "dlgcarnumber.h"
#include "dlglistofmenu.h"
#include "dlgguests.h"
#include <QCloseEvent>
#include <QScrollBar>

#define PART2_COL_WIDTH 150
#define PART2_ROW_HEIGHT 60
#define PART3_ROW_HEIGHT 80
#define PART4_ROW_HEIGHT 80

static QList<C5LogRecord> fLogDelay;

DlgOrder::DlgOrder() :
    C5Dialog(C5Config::dbParams()),
    ui(new Ui::DlgOrder)
{
    ui->setupUi(this);
    fCarNumber = 0;
    ui->lbCar->setVisible(C5Config::carMode());
    ui->btnCar->setVisible(C5Config::carMode());
    ui->lbVisit->setVisible(false);
    fOrder = new C5WaiterOrderDoc();
    ui->btnGuest->setVisible(C5Config::useHotel());
    ui->btnCompactDishAddMode->setChecked(C5Config::getRegValue("compact dish add mode", true).toBool());
    fTimerCounter = 0;
    connect(&fTimer, &QTimer::timeout, this, &DlgOrder::timeout);
    fTimer.start(1000);
}

DlgOrder::~DlgOrder()
{
    delete ui;
    delete fOrder;
}

void DlgOrder::openTable(const QJsonObject &table, C5User *user)
{
    __userid = user->fId;
    __username = user->fFull;
    QJsonObject hall = C5CafeCommon::hall(table["f_hall"].toString());
    C5Config::setValues(C5CafeCommon::fHallConfigs[hall["f_settings"].toString().toInt()]);
    if (table["f_special_config"].toString().toInt() > 0) {
        C5Config::setValues(C5CafeCommon::fHallConfigs[table["f_special_config"].toString().toInt()]);
    }
//    if (hall.count() > 0) {
//        C5Config::setServiceFactor(C5CafeCommon::fHallConfigs[hall["f_settings"].toString().toInt()][param_service_factor]);
//        QString menuName = C5Menu::fMenuNames[C5CafeCommon::fHallConfigs[hall["f_settings"].toString().toInt()][param_default_menu]];
//        C5Config::setValue(param_default_menu_name, menuName);
//    }
    DlgOrder *d = new DlgOrder();
    d->ui->lbCar->setVisible(C5Config::carMode());
    d->ui->btnCar->setVisible(C5Config::carMode());
    d->fMenuName = C5Config::defaultMenuName();
    d->showFullScreen();
    d->hide();
    qApp->processEvents();
    d->ui->btnRoomService->setEnabled(user->check(cp_t5_change_service_value));
    d->ui->tblPart1->fitRowToHeight();
    d->ui->tblPart1->fitColumnsToWidth();
    QFont f(qApp->font());
    f.setBold(true);
    d->ui->tblPart1->setFont(f);
    d->ui->tblPart2->setItemDelegate(new C5Part2TableDelegate());
    d->ui->tblPart2->horizontalHeader()->setDefaultSectionSize(PART2_COL_WIDTH);
    d->ui->tblPart2->fitColumnsToWidth();
    d->ui->tblPart2->verticalHeader()->setDefaultSectionSize(PART2_ROW_HEIGHT);
    d->ui->tblPart2->fitRowToHeight();
    d->ui->tblDishes->setItemDelegate(new C5DishTableDelegate());
    d->ui->tblDishes->horizontalHeader()->setDefaultSectionSize(PART2_COL_WIDTH);
    d->ui->tblDishes->fitColumnsToWidth();
    d->ui->tblDishes->verticalHeader()->setDefaultSectionSize(PART3_ROW_HEIGHT);
    d->ui->tblDishes->fitRowToHeight();
    d->ui->worder->config(d->fOrder);
    d->buildMenu(d->fMenuName, "", "");
    d->fUser = user;
    d->ui->lbTable->setText(table["f_name"].toString());
    d->ui->lbStaffName->setText(QString("%1%2").arg(user->fFull).arg(__c5config.autoDateCash() ? "" : " [" + __c5config.dateCash() + " / " + QString::number(__c5config.dateShift()) + "]"));
    d->load(table["f_id"].toString().toInt());
    d->exec();
    delete d;
}

void DlgOrder::accept()
{
    if (!fOrder->hString("f_id").isEmpty()) {
        if (fOrder->isEmpty()) {
            if (C5Message::question(tr("Order is empty. Remove?")) == QDialog::Accepted) {
                fOrder->hSetString("closeempty", "1");
            }
        }
    }
    C5SocketHandler *sh = createSocketHandler(SLOT(saveAndQuit(QJsonObject)));
    fOrder->hSetString("unlocktable", "1");
    fOrder->sendToServer(sh);
}

void DlgOrder::reject()
{
    accept();
}

void DlgOrder::load(int table)
{
    C5SocketHandler *sh = createSocketHandler(SLOT(handleOpenTable(QJsonObject)));
    sh->bind("cmd", sm_opentable);
    sh->bind("table", table);
    sh->bind("host", hostinfo);
    sh->send();
}

void DlgOrder::buildMenu(const QString &menu, QString part1, QString part2)
{
    if (menu.isEmpty()) {
        C5Message::error(tr("Menu is not defined"));
        return;
    }
    ui->btnChangeMenu->setText(menu);
    QStringList p1 = C5Menu::fMenu[menu].keys();
    int col = 0, row = 0;
    ui->tblPart1->clear();
    foreach (QString s, p1) {
        ui->tblPart1->setItem(row, col++, new QTableWidgetItem(s));
        if (col == 2) {
            col = 0;
            row ++;
        }
    }

    if (part1.isEmpty()) {
        if (!ui->tblPart1->item(0, 0)) {
            C5Message::error(tr("Menu is not defined"));
            return;
        }
        part1 = ui->tblPart1->item(0, 0)->text();
    }

    ui->tblPart2->clear();
    ui->tblPart2->setRowCount(0);
    QStringList p2 = C5Menu::fMenu[menu][part1].keys();
    int colCount = ui->tblPart2->columnCount();
    col = 0;
    row = 0;
    foreach (QString s, p2) {
        if (row > ui->tblPart2->rowCount() - 1) {
            ui->tblPart2->setRowCount(row + 1);
        }
        QTableWidgetItem *item = new QTableWidgetItem(s);
        item->setData(Qt::UserRole, C5Menu::fPart2Color[s]);
        ui->tblPart2->setItem(row, col++, item);
        if (col == colCount) {
            col = 0;
            row ++;
        }
    }
    if (ui->tblPart2->rowCount() > 0) {
        for (int i = 0; i < ui->tblPart2->columnCount(); i++) {
            QTableWidgetItem *item = ui->tblPart2->item(ui->tblPart2->rowCount() - 1, i);
            if (!item) {
                item = new QTableWidgetItem("");
                item->setData(Qt::UserRole, -1);
                ui->tblPart2->setItem(ui->tblPart2->rowCount() - 1, i, item);
            }
        }
    }

    ui->tblDishes->clear();
    ui->tblDishes->setRowCount(0);
    if (part2.isEmpty()) {
        return;
    }
    colCount = ui->tblDishes->columnCount();
    col = 0;
    row = 0;
    QList<QJsonObject> dishes = C5Menu::fMenu[menu][part1][part2];
    for (int i = 0; i < dishes.count(); i++) {
        QJsonObject &o = dishes[i];
        QTableWidgetItem *item = new QTableWidgetItem(o["f_name"].toString());
        item->setData(Qt::UserRole, o);
        if (row > ui->tblDishes->rowCount() - 1) {
            ui->tblDishes->setRowCount(row + 1);
        }
        ui->tblDishes->setItem(row, col++, item);
        if (col == colCount) {
            col = 0;
            row ++;
        }
    }
}

void DlgOrder::addDishToOrder(const QJsonObject &obj)
{
    QJsonObject o = obj;
    o["f_guest"] = "1";
    if (o["f_hourlypayment"].toString().toInt() > 0) {
        for (int i = 0; i < fOrder->itemsCount(); i++) {
            if (fOrder->iInt("f_hourlypayment", i) > 0 && fOrder->iInt("f_state", i) == DISH_STATE_OK) {
                C5Message::error(tr("Hourly payment already exists"));
                return;
            }
        }
    }
    bool found = false;
    QString special = DlgListDishSpecial::getSpecial(obj["f_dish"].toString(), fDBParams);
    o["f_comment"] = special;
    if (ui->btnCompactDishAddMode->isChecked()) {
        for (int i = 0; i < fOrder->itemsCount(); i++) {
            if (o["f_dish"].toString() == fOrder->iString("f_dish", i)) {
                if (fOrder->iInt("f_state", i) != DISH_STATE_OK) {
                    continue;
                }
                if (o["f_store"].toString() != fOrder->iString("f_store", i)) {
                    break;
                }
                if (o["f_print1"].toString() != fOrder->iString("f_print1", i)) {
                    break;
                }
                if (o["f_print2"].toString() != fOrder->iString("f_print2", i)) {
                    break;
                }
                if (o["f_comment"].toString() != fOrder->iString("f_comment", i)) {
                    break;
                }
                fOrder->iSetString("f_qty1", float_str(fOrder->iDouble("f_qty1", i) + 1, 1), i);
                ui->worder->updateItem(fOrder->item(i), i);
                found = true;
            }
        }
    }
    if (!found) {
        if (obj["f_id"].toString().isEmpty()) {
            o["f_id"] = "";
            o["f_header"] = fOrder->hString("f_id");
            o["f_state"] = QString::number(DISH_STATE_OK);
            o["f_service"] = C5Config::serviceFactor();
            o["f_discount"] = fOrder->hString("f_discountfactor");
            o["f_total"] = "0";
            o["f_qty1"] = "1";
            o["f_qty2"] = "0";
            logRecord("", "New dish", o["f_name"].toString() + " " + o["f_comment"].toString(), "");
        }
        ui->worder->addItem(o);
        fOrder->addItem(o);
    }
    fOrder->hSetDouble("f_amountcash", 0);
    fOrder->hSetDouble("f_amountcard", 0);
    fOrder->hSetDouble("f_amountbank", 0);
    fOrder->hSetDouble("f_amountother", 0);
    fOrder->countTotal();
    setButtonsState();
}

void DlgOrder::loadOrder(const QJsonObject &obj)
{

    ui->lbCar->clear();
    ui->lbVisit->clear();
    ui->lbVisit->setVisible(false);
    fOrder->fHeader = obj["header"].toObject();
    if (fOrder->hString("f_id").isEmpty()) {
        logRecord("", "New order", fOrder->hString("f_tablename"), "");
        fOrder->hSetInt("f_staff", fUser->fId);
        fOrder->hSetString("f_staffname", fUser->fFull);
        fOrder->hSetInt("f_state", ORDER_STATE_OPEN);
        fOrder->hSetInt("f_guests", 1);
        fOrder->hSetString("f_comment", "");
        fOrder->hSetString("f_hall", obj["table"].toArray().at(0).toObject()["f_hall"].toString());
        fOrder->hSetDouble("f_amounttotal", 0);
        fOrder->hSetDouble("f_amountcash", 0);
        fOrder->hSetDouble("f_amountcard", 0);
        fOrder->hSetDouble("f_amountbank", 0);
        fOrder->hSetDouble("f_amountother", 0);
        fOrder->hSetDouble("f_amountservice", 0);
        fOrder->hSetDouble("f_amountdiscount", 0);
        fOrder->hSetString("f_servicemode", C5CafeCommon::serviceMode(C5CafeCommon::hall(obj["table"].toArray().at(0).toObject()["f_hall"].toString())["f_settings"].toString()));
        fOrder->hSetString("f_servicefactor", C5Config::serviceFactor());
        fOrder->hSetDouble("f_discountfactor", 0);
    } else {
        logRecord("", "Open order", fOrder->hString("f_tablename"), "");
    }

    QJsonObject hall = C5CafeCommon::hall(fOrder->hString("f_hall"));
    C5Config::setValues(C5CafeCommon::fHallConfigs[hall["f_settings"].toString().toInt()]);
    QJsonObject table = C5CafeCommon::table(fOrder->hInt("f_table"));
    if (table["f_special_config"].toString().toInt() > 0) {
        C5Config::setValues(C5CafeCommon::fHallConfigs[table["f_special_config"].toString().toInt()]);
    }
    ui->lbCar->setVisible(C5Config::carMode());
    ui->btnCar->setVisible(C5Config::carMode());

    ui->lbTable->setText(fOrder->hString("f_tablename"));
    fOrder->hSetInt("f_currentstaff", fUser->fId);
    fOrder->hSetString("f_currentstaffname", fUser->fFull);
    fOrder->fItems = obj["body"].toArray();
    itemsToTable();
    if (fOrder->hDouble("f_discountfactor") > 0) {
        ui->lbDiscount->setEnabled(true);
        ui->lbDiscount->setText(QString("[%1: %2%]").arg(tr("Discount")).arg(fOrder->hDouble("f_discountfactor") * 100));
    } else {
        ui->lbDiscount->setEnabled(false);
        ui->lbDiscount->setText(tr("[Discount]"));
    }
    if (fOrder->hString("car").toInt() > 0) {
        setCar(fOrder->hInt("car"));
    }
    setServiceLabel();
    if (fOrder->hInt("f_staff") != fOrder->hInt("f_currentstaff")) {
        C5Message::info(QString("%1\r\n%2").arg(tr("Order owner")).arg(fOrder->hString("f_staffname")));
    }
}

void DlgOrder::changeQty(double qty)
{
    int index = 0;
    if (!ui->worder->currentRow(index)) {
        return;
    }
    QJsonObject o = fOrder->item(index);
    if (o["d_package"].toString().toInt() > 0) {
        C5Message::error(tr("You cannot change the quantity of items of package"));
        return;
    }
    if (o["f_hourlypayment"].toString().toInt() > 0) {
        C5Message::error(tr("This is hourly payment item"));
        return;
    }
    QString oldQty = o["f_qty1"].toString();
    if (qty > 0) {
        if (str_float(o["f_qty1"].toString()) + qty > 999) {
            logRecord(o["f_id"].toString(), "Try set qty more than 999", "", "");
            return;
        }
        if (qty < 1 && str_float(o["f_qty2"].toString()) < 0.09) {
            o["f_qty1"] = float_str(qty, 2);
        } else {
            o["f_qty1"] = float_str(o["f_qty1"].toString().toDouble() + qty, 2);
        }
    } else {
        if (str_float(o["f_qty1"].toString()) + qty < 0.09) {
            return;
        }
        if (str_float(o["f_qty2"].toString()) <= str_float(o["f_qty1"].toString()) + qty) {
            o["f_qty1"] = float_str(o["f_qty1"].toString().toDouble() + qty, 2);
        }
    }
    logRecord(o["f_id"].toString(), "Qty of " + o["f_name"].toString(), oldQty, o["f_qty1"].toString());
    fOrder->fItems[index] = o;
    fOrder->countTotal();
    setButtonsState();
    ui->worder->updateItem(fOrder->fItems[index].toObject(), index);
}

void DlgOrder::itemsToTable()
{
    ui->worder->config(fOrder);
    countHourlyPayment();
    fOrder->countTotal();
    ui->worder->itemsToTable(0);
    ui->btnSit->setText(fOrder->hString("f_guests"));
    setButtonsState();
}

void DlgOrder::saveOrder()
{
    C5SocketHandler *sh = createSocketHandler(SLOT(saveAndQuit(QJsonObject)));
    sh->bind("cmd", sm_saveorder);
    QJsonObject o;
    o["header"] = fOrder->fHeader;
    o["body"] = fOrder->fItems;
    sh->send(o);
}

void DlgOrder::setServiceLabel()
{
    if (fOrder->hDouble("f_servicefactor") > 0) {
        ui->lbService->setEnabled(true);
        ui->lbService->setText(QString("[%1: %2%]").arg(tr("Service")).arg(fOrder->hDouble("f_servicefactor") * 100));
    } else {
        ui->lbService->setEnabled(false);
        ui->lbService->setText(tr("[Service]"));
    }
}

void DlgOrder::logRecord(const QString &rec, const QString &action, const QString &value1, const QString &value2)
{
    if (fOrder->hString("f_id").isEmpty()) {
        C5LogRecord l;
        l.fType = LOG_WAITER;
        l.fUser = __username;
        l.fDate = QDate::currentDate();
        l.fTime = QTime::currentTime();
        l.fRec = rec;
        l.fInvoice = "";
        l.fReservation = "";
        l.fAction = action;
        l.fValue1 = value1;
        l.fValue2 = value2;
        fLogDelay.append(l);
    } else {
        processDelayedLogs();
        C5LogToServerThread::remember(LOG_WAITER, __username, rec, fOrder->hString("f_id"), "", action, value1, value2);
    }
}

void DlgOrder::processDelayedLogs()
{
    if (!fOrder->hString("f_id").isEmpty()) {
        foreach (C5LogRecord r, fLogDelay) {
            C5LogToServerThread::remember(LOG_WAITER, r.fUser, r.fRec, fOrder->hString("f_id"), "", r.fAction, r.fValue1, r.fValue2);
        }
        fLogDelay.clear();
    }
}

void DlgOrder::setButtonsState()
{
    bool btnSendToCooking = false;
    bool btnPayment = true;

    for (int i = 0; i < fOrder->fItems.count(); i++) {
        if (fOrder->iInt("f_state", i) != DISH_STATE_OK) {
            continue;
        }
        if (fOrder->iDouble("f_qty1", i) > fOrder->iDouble("f_qty2", i)) {
            btnSendToCooking = true;
        }
    }

    if (btnSendToCooking) {
        btnPayment = false;
    }

    ui->btnPrintService->setEnabled(btnSendToCooking);
    ui->btnPayment->setEnabled(btnPayment);
}

void DlgOrder::changeTimeOrder()
{
    int index = 0;
    if (!ui->worder->currentRow(index)) {
        return;
    }
    QJsonObject o = fOrder->item(index);
    o["f_timeorder"] = static_cast<QPushButton*>(sender())->text();
    fOrder->fItems[index] = o;
    ui->worder->updateItem(fOrder->fItems[index].toObject(), index);
}

void DlgOrder::setCar(int num)
{
    QString govnumber;
    int client;
    ui->lbCar->setVisible(false);
    C5Database db(fDBParams);
    db[":f_id"] = num;
    db.exec("select concat(c.f_name, ', ', bc.f_govnumber, ', ', trim(cl.f_contact)), "
            "bc.f_govnumber, cl.f_id as f_client "
            "from b_car bc "
            "left join c_partners cl on cl.f_id=bc.f_costumer "
            "left join s_car c on c.f_id=bc.f_car "
            "where bc.f_id=:f_id");
    if (db.nextRow()) {
        fCarNumber = num;
        govnumber = db.getString(1);
        client = db.getInt("f_client");
        ui->lbCar->setVisible(true);
        ui->lbCar->setEnabled(true);
        ui->lbCar->setText(QString("[%1]").arg(db.getString(0)));
    }
    fOrder->fHeader["car"] = QString::number(num);

    if (fOrder->hString("f_id").isEmpty()) {
        C5SocketHandler *sh = createSocketHandler(SLOT(saveAndDiscount(QJsonObject)));
        sh->bind("cmd", sm_saveorder);
        QJsonObject o;
        o["header"] = fOrder->fHeader;
        o["body"] = fOrder->fItems;
        o["govnumber"] = govnumber;
        sh->send(o);
    } else {
//        QJsonObject obj;
//        obj["saved"] = 1;
//        obj["govnumber"] = govnumber;
//        saveAndDiscount(obj);
        C5SocketHandler *sh = createSocketHandler(SLOT(saveAndDiscount(QJsonObject)));
        sh->bind("cmd", sm_saveorder);
        QJsonObject o;
        o["header"] = fOrder->fHeader;
        o["body"] = fOrder->fItems;
        o["govnumber"] = govnumber;
        sh->send(o);
    }
}

void DlgOrder::countHourlyPayment()
{
    if (fOrder->hInt("f_print") > 0) {
        return;
    }
    for (int i = 0; i < fOrder->itemsCount(); i++) {
        if (fOrder->iInt("f_hourlypayment", i) > 0) {
            QString ts = fOrder->hString("f_dateopen") + " " + fOrder->hString("f_timeopen");
            QDateTime t1 = QDateTime::fromString(ts, "dd/MM/yyyy HH:mm:ss");
            QDateTime t2 = QDateTime::currentDateTime();
            qDebug() << t1 << t2;
            int min = t1.secsTo(t2);
            QString hint = QString("%1:%2").arg(min / 3600, 2, 10, QChar('0')).arg((min % 3600) / 60, 2, 10, QChar('0'));
            min /= 60;
            min /= fOrder->iInt("f_hourlypayment", i);
            qDebug() << min % fOrder->iInt("f_hourlypayment", i);
            if (min % fOrder->iInt("f_hourlypayment", i) > 0) {
                min++;
            }
            if (min == 0) {
                min = 1;
            }
            fOrder->iSetDouble("f_total", min * fOrder->iDouble("f_price", i), i);
            fOrder->iSetString("f_comment", hint, i);
            if (ui->worder->rowCount() > i) {
                ui->worder->updateItem(fOrder->fItems[i].toObject(), i);
            }
        }
    }
    fOrder->countTotal();
    ui->worder->updatePayment(0);
}

void DlgOrder::restoreStoplistQty(int dish, double qty)
{
    auto *sh = createSocketHandler(SLOT(handleStopList(QJsonObject)));
    sh->bind("cmd", sm_stoplist);
    sh->bind("dish", dish);
    sh->bind("qty", qty);
    sh->bind("state", sl_restore_qty);
    sh->send();
}

void DlgOrder::timeout()
{
    fTimerCounter++;
    if ((fTimerCounter % 60) == 0) {
        countHourlyPayment();
    }
}

void DlgOrder::handleDiscount(const QJsonObject &obj)
{
    sender()->deleteLater();
    if (obj["reply"].toInt() == 0) {
        if (obj["handlevisit"].toInt() == 1) {
            /* CHECK FOR AUTOMATICALLY EVERY 11 DISCOUNT BY GOVNUMBER */
            QString govnumber = obj["code"].toString();
            if (!govnumber.isEmpty() && fOrder->hDouble("f_discountfactor") < 0.001) {
                C5SocketHandler *sh = createSocketHandler(SLOT(handleVisit(QJsonObject)));
                sh->bind("cmd", sm_checkdiscount_by_visit);
                sh->bind("order", fOrder->hString("f_id"));
                sh->bind("code", govnumber);
                sh->send();
                return;
            }
        } else {
            C5Message::error(obj["msg"].toString());
            return;
        }
    }
    switch (obj["type"].toInt()) {
    case CARD_TYPE_DISCOUNT:
        fOrder->hSetString("f_bonustype", obj["card"].toObject()["f_type"].toString());
        fOrder->hSetString("f_bonusid", obj["card"].toObject()["f_id"].toString());
        switch (obj["card"].toObject()["f_type"].toString().toInt()) {
        case CARD_TYPE_DISCOUNT:
            fOrder->hSetDouble("f_discountfactor", obj["card"].toObject()["f_value"].toString().toDouble() / 100.0);
            for (int i = 0; i < fOrder->fItems.count(); i++) {
                fOrder->iSetString("f_discount", fOrder->hString("f_discountfactor"), i);
            }
            break;
        }
        fOrder->countTotal();
        break;
    }
    itemsToTable();
}

void DlgOrder::handleVisit(const QJsonObject &obj)
{
    sender()->deleteLater();
    if (obj["reply"].toInt() == 0) {
        C5Message::error(obj["msg"].toString());
        return;
    }
    if (obj["noconfig"].toInt() > 0) {
        return;
    }
    ui->lbVisit->setVisible(true);
    ui->lbVisit->setEnabled(true);
    ui->lbVisit->setText(QString("[%1 / %2]").arg(obj["current"].toInt()).arg(obj["visit"].toInt()));
    if (obj["visit"].toInt() > 0 && obj["current"].toInt() == 0 && fOrder->hDouble("f_discountfactor") < 0.001) {
        C5SocketHandler *sh = createSocketHandler(SLOT(handleDiscount(QJsonObject)));
        sh->bind("cmd", sm_discount);
        sh->bind("order", fOrder->hString("f_id"));
        sh->bind("code", obj["card_code"].toString());
        sh->send();
    }
}

void DlgOrder::handleOpenTable(const QJsonObject &obj)
{
    sender()->deleteLater();
    switch (obj["reply"].toInt()) {
    case 0:
    case 2: {
        C5Message::error(obj["msg"].toString());
        QDialog::accept();
        return;
    }
    case 3: {
        C5SocketHandler *sh = createSocketHandler(SLOT(handleOpenTable(QJsonObject)));
        sh->bind("cmd", sm_opentable);
        sh->bind("table", obj["table"].toInt());
        sh->bind("host", hostinfo);
        sh->send();
        return;
    }
    case 1: {
        loadOrder(obj);
        break;
    }
    }
}

void DlgOrder::handlePrintService(const QJsonObject &obj)
{
    sender()->deleteLater();
    fOrder->fItems = obj["body"].toArray();
    fOrder->fHeader = obj["header"].toObject();
    itemsToTable();
    C5SocketHandler *sh = createSocketHandler(SLOT(saveAndQuit(QJsonObject)));
    fOrder->sendToServer(sh);
}

void DlgOrder::handleStopList(const QJsonObject &obj)
{
    sender()->deleteLater();
    switch (obj["state"].toInt()) {
    case sl_set:
        C5Menu::fStopList[obj["f_dish"].toString().toInt()] = obj["f_qty"].toDouble();
        break;
    case sl_add: {
        QJsonObject jdish = QJsonDocument::fromJson(obj["dish"].toString().toUtf8()).object();
        switch (obj["status"].toInt()) {
        case sl_ok: {
            C5Menu::fStopList[jdish["f_dish"].toString().toInt()] = obj["newqty"].toDouble();
            addDishToOrder(jdish);
            break;
        }
        case sl_not_enough_qty:
            C5Message::error(obj["sl_msg"].toString());
            break;
        case sl_not_in_stoplist:
            addDishToOrder(jdish);
            C5Menu::fStopList.remove(jdish["f_dish"].toString().toInt());
            break;
        }
    }
    case sl_set_qty:
        break;
    case sl_remove:
        C5Menu::fStopList.remove(obj["f_dish"].toString().toInt());
        break;
    case sl_restore_qty:
        switch (obj["status"].toInt()) {
        case sl_ok:
            C5Menu::fStopList[obj["dish"].toInt()] = obj["newqty"].toDouble();
            break;
        case sl_not_in_stoplist:
            C5Menu::fStopList.remove(obj["dish"].toString().toInt());
            break;
        default:
            C5Message::error(obj["msg"].toString());
            break;
        }
        break;
    }

    ui->tblDishes->viewport()->update();
}

void DlgOrder::saveAndQuit(const QJsonObject &obj)
{
    sender()->deleteLater();
    fOrder->fHeader = obj["header"].toObject();
    fOrder->fItems = obj["body"].toArray();
    processDelayedLogs();
    if (obj["header"].toObject()["unlocktable"].toString().toInt() > 0) {
        QDialog::accept();
        return;
    }
    itemsToTable();
}

void DlgOrder::saveAndDiscount(const QJsonObject &obj)
{
    sender()->deleteLater();
    if (!obj.contains("saved")) {
        fOrder->fHeader = obj["header"].toObject();
        fOrder->fItems = obj["body"].toArray();
    }
    /* CHECK FOR AUTOMATICALLY DISCOUNT BY GOV NUMBER*/
    QString govnumber = obj["govnumber"].toString();
    if (!govnumber.isEmpty() && fOrder->hDouble("f_discountfactor") < 0.001) {
        C5SocketHandler *sh = createSocketHandler(SLOT(handleDiscount(QJsonObject)));
        sh->bind("cmd", sm_discount);
        sh->bind("order", fOrder->hString("f_id"));
        sh->bind("code", govnumber);
        sh->bind("handlevisit", 1);
        sh->send();
    }
}

void DlgOrder::changeTable(const QJsonObject &obj)
{
    sender()->deleteLater();
    if (obj["reply"].toInt() == 0) {
        C5Message::error(obj["msg"].toString());
        return;
    }
    fOrder->fHeader = obj["header"].toObject();
    processDelayedLogs();
    int tableId;
    if (!DlgFace::getTable(tableId, fOrder->hString("f_hall"))) {
        return;
    }
    QJsonObject table = C5CafeCommon::table(tableId);
    if (table.count() > 0) {
        ui->lbTable->setText(table["f_name"].toString());
        QJsonObject hall = C5CafeCommon::hall(table["f_hall"].toString());
        if (hall.count() > 0) {
            C5Config::setServiceFactor(C5CafeCommon::fHallConfigs[hall["f_settings"].toString().toInt()][param_service_factor]);
            QString menuName = C5Menu::fMenuNames[C5CafeCommon::fHallConfigs[hall["f_settings"].toString().toInt()][param_default_menu]];
            if (fMenuName != menuName) {
                fMenuName = menuName;
                buildMenu(fMenuName, "", "");
            }
        }
    }
    load(tableId);
}

void DlgOrder::handleError(int err, const QString &msg)
{
    Q_UNUSED(err);
    sender()->deleteLater();
    C5Message::error(msg);
}

void DlgOrder::on_tblPart1_itemClicked(QTableWidgetItem *item)
{
    if (!item) {
        return;
    }
    if (item->text().isEmpty()) {
        return;
    }
    fPart1Name = item->text();
    buildMenu(fMenuName, fPart1Name, "");
}

void DlgOrder::on_tblPart2_itemClicked(QTableWidgetItem *item)
{
    if (!item) {
        return;
    }

    QString part2 = item->text();
    if (part2.isEmpty()) {
        return;
    }
    buildMenu(fMenuName, fPart1Name, part2);
}

void DlgOrder::on_tblDishes_itemClicked(QTableWidgetItem *item)
{
    if (!item) {
        return;
    }
    if (item->text().isEmpty()) {
        return;
    }
    const QJsonObject &o = item->data(Qt::UserRole).toJsonObject();
    if (ui->btnStopListMode->isChecked()) {
        double max = 999;
        if (!DlgPassword::getAmount(o["f_name"].toString(), max, false)) {
            return;
        }
        auto *sh = createSocketHandler(SLOT(handleStopList(QJsonObject)));
        sh->bind("cmd", sm_stoplist);
        sh->bind("f_dish", o["f_dish"].toString());
        sh->bind("f_stopqty", max);
        sh->bind("state", 1);
        sh->send();
    } else {
        if (C5Menu::fStopList.contains(o["f_dish"].toString().toInt())) {
            if (C5Menu::fStopList[o["f_dish"].toString().toInt()] < 0.01) {
                return;
            }
            auto *sh = createSocketHandler(SLOT(handleStopList(QJsonObject)));
            sh->bind("cmd", sm_stoplist);
            sh->bind("f_dish", o["f_dish"].toString());
            sh->bind("f_qty", 1);
            sh->bind("state", sl_add);
            sh->bind("dish", QJsonDocument(o).toJson(QJsonDocument::Compact));
            sh->send();
            return;
        }
        addDishToOrder(o);
    }
}

void DlgOrder::on_btnPlus1_clicked()
{
    changeQty(1);
}

void DlgOrder::on_btnMin1_clicked()
{
    changeQty(-1);
}

void DlgOrder::on_btnPlus05_clicked()
{
    changeQty(0.5);
}

void DlgOrder::on_btnMin05_clicked()
{
    changeQty(-0.5);
}

void DlgOrder::on_btnCustom_clicked()
{
    int index = 0;
    if (!ui->worder->currentRow(index)) {
        return;
    }
    double max = 999;
    QJsonObject o = fOrder->item(index);
    if (!DlgPassword::getAmount(o["f_name"].toString(), max, false)) {
        return;
    }
    changeQty(max);
}

void DlgOrder::on_btnExit_clicked()
{
    accept();
}

void DlgOrder::on_btnVoid_clicked()
{
    int index = 0;
    if (!ui->worder->currentRow(index)) {
        return;
    }
    QJsonObject o = fOrder->fItems.at(index).toObject();
    QString oldQty = o["f_qty1"].toString() + "/" + o["f_qty2"].toString();
    if (o["f_qty2"].toString().toDouble() < 0.01) {
        if (C5Message::question(QString("%1<br>%2(%3)").arg(tr("Confirm to remove")).arg(o["f_name"].toString()).arg(o["f_qty1"].toString())) != QDialog::Accepted) {
            return;
        }
        o["f_state"] = DISH_STATE_NONE;
        fOrder->fItems[index] = o;
        saveOrder();
        logRecord(o["f_id"].toString(), "Remove dish", o["f_name"].toString(), o["f_qty1"].toString());
        restoreStoplistQty(o["f_dish"].toString().toInt(), o["f_qty1"].toString().toDouble());
        return;
    } else {
        double max = 0;
        if (fUser->check(cp_t5_remove_printed_service)) {
            max = o["f_qty1"].toString().toDouble();
        } else {
            max = o["f_qty1"].toString().toDouble() - o["f_qty2"].toString().toDouble();
        }
        if (max < 0.01) {
            return;
        }
        if (!DlgPassword::getAmount(o["f_name"].toString(), max, true)) {
            return;
        }
        if (max <= o["f_qty1"].toString().toDouble() - o["f_qty2"].toString().toDouble()) {
            o["f_qty1"] = QString::number(o["f_qty1"].toString().toDouble() - max);
            logRecord(o["f_id"].toString(), "Remove dish or change qty of " + o["f_name"].toString(), oldQty, o["f_qty1"].toString() + "/" + o["f_qty2"].toString());
        } else {
            QString reasonname;
            int reasonid;
            if (!DlgDishRemoveReason::getReason(reasonname, reasonid)) {
                return;
            }
            if (max > o["f_qty1"].toString().toDouble() - o["f_qty2"].toString().toDouble()) {
                max -= o["f_qty1"].toString().toDouble() - o["f_qty2"].toString().toDouble();
            }
            //GET DISH STATE
            int dishState = reasonid;
            if (max < o["f_qty1"].toString().toDouble()) {
                o["f_qty1"] = QString::number(o["f_qty1"].toString().toDouble() - max);
                QJsonObject o2 = o;
                o2["f_id"] = "0";
                o2["f_state"] = QString::number(dishState * -1); // Print removed dishes if state < 0
                o2["f_qty1"] = QString::number(max, 'f', 2);
                o2["f_qty2"] = o2["f_qty1"];
                o2["f_removereason"] = reasonname;
                fOrder->fItems.append(o2);
                if (o2["f_qty2"].toString().toDouble() > o2["f_qty1"].toString().toDouble()) {
                    o2["f_qty2"] = o2["f_qty1"];
                }
            } else {
                o["f_state"] = QString::number(dishState * -1); // Print removed dishes if state < 0
                o["f_removereason"] = reasonname;
            }
            if (o["f_qty2"].toString().toDouble() > o["f_qty1"].toString().toDouble()) {
                o["f_qty2"] = o["f_qty1"];
            }
            logRecord(o["f_id"].toString(), "Remove dish or change qty with reason of " + o["f_name"].toString() + ": " + reasonname, oldQty, o["f_qty1"].toString() + "/" + o["f_qty2"].toString());
        }
        if (o["f_qty2"].toString().toDouble() > o["f_qty1"].toString().toDouble()) {
            o["f_qty2"] = o["f_qty1"];
        }
        fOrder->fItems[index] = o;
        if (fOrder->hInt("f_print") > 0) {
            fOrder->hSetInt("f_print", fOrder->hInt("f_print") * -1);
        }
        saveOrder();
    }
}

void DlgOrder::on_btnComment_clicked()
{
    int index = 0;
    if (!ui->worder->currentRow(index)) {
        return;
    }
    QJsonObject o = fOrder->fItems.at(index).toObject();
    if (o["f_hourlypayment"].toString().toInt() > 0) {
        C5Message::error(tr("Cannot add comment to hourly payment"));
        return;
    }
    QString comment = o["f_name"].toString();
    if (!DlgListOfDishComments::getComment(comment)) {
        return;
    }
    o["f_comment"] = comment;
    logRecord(o["f_id"].toString(), "Comment for dish", o["f_name"].toString(), comment);
    fOrder->fItems[index] = o;
    ui->worder->updateItem(fOrder->fItems[index].toObject(), index);
}

void DlgOrder::on_btnDishScrollDown_clicked()
{
    ui->tblDishes->verticalScrollBar()->setValue(ui->tblDishes->verticalScrollBar()->value() + 3);
}

void DlgOrder::on_btnTypeScrollDown_clicked()
{
    ui->tblPart2->verticalScrollBar()->setValue(ui->tblPart2->verticalScrollBar()->value() + 2);
}

void DlgOrder::on_btnTypeScrollUp_clicked()
{
    ui->tblPart2->verticalScrollBar()->setValue(ui->tblPart2->verticalScrollBar()->value() - 2);
}

void DlgOrder::on_btnDishScrollUp_clicked()
{
    ui->tblDishes->verticalScrollBar()->setValue(ui->tblDishes->verticalScrollBar()->value() - 3);
}

void DlgOrder::on_btnExpandDishTable_clicked()
{
    if (ui->wDishTypes->isVisible()) {
        ui->wDishTypes->setVisible(false);
        ui->wDishTypesControl->setVisible(false);
        ui->btnExpandDishTable->setIcon(QIcon(":/minimize.png"));
    } else {
        ui->wDishTypes->setVisible(true);
        ui->wDishTypesControl->setVisible(true);
        ui->btnExpandDishTable->setIcon(QIcon(":/expand2.png"));
    }
}

void DlgOrder::on_btnChangeMenu_clicked()
{
    QString menu;
    if (!DlgListOfMenu::getMenuId(menu, fDBParams)) {
        return;
    }
    fMenuName = menu;
    buildMenu(menu, "", "");
}

void DlgOrder::on_btnChangeTable_clicked()
{
    C5SocketHandler *sh = createSocketHandler(SLOT(changeTable(QJsonObject)));
    fOrder->hSetString("unlocktable", "1");
    fOrder->sendToServer(sh);
}

void DlgOrder::on_btnGuest_clicked()
{
    QString res, inv, room, guest;
    DlgGuest::getGuest(res, inv, room, guest);
}

void DlgOrder::on_btnRoomService_clicked()
{
    QString oldFactor = fOrder->hString("f_servicefactor");
    if (fOrder->hDouble("f_servicefactor") > 0.0001) {
        fOrder->hSetDouble("f_servicefactor", 0);
    } else {
        fOrder->hSetString("f_servicefactor", C5Config::serviceFactor());
    }
    logRecord("", "Change service factor", oldFactor, fOrder->hString("f_servicefactor"));
    itemsToTable();
    fOrder->countTotal();
    setServiceLabel();
}

void DlgOrder::on_btnSearchInMenu_clicked()
{
    DlgSearchInMenu *d = new DlgSearchInMenu();
    connect(d, SIGNAL(dish(QJsonObject)), this, SLOT(addDishToOrder(QJsonObject)));
    d->buildMenu(fMenuName);
    d->exec();
    delete d;
}

void DlgOrder::on_btnCompactDishAddMode_clicked()
{
    C5Config::setRegValue("compact dish add mode", ui->btnCompactDishAddMode->isChecked());
}

void DlgOrder::on_btnTime1_clicked()
{
    changeTimeOrder();
}

void DlgOrder::on_btnTime2_clicked()
{
    changeTimeOrder();
}

void DlgOrder::on_btnTime3_clicked()
{
    changeTimeOrder();
}

void DlgOrder::on_btnCar_clicked()
{
    if (fOrder->hDouble("f_discountfactor") > 0.001) {
        C5Message::info(tr("You should not change the car, becouse discount was applied"));
        return;
    }
    int num;
    if (!DlgCarNumber::getNumber(num)) {
        return;
    }
    setCar(num);
}

void DlgOrder::on_btnPackage_clicked()
{
    int id;
    QString name;
    if (DlgListOfPackages::package(id, name)) {
        QList<QJsonObject> items = C5Menu::fPackagesList[id];
        for (const QJsonObject &o: items) {
            addDishToOrder(o);
        }
        logRecord("", "New package", name, "");
    }
}

void DlgOrder::on_btnPayment_clicked()
{
    if (C5Config::carMode()) {
        if (fOrder->hInt("car") == 0) {
            C5Message::error(tr("Car model and costumer not specified"));
            return;
        }
    }
    bool empty = true;
    for (int i = 0; i < fOrder->itemsCount(); i++) {
        if (fOrder->iInt("f_state", i) != DISH_STATE_OK) {
            continue;
        }
        if (fOrder->iDouble("f_qty1", i) > fOrder->iDouble("f_qty2", i)) {
            C5Message::error(tr("Order is incomplete"));
            return;
        }
        empty = false;
    }
    if (empty) {
        C5Message::error(tr("Order is incomplete"));
        return;
    }
    int paymentResult = DlgPayment::payment(fUser, fOrder);
    switch (paymentResult) {
    case PAYDLG_ORDER_CLOSE:
        fOrder->fHeader = QJsonObject();
        fOrder->fItems = QJsonArray();
        itemsToTable();
        if (__c5config.waiterLoginAfterPayment())  {
            accept();
        } else {
            on_btnChangeTable_clicked();
        }
        break;
    case PAYDLG_NONE:
        saveOrder();
        break;
    }
}

void DlgOrder::on_btnPrintService_clicked()
{
    C5SocketHandler *sh = createSocketHandler(SLOT(handlePrintService(QJsonObject)));
    sh->bind("cmd", sm_printservice);
    QJsonObject o;
    o["header"] = fOrder->fHeader;
    o["body"] = fOrder->fItems;
    sh->send(o);
    logRecord("", "Send to cooking", "", "");
}

void DlgOrder::on_btnSit_clicked()
{
    auto *d = new DlgGuests(fDBParams, fOrder);
    d->exec();
    delete d;
    itemsToTable();
}

void DlgOrder::on_btnJoinTable_clicked()
{
    auto *d = new DlgSplitOrder(fDBParams);
    d->configOrder(fOrder);
    d->exec();
    d->deleteLater();
}

void DlgOrder::on_btnTools_clicked()
{
    DlgTools d(fDBParams);
    d.exec();
}
