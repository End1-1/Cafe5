#include "dlgorder.h"
#include "c5socketmessage.h"
#include "ui_dlgorder.h"
#include "c5cafecommon.h"
#include "c5user.h"
#include "c5witerconf.h"
#include "dlgdishremovereason.h"
#include "c5orderdriver.h"
#include "c5ordertabledelegate.h"
#include "dlgmenuset.h"
#include "dlgsearchinmenu.h"
#include "dlgpassword.h"
#include "dlgguest.h"
#include "dlgqty.h"
#include "c5logtoserverthread.h"
#include "dlglistofdishcomments.h"
#include "customerinfo.h"
#include "dlglistdishspecial.h"
#include "dlgreceiptlanguage.h"
#include "stoplist.h"
#include "dlgface.h"
#include "c5translator.h"
#include "dlglist.h"
#include "dlglistofpackages.h"
#include "dlgsplitorder.h"
#include "dlgpreorderw.h"
#include "cashboxconfig.h"
#include "dlgprecheckoptions.h"
#include "dlgcarnumber.h"
#include "worderw.h"
#include "dlglistofmenu.h"
#include "dishitem.h"
#include "datadriver.h"
#include "cpartners.h"
#include "change.h"
#include "dlgcl.h"
#include "idram.h"
#include "dlgguests.h"
#include "dlgchosesplitorderoption.h"
#include "qdishpart2button.h"
#include "qdishbutton.h"
#include "dlgstoplistoption.h"
#include "dlgaskforprecheck.h"
#include "dlgviewstoplist.h"
#include "dlgtext.h"
#include <QToolButton>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QScrollBar>
#include <QInputDialog>
#include <QPaintEvent>
#include <QPainter>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSettings>
#include <QSslSocket>

#define PART2_COL_WIDTH 150
#define PART2_ROW_HEIGHT 60
#define PART3_ROW_HEIGHT 80
#define PART4_ROW_HEIGHT 80

DlgOrder::DlgOrder(C5User *user) :
    C5Dialog(C5Config::dbParams()),
    ui(new Ui::DlgOrder),
    fUser(user)
{
    ui->setupUi(this);
    ui->leReceived->clear();
    ui->leChange->clear();
    fOpenDateTime = QDateTime::currentDateTime();
    fCarNumber = 0;
    ui->lbVisit->setVisible(false);
    ui->lbStaff->setText(user->fullName());
    ui->wqtypaneldown->setVisible(false);
    ui->btnBillWithoutService->setEnabled(user->check(cp_t5_bill_without_service));
    fTimerCounter = 0;
    connect( &fTimer, &QTimer::timeout, this, &DlgOrder::timeout);
    fTimer.start(1000);
    ui->wpayment->setVisible(false);
    ui->btnPayTransferToRoom->setVisible(fUser->check(cp_t5_pay_transfer_to_room));
    ui->btnPayComplimentary->setVisible(fUser->check(cp_t5_pay_complimentary));
    ui->btnPayCityLedger->setVisible(fUser->check(cp_t5_pay_cityledger));
    ui->btnSelfCost->setVisible(fUser->check(cp_t5_pay_breakfast));
    if (!cp_t5_pay_idram) {
        ui->btnPaymentIdram->setVisible(false);
        ui->leIDRAM->setVisible(false);
    }
    if (!cp_t5_pay_payx) {
        ui->btnPayX->setVisible(false);
        ui->lePayX->setVisible(false);
        ui->btnFillPayX->setVisible(false);
        ui->btnCalcPayX->setVisible(false);
    }
#ifndef QT_DEBUG
    //ui->btnFillIdram->setVisible(false);
#endif
    setRoomComment();
    setDiscountComment();
    setComplimentary();
    setSelfcost();
    switch (C5Config::getRegValue("btn_tax_state").toInt()) {
        case 0:
        case 1:
            ui->btnTax->setChecked(true);
            break;
        case 2:
            ui->btnTax->setChecked(false);
            break;
    }
    if (__c5config.getValue(param_tax_print_always_offer).toInt() == 1) {
        ui->btnTax->setChecked(true);
        ui->btnTax->setEnabled(false);
    }
    QSettings _ls(qApp->applicationDirPath() + "/ls.inf", QSettings::IniFormat);
    if (_ls.value("ls/notax").toInt() == 1) {
        ui->btnTax->setChecked(false);
    }
    fMenuID = C5Config::defaultMenu();
    fPart1 = 0;
    fStoplistMode = false;
    connect(ui->scrollAreaDish->verticalScrollBar(), &QScrollBar::valueChanged, this, [this](int value) {
        for (QObject *o : ui->scrollAreaWidgetContentsDish->children()) {
            QWidget *w = dynamic_cast<QWidget *>(o);
            if (w) {
                w->repaint();
            }
        }
        ui->scrollAreaDish->viewport()->update();
    });
}

DlgOrder::~DlgOrder()
{
    delete ui;
}

void DlgOrder::openTable(int table, C5User *user)
{
    C5Config::setValues(C5CafeCommon::fHallConfigs[dbhall->settings(dbtable->hall(table))]);
    if (dbtable->specialConfig(table) > 0) {
        C5Config::setValues(C5CafeCommon::fHallConfigs[dbtable->specialConfig(table)]);
    }
    //    if (hall.count() > 0) {
    //        C5Config::setServiceFactor(C5CafeCommon::fHallConfigs[hall["f_settings"].toString().toInt()][param_service_factor]);
    //        QString menuName = C5Menu::fMenuNames[C5CafeCommon::fHallConfigs[hall["f_settings"].toString().toInt()][param_default_menu]];
    //        C5Config::setValue(param_default_menu_name, menuName);
    //    }
    DlgOrder *d = new DlgOrder(user);
    d->setWindowState(Qt::WindowFullScreen);
    QFont f(qApp->font());
    f.setBold(true);
    if (d->load(table)) {
        d->exec();
        if (!d->property("reprint").toString().isEmpty()) {
            if (__c5config.getValue(param_waiter_receipt_qty).toInt() > 0 && user->check(cp_t5_multiple_receipt)) {
                while (C5Message::question(tr("Print additional receipt for this order?")) == QDialog::Accepted) {
                    C5SocketHandler *sh = d->createSocketHandler("");
                    sh->bind("cmd", sm_printreceipt);
                    sh->bind("staffid", QString::number(user->id()));
                    sh->bind("staffname", user->fullName());
                    sh->bind("f_receiptlanguage", QString::number(C5Config::getRegValue("receipt_language").toInt()));
                    sh->bind("order", d->property("reprint").toString());
                    sh->bind("printtax", false);
                    sh->bind("receipt_printer", C5Config::fSettingsName);
                    sh->send();
                }
            }
        }
    }
    delete d;
}

void DlgOrder::openTableById(const QString &id, C5User *user)
{
    DlgOrder *d = new DlgOrder(user);
    d->setWindowState(Qt::WindowFullScreen);
    QFont f(qApp->font());
    f.setBold(true);
    d->buildMenu(d->fMenuID, 0, 0);
    d->ui->btnChangeStaff->setText(QString("%1\n%2").arg(tr("Staff")).arg(user->fullName()));
    //d->ui->btnChangeStaff->setText(QString("%1%2").arg(user.fullName()).arg(__c5config.autoDateCash() ? "" : " [" + __c5config.dateCash() + " / " + QString::number(__c5config.dateShift()) + "]"));
    WOrder *wo = new WOrder();
    if (!wo->fOrderDriver->loadData(id)) {
        C5Message::error(wo->fOrderDriver->error());
        delete wo;
        delete d;
        return;
    }
    d->fTable = wo->fOrderDriver->headerValue("f_table").toInt();
    d->ui->btnTable->setText(tr("Table") + ": " + dbtable->name(d->fTable));
    C5Config::setValues(C5CafeCommon::fHallConfigs[dbhall->settings(dbtable->hall(d->fTable))]);
    if (dbtable->specialConfig(d->fTable) > 0) {
        C5Config::setValues(C5CafeCommon::fHallConfigs[dbtable->specialConfig(d->fTable)]);
    }
    connect(wo, &WOrder::activated, d, &DlgOrder::worderActivated);
    wo->setDlg(d);
    d->ui->vs->insertWidget(d->ui->vs->count() - 1, wo);
    C5Config::setValues(C5CafeCommon::fHallConfigs[dbhall->settings(dbtable->hall(d->fTable))]);
    if (dbtable->specialConfig(d->fTable) > 0) {
        C5Config::setValues(C5CafeCommon::fHallConfigs[dbtable->specialConfig(d->fTable)]);
    }
    d->ui->btnTable->setText(tr("Table") + ": " + dbtable->name(d->fTable));
    if (wo->fOrderDriver->headerOptionsValue("f_car").toInt() > 0) {
        //setCar(worder()->fOrderDriver->carValue("f_car").toInt());
    }
    d->logRecord(user->fullName(), wo->fOrderDriver->headerValue("f_id").toString(), "", "Open order",
                 dbtable->name(d->fTable), "");
    if (wo->fOrderDriver->headerValue("f_staff").toInt() != d->fUser->id()) {
        C5Message::info(QString("%1\r\n%2").arg(tr("Order owner")).arg(dbuser->fullName(
                            wo->fOrderDriver->headerValue("f_staff").toInt())));
    }
    d->ui->vs->addStretch();
    d->exec();
    delete d;
}

void DlgOrder::accept()
{
    for (int i = 0; i < ui->vs->count(); i++) {
        WOrder *wo = dynamic_cast<WOrder *>(ui->vs->itemAt(i)->widget());
        if (wo) {
            if (wo->fOrderDriver->isEmpty()
                    && __c5config.getValue(param_waiter_donotclose_empty_order).toInt() == 0
                    && wo->fOrderDriver->headerValue("f_state").toInt() != ORDER_STATE_PREORDER_EMPTY
                    && wo->fOrderDriver->headerValue("f_state").toInt() != ORDER_STATE_PREORDER_WITH_ORDER) {
                wo->fOrderDriver->setCloseHeader();
            }
            for (int i = 0; i < wo->fOrderDriver->dishesCount(); i++) {
                if (wo->fOrderDriver->dishesValue("f_reprint", i).toInt() < 0) {
                    wo->fOrderDriver->setDishesValue("f_reprint", abs(wo->fOrderDriver->dishesValue("f_reprint", i).toInt() - 1), i);
                }
            }
            if (!wo->fOrderDriver->save()) {
                C5Message::error(wo->fOrderDriver->error());
                return;
            }
            logRecord(fUser->fullName(), wo->fOrderDriver->headerValue("f_id").toString(), "", "Close window", "", "");
        }
    }
    QString err;
    if (!dbtable->closeTable(fTable, err)) {
        C5Message::error(err);
        return;
    }
    if (!ui->btnExit->isEnabled()) {
        return;
    }
    C5Dialog::accept();
}

void DlgOrder::reject()
{
    accept();
}

WOrder *DlgOrder::worder()
{
    WOrder *o = nullptr;
    for (int i = 0; i < ui->vs->count(); i++) {
        WOrder *tmp = dynamic_cast<WOrder *>(ui->vs->itemAt(i)->widget());
        if (tmp) {
            o = tmp;
            if (o->isSelected()) {
                return o;
            }
        }
    }
    return o;
}

QList<WOrder *> DlgOrder::worders()
{
    QList<WOrder *> lst;
    for (int i = 0; i < ui->vs->count(); i++) {
        WOrder *o = dynamic_cast<WOrder *>(ui->vs->itemAt(i)->widget());
        if (o) {
            lst.append(o);
        }
    }
    return lst;
}

bool DlgOrder::load(int table)
{
    QStringList orders;
    QString err;
    if (!dbtable->openTable(table, orders, err)) {
        C5Message::error(err);
        return false;
    }
    //TODO: replace stoplist to correct place
    auto *sh = createSocketHandler(SLOT(handleStopList(QJsonObject)));
    sh->bind("cmd", sm_stoplist);
    sh->bind("state", sl_get);
    sh->send();
    fTable = table;
    fMenuID = C5Config::defaultMenu();
    buildMenu(fMenuID, 0, 0);
    on_btnRecent_clicked();
    ui->btnTable->setText(tr("Table") + ": " + dbtable->name(table));
    ui->btnChangeStaff->setText(QString("%1\n%2").arg(tr("Staff"),
                                fUser->fullName())); //.arg(__c5config.autoDateCash() ? "" : " [" + __c5config.dateCash() + " / " + QString::number(__c5config.dateShift()) + "]"));
    while (ui->vs->itemAt(0)) {
        ui->vs->itemAt(0)->widget()->deleteLater();
        ui->vs->removeItem(ui->vs->itemAt(0));
    }
    ui->lbVisit->clear();
    ui->lbVisit->setVisible(false);
    if (orders.count() == 0) {
        if (__c5config.getValue(param_waiter_donotclose_empty_order).toInt() == 1) {
            if (C5Message::question(tr("Open new order?")) != QDialog::Accepted) {
                return false;
            }
        }
        QString newid;
        WOrder *wo = new WOrder();
        connect(wo, &WOrder::activated, this, &DlgOrder::worderActivated);
        if (!wo->fOrderDriver->newOrder(fUser->id(), newid, table)) {
            C5Message::error(wo->fOrderDriver->error());
            return false;
        }
        //        if (dbtable->specialConfig(fTable) > 0) {
        //            QMap<int, QString> v = C5CafeCommon::fHallConfigs[dbhall->settings(dbtable->hall(table))];
        //            wo->fOrderDriver->setHeader("f_servicefactor", v[param_service_factor].toDouble());
        //        }
        wo->setDlg(this);
        ui->vs->insertWidget(ui->vs->count() - 1, wo);
        if (!__c5config.getValue(param_auto_discount).isEmpty()) {
            discountOrder(fUser, __c5config.getValue(param_auto_discount));
        }
        if (dbtable->hourlyId(table) > 0) {
            addDishToOrder(dbtable->hourlyId(table));
        }
        logRecord(fUser->fullName(), newid, newid, "New order", dbtable->name(table),
                  QString("%1%2").arg(wo->fOrderDriver->headerValue("f_prefix").toString()).arg(
                      wo->fOrderDriver->headerValue("f_hallid").toInt()));
    } else {
        for (const QString &oid : orders) {
            WOrder *wo = new WOrder();
            connect(wo, &WOrder::activated, this, &DlgOrder::worderActivated);
            wo->fOrderDriver->setCurrentOrderID(oid);
            wo->setDlg(this);
            ui->vs->insertWidget(ui->vs->count() - 1, wo);
            ui->btnTable->setText(tr("Table") + ": " + dbtable->name(table));
            if (wo->fOrderDriver->headerOptionsValue("f_car").toInt() > 0) {
                //setCar(worder()->fOrderDriver->carValue("f_car").toInt());
            }
            logRecord(fUser->fullName(), wo->fOrderDriver->headerValue("f_id").toString(), "", "Open order", dbtable->name(table),
                      "");
            if (wo->fOrderDriver->headerValue("f_staff").toInt() != fUser->id()) {
                //TODO: APPEND OPTION
                //C5Message::info(QString("%1\r\n%2").arg(tr("Order owner")).arg(dbuser->fullName(wo->fOrderDriver->headerValue("f_staff").toInt())));
            }
        }
    }
    ui->vs->addStretch();
    itemsToTable();
    bool isbooking = dbhall->booking(dbtable->hall(table));
    if (isbooking) {
        ui->wPreorder->setVisible(isbooking);
    }
    return true;
}

void DlgOrder::disableForCheckall(bool v)
{
    ui->wqtypanelup->setEnabled(!v);
    ui->wqtypaneldown->setVisible(v);
    ui->wpaneldown->setEnabled(!v);
    ui->wleft->setEnabled(!v);
    ui->wdish->setEnabled(!v);
    ui->wmenu->setEnabled(!v);
    WOrder *wo = worder();
    if (wo) {
        wo->setCheckMode(v);
    }
}

void DlgOrder::buildMenu(int menuid, int part1, int part2)
{
    if (menuid == 0) {
        C5Message::error(tr("Menu is not defined"));
        return;
    }
    ui->btnChangeMenu->setText(QString("%1\n%2").arg(tr("Menu"), dbmenuname->name(menuid)));
    fMenuID = menuid;
    fPart1 = part1;
    while (ui->hlDishPart1->itemAt(0)) {
        ui->hlDishPart1->itemAt(0)->widget()->deleteLater();
        ui->hlDishPart1->removeItem(ui->hlDishPart1->itemAt(0));
    }
    for (int id : menu5->fMenuList.data[menuid].keys()) {
        QPushButton *btn = new QPushButton(dbdishpart1->name(id));
        btn->setProperty("btn_part1", true);
        connect(btn, &QPushButton::clicked, this, &DlgOrder::dishpart1Clicked);
        btn->setProperty("id", id);
        ui->hlDishPart1->addWidget(btn);
    }
    if (fPart1 == 0) {
        if (!ui->hlDishPart1->itemAt(0)) {
            C5Message::error(tr("Menu is not defined"));
            return;
        }
        fPart1 = static_cast<QPushButton *>(ui->hlDishPart1->itemAt(0)->widget())->property("id").toInt();
    }
    while (ui->glDishPart2->itemAt(0)) {
        ui->glDishPart2->itemAt(0)->widget()->deleteLater();
        ui->glDishPart2->removeItem(ui->glDishPart2->itemAt(0));
    }
    int colCount = 2;
    int col = 0;
    int row = 0;
    for (const DPart2 &d2 : menu5->fMenuList.part1(fMenuID, fPart1).data) {
        QDishPart2Button *btn = new QDishPart2Button();
        connect(btn, &QDishPart2Button::clicked, this, &DlgOrder::dishPart2Clicked);
        btn->setText(dbdishpart2->name(d2.fId));
        btn->setProperty("bgcolor", dbdishpart2->color(d2.fId));
        btn->setProperty("id", d2.fId);
        ui->glDishPart2->addWidget(btn, row, col++, 1, 1);
        if (col == colCount) {
            col = 0;
            row ++;
        }
    }
    ui->glDishPart2->setRowStretch(row + 1, 1);
    buildDishes(part2, menu5->fMenuList.part1(fMenuID, fPart1).data);
}

void DlgOrder::addDishToOrder(int menuid)
{
    WOrder *wo = worder();
    int dishid = dbmenu->dishid(menuid);
    if (dbdish->isHourlyPayment(dishid)) {
        for (int i = 0; i < worder()->fOrderDriver->dishesCount(); i++) {
            if (dbdish->isHourlyPayment(worder()->fOrderDriver->dishesValue("f_id", i).toInt())
                    && worder()->fOrderDriver->dishesValue("f_state", i).toInt() == DISH_STATE_OK) {
                C5Message::error(tr("Hourly payment already exists"));
                return;
            }
        }
    }
    double price = 0;
    if (dbdish->isExtra(dishid)) {
        if (!DlgPassword::getAmount(tr("Extra price"), price)) {
            return;
        }
        if (price < 0.01) {
            C5Message::error(tr("Extra price is not defined"));
            return;
        }
    } else {
        price = dbmenu->price(menuid);
    }
    bool found = false;
    int itemindex = -1;
    QString special;
    if (!DlgListDishSpecial::getSpecial(dishid, fDBParams, special)) {
        return;
    }
    QString comment = "";
    comment = special;
    if (!found) {
        itemindex = wo->addItem(menuid, comment, price);
        logRecord(fUser->fullName(), wo->fOrderDriver->headerValue("f_id").toString(), "", "New dish",
                  dbdish->name(dishid) + " " + comment, "");
    }
    wo->fOrderDriver->setHeader("f_amountcash", 0);
    wo->fOrderDriver->setHeader("f_amountcard", 0);
    wo->fOrderDriver->setHeader("f_amountbank", 0);
    wo->fOrderDriver->setHeader("f_amountother", 0);
    wo->fOrderDriver->amountTotal();
    ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->maximum() + 100);
    setButtonsState();
    if (__c5config.getValue(param_rest_qty_before_add_dish).toInt() == 1) {
        on_btnAnyqty_clicked();
    }
}

void DlgOrder::itemsToTable()
{
    for (WOrder *o : worders()) {
        o->setDlg(this);
        o->updateDishes();
    }
    WOrder *wo = worder();
    wo->fOrderDriver->amountTotal();
    setButtonsState();
    if (!wo) {
        return;
    }
    ui->btnService->setText(QString("%1 %2%: %3")
                            .arg(tr("Service amount"))
                            .arg(worder()->fOrderDriver->headerValue("f_servicefactor").toDouble() * 100)
                            .arg(float_str(worder()->fOrderDriver->headerValue("f_amountservice").toDouble(), 2)));
    ui->btnDiscount->setText(QString("%1 %2%: %3")
                             .arg(tr("Discount"))
                             .arg(worder()->fOrderDriver->headerValue("f_discountfactor").toDouble() * 100)
                             .arg(float_str(worder()->fOrderDriver->headerValue("f_amountdiscount").toDouble(), 2)));
    ui->leTaxNumber->setText(wo->fOrderDriver->taxValue("f_receiptnumber").toString());
    ui->lePrepaidAmount->setDouble(wo->fOrderDriver->preorder("f_prepaidcash").toDouble()
                                   + wo->fOrderDriver->preorder("f_prepaidcard").toDouble()
                                   + wo->fOrderDriver->preorder("f_prepaidpayx").toDouble());
    ui->leGuest->setText(dbopreorder->guestName(wo->fOrderDriver->headerValue("f_id").toString()));
    if (ui->lePrepaidAmount->getDouble() > 0.01) {
        ui->wPreorder->setVisible(true);
    } else {
        ui->wPreorder->setVisible(false);
    }
}

void DlgOrder::setStoplistmode()
{
    fStoplistMode = !fStoplistMode;
}

bool DlgOrder::stoplistMode()
{
    return fStoplistMode;
}

void DlgOrder::viewStoplist()
{
    DlgViewStopList v;
    v.exec();
    auto *sh = createSocketHandler(SLOT(handleStopList(QJsonObject)));
    sh->bind("cmd", sm_stoplist);
    sh->bind("state", sl_get);
    sh->send();
}

void DlgOrder::logRecord(const QString &username, const QString &orderid, const QString &rec, const QString &action,
                         const QString &value1, const QString &value2)
{
    C5LogToServerThread::remember(LOG_WAITER, username, rec, orderid, "", action, value1, value2);
}

void DlgOrder::setButtonsState()
{
    bool btnSendToCooking = false;
    bool btnPayment = true;
    bool orderEmpty = true;
    QList<WOrder *> orders = worders();
    for (WOrder *o : orders) {
        for (int i = 0; i < o->fOrderDriver->dishesCount(); i++) {
            if (o->fOrderDriver->dishesValue("f_state", i).toInt() != DISH_STATE_OK) {
                continue;
            }
            orderEmpty = false;
            if (o->fOrderDriver->dishesValue("f_qty1", i).toDouble() > o->fOrderDriver->dishesValue("f_qty2", i).toDouble()) {
                btnSendToCooking = true;
            }
        }
    }
    if (btnSendToCooking || orderEmpty) {
        btnPayment = false;
    }
    ui->btnPrintService->setEnabled(btnSendToCooking);
    ui->wleft->setVisible(false);
    ui->wdish->setVisible(false);
    ui->btnTotal->setEnabled(false);
    ui->btnTotal->setProperty("stylesheet_button_redalert", false);
    ui->btnTotal->style()->polish(ui->btnTotal);
    ui->btnTotal->setText(QString("%1\n%2").arg(tr("Bill"), "-"));
    ui->wdtOrder->setEnabled(true);
    setRoomComment();
    setCLComment();
    setSelfcost();
    setDiscountComment();
    WOrder *wo = worder();
    if (!wo) {
        return;
    }
    ui->btnSit->setText(QString("%1: %2").arg(tr("Guests count"),
                        wo->fOrderDriver->headerOptionsValue("f_guests").toString()));
    headerToLineEdit();
    QString orderComment = wo->fOrderDriver->headerValue("f_comment").toString().isEmpty() ?
                           tr("Comment") : wo->fOrderDriver->headerValue("f_comment").toString();
    ui->btnOrderComment->setText(orderComment);
    auto *cbc = Configs::construct<CashboxConfig> (__c5config.dbParams(), 2);
    if (wo->fOrderDriver->headerValue("f_precheck").toInt() > 0) {
        ui->btnTotal->setProperty("stylesheet_button_redalert", true);
        ui->btnTotal->style()->polish(ui->btnTotal);
        ui->btnTotal->setText(QString("%1\n%2")
                              .arg(tr("Bill"), float_str(wo->fOrderDriver->headerValue("f_amounttotal").toDouble(), 2)));
        ui->wpayment->setVisible(fUser->check(cp_t5_print_receipt));
        ui->wdishes->setVisible(false);
        ui->wleft->setVisible(false);
        ui->wdtOrder->setEnabled(false);
        ui->btnService->setEnabled(false);
        ui->btnDiscount->setEnabled(false);
        ui->btnPaymentCash->setText(cbc->cash1Name);
        ui->btnPaymentCard->setText(cbc->cash2Name);
        ui->btnPaymentBank->setText(cbc->cash3Name);
        ui->btnPrepayment->setText(cbc->cash4Name);
        ui->btnPaymentIdram->setText(cbc->cash5Name);
        ui->btnPayX->setText(cbc->cash6Name);
        ui->btnPaymentOther->setText(cbc->cash7Name);
        headerToLineEdit();
        if (ui->wpayment->isVisible()) {
            ui->btnPayCityLedger->setEnabled(ui->leOther->getDouble() > 0.01);
            ui->btnPayComplimentary->setEnabled(ui->leOther->getDouble() > 0.01);
            ui->btnPayTransferToRoom->setEnabled(ui->leOther->getDouble() > 0.01 && cbc->cash6Hotel);
            ui->btnSelfCost->setEnabled(ui->leOther->getDouble() > 0.01);
        }
    } else {
        ui->btnTotal->setProperty("stylesheet_button_redalert", false);
        ui->btnTotal->style()->polish(ui->btnTotal);
        ui->btnTotal->setText(QString("%1\n%2").arg(tr("Bill"),
                              float_str(wo->fOrderDriver->headerValue("f_amounttotal").toDouble(), 2)));
        ui->wpayment->setVisible(false);
        ui->wdishes->setVisible(true);
        ui->wleft->setVisible(true);
    }
    ui->btnTotal->style()->polish(ui->btnTotal);
    if (wo->fOrderDriver->headerValue("f_partner").toInt() > 0) {
        CPartners p;
        C5Database db(__c5config.dbParams());
        db[":f_id"] = wo->fOrderDriver->headerValue("f_partner");
        db.exec("select * from c_partners where f_id=:f_id");
        p.getRecord(db);
        ui->btnDelivery->setText(QString("%1, %2, %3").arg(p.phone, p.taxName, p.address));
    }
    ui->btnTotal->setEnabled(btnPayment
                             || wo->fOrderDriver->headerValue("f_state").toInt() == ORDER_STATE_PREORDER_EMPTY
                             || wo->fOrderDriver->headerValue("f_state").toInt() == ORDER_STATE_PREORDER_WITH_ORDER);
    ui->wleft->setVisible(wo->fOrderDriver->headerValue("f_precheck").toInt() < 1);
    ui->wdish->setVisible(wo->fOrderDriver->headerValue("f_precheck").toInt() < 1);
}

void DlgOrder::removeWOrder(WOrder *wo)
{
    for (int i = 0; i < ui->vs->count(); i++) {
        if (wo == ui->vs->itemAt(i)->widget()) {
            ui->vs->itemAt(i)->widget()->deleteLater();
            ui->vs->removeItem(ui->vs->itemAt(i));
            return;
        }
    }
}
//void DlgOrder::setCar(int num)
//{
//    if (worder()->fOrderDriver->currentOrderId().isEmpty()) {
//        saveOrder();
//    }
//    QString govnumber;
//    int client;
//    ui->lbCar->setVisible(false);
//    C5Database db(fDBParams);
//    db[":f_id"] = num;
//    db.exec("select concat(c.f_name, ', ', bc.f_govnumber, ', ', trim(cl.f_contact)), "
//            "bc.f_govnumber, cl.f_id as f_client "
//            "from b_car bc "
//            "left join c_partners cl on cl.f_id=bc.f_costumer "
//            "left join s_car c on c.f_id=bc.f_car "
//            "where bc.f_id=:f_id");
//    if (db.nextRow()) {
//        fCarNumber = num;
//        govnumber = db.getString(1);
//        client = db.getInt("f_client");
//        ui->lbCar->setVisible(true);
//        ui->lbCar->setEnabled(true);
//        ui->lbCar->setText(QString("[%1]").arg(db.getString(0)));
//    }
//    worder()->fOrderDriver->setCar("f_car", num);

//    if (!govnumber.isEmpty() && worder()->fOrderDriver->hDouble("f_discountfactor") < 0.001) {
//        C5SocketHandler *sh = createSocketHandler(SLOT(handleDiscount(QJsonObject)));
//        sh->bind("cmd", sm_discount);
//        sh->bind("order", worder()->fOrderDriver->hString("f_id"));
//        sh->bind("code", govnumber);
//        sh->bind("handlevisit", 1);
//        sh->send();
//    }

//    int visit, client;
//    db[":f_govnumber"] = govnumber;
//    db.exec("select count(f_order), f_costumer from b_car_orders bco "
//            "inner join b_car bc on bc.f_id=bco.f_car "
//            "where bc.f_govnumber=:f_govnumber");
//    if (db.nextRow()) {
//        visit = db.getInt(0);
//        client = db.getInt(1);
//    } else {
//        o["reply"] = 0;
//        o["msg"] = tr("Invalide card code");
//        return;
//    }
//    db[":f_code"] = "auto_" + govnumber;
//    db[":f_mode"] = CARD_TYPE_AUTO_DISCOUNT;
//    db.exec("select * from b_cards_discount where f_code=:f_code");
//    if (db.nextRow()) {
//        o["card_code"] = db.getString("f_code");
//    } else {
//        o["card_code"] = "auto_" + fIn["code"].toString();
//        db[":f_mode"] = CARD_TYPE_AUTO_DISCOUNT;
//        db[":f_code"] = "auto_" + fIn["code"].toString();
//        db[":f_client"] = o["client"].toInt();
//        db[":f_value"] = 100;
//        db[":f_datestart"] = QDate::currentDate();
//        db[":f_dateend"] = QDate::currentDate().addDays(360 * 10);
//        db[":f_active"] = 1;
//        db.insert("b_cards_discount", false);
//    }
//    db[":f_code"] = "VISIT";
//    db.exec("select * from b_cards_discount where f_code=:f_code");
//    if (!db.nextRow()) {
//        o["noconfig"] = 1;
//    } else {
//        o["current"] = o["visit"].toInt() % db.getInt("f_value");
//    }
//    o["reply"] = 1;

//    if (worder()->fOrderDriver->hString("f_id").isEmpty()) {
//        C5SocketHandler *sh = createSocketHandler(SLOT(saveAndDiscount(QJsonObject)));
//        sh->bind("cmd", sm_saveorder);
//        QJsonObject o;
//        o["header"] = worder()->fOrderDriver->fHeader;
//        o["body"] = worder()->fOrderDriver->fItems;
//        o["govnumber"] = govnumber;
//        sh->send(o);
//    } else {
////        QJsonObject obj;
////        obj["saved"] = 1;
////        obj["govnumber"] = govnumber;
////        saveAndDiscount(obj);
//        C5SocketHandler *sh = createSocketHandler(SLOT(saveAndDiscount(QJsonObject)));
//        sh->bind("cmd", sm_saveorder);
//        QJsonObject o;
//        o["header"] = worder()->fOrderDriver->fHeader;
//        o["body"] = worder()->fOrderDriver->fItems;
//        o["govnumber"] = govnumber;
//        sh->send(o);
//    }
//}

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
    ui->lbTime->setText(QTime::currentTime().toString(FORMAT_TIME_TO_SHORT_STR));
    fTimerCounter++;
    if ((fTimerCounter % 60) == 0) {
        for (WOrder *o : worders()) {
            o->fOrderDriver->amountTotal();
        }
    }
}

void DlgOrder::worderActivated()
{
    WOrder *a = static_cast<WOrder *>(sender());
    WOrder *o = nullptr;
    for (int i = 0; i < ui->vs->count(); i++) {
        o = dynamic_cast<WOrder *>(ui->vs->itemAt(i)->widget());
        if (o) {
            if (o == a) {
                o->setSelected();
            } else {
                o->setSelected(false);
            }
        }
    }
    setButtonsState();
}

//void DlgOrder::handleVisit(const QJsonObject &obj)
//{
//    sender()->deleteLater();
//    if (obj["reply"].toInt() == 0) {
//        C5Message::error(obj["msg"].toString());
//        return;
//    }
//    if (obj["noconfig"].toInt() > 0) {
//        return;
//    }
//    ui->lbVisit->setVisible(true);
//    ui->lbVisit->setEnabled(true);
//    ui->lbVisit->setText(QString("[%1 / %2]").arg(obj["current"].toInt()).arg(obj["visit"].toInt()));
//    if (obj["visit"].toInt() > 0 && obj["current"].toInt() == 0 && worder()->fOrderDriver->hDouble("f_discountfactor") < 0.001) {
//        C5SocketHandler *sh = createSocketHandler(SLOT(handleDiscount(QJsonObject)));
//        sh->bind("cmd", sm_discount);
//        sh->bind("order", worder()->fOrderDriver->hString("f_id"));
//        sh->bind("code", obj["card_code"].toString());
//        sh->send();
//    }
//}

void DlgOrder::handlePrintService(const QJsonObject &obj)
{
    sender()->deleteLater();
    for (WOrder *o : worders()) {
        if (o->fOrderDriver->currentOrderId() == obj["order"].toString()) {
            o->fOrderDriver->reloadOrder();
            o->fOrderDriver->amountTotal();
        }
    }
    if (__c5config.getValue(param_waiter_closeorder_after_serviceprint).toInt() == 1) {
        on_btnExit_clicked();
        return;
    }
    itemsToTable();
}

void DlgOrder::handleReceipt(const QJsonObject &obj)
{
    ui->btnExit->setEnabled(true);
    sender()->deleteLater();
    if (obj["reply"].toInt() == 0) {
        C5Message::error(obj["msg"].toString());
        return;
    }
    for (WOrder *wo : worders()) {
        if (wo->fOrderDriver->currentOrderId() == obj["order"].toString()) {
            wo->fOrderDriver->reloadOrder();
            if (obj["close"].toInt() == 1) {
                C5LogToServerThread::remember(LOG_WAITER, fUser->fullName(), "", wo->fOrderDriver->currentOrderId(), "", "Close order",
                                              "", "");
                if (!wo->fOrderDriver->closeOrder()) {
                    C5Message::error(wo->fOrderDriver->error());
                    return;
                }
                removeWOrder(wo);
                if (!worder()) {
                    setProperty("reprint", obj["order"].toString());
                    accept();
                    return;
                }
            }
        }
    }
    if (__c5config.getValue(param_waiter_close_order_after_precheck).toInt() == 1) {
        ui->btnExit->click();
        return;
    }
    itemsToTable();
}

void DlgOrder::handleStopList(const QJsonObject &obj)
{
    sender()->deleteLater();
    if (obj["reply"].toInt() == 0) {
        C5Message::error(obj["msg"].toString());
        return;
    }
    switch (obj["state"].toInt()) {
        case sl_set:
            C5Menu::fStopList[obj["f_dish"].toString().toInt()] = obj["f_qty"].toDouble();
            break;
        case sl_add: {
            switch (obj["status"].toInt()) {
                case sl_ok: {
                    C5Menu::fStopList[obj["f_dish"].toString().toInt()] = obj["newqty"].toDouble();
                    addDishToOrder(obj["f_menu"].toString().toInt());
                    break;
                }
                case sl_not_enough_qty:
                    C5Message::error(obj["sl_msg"].toString());
                    break;
                case sl_not_in_stoplist:
                    addDishToOrder(obj["f_menu"].toString().toInt());
                    C5Menu::fStopList.remove(obj["f_dish"].toString().toInt());
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
        case sl_get:
            C5Menu::fStopList.clear();
            QJsonArray jga = obj["list"].toArray();
            for (int i = 0; i < jga.count(); i++) {
                QJsonObject jgo = jga.at(i).toObject();
                C5Menu::fStopList[jgo["dish"].toInt()] = jgo["qty"].toDouble();
            }
            break;
    }
    for (QObject *o : ui->wdishes->children()) {
        QWidget *w = dynamic_cast<QWidget *>(o);
        if (w) {
            w->repaint();
        }
    }
}

void DlgOrder::handleError(int err, const QString &msg)
{
    Q_UNUSED(err);
    sender()->deleteLater();
    C5Message::error(msg);
}

void DlgOrder::dishpart1Clicked()
{
    QPushButton *btn = static_cast<QPushButton *>(sender());
    buildMenu(fMenuID, btn->property("id").toInt(), 0);
}

void DlgOrder::processMenuID(int menuid)
{
    WOrder *wo = worder();
    if (wo) {
        if (wo->fOrderDriver->headerValue("f_precheck").toInt() > 0) {
            C5Message::error(tr("Cannot add new dish if precheck was printed"));
            return;
        }
    }
    int dishid = dbmenu->dishid(menuid);
    if (fStoplistMode) {
        if (dbdish->isExtra(dishid)) {
            C5Message::error(tr("Cannot add special dish to stoplist"));
            return;
        }
        double max = 999;
        if (!DlgPassword::getAmount(dbdish->name(dishid), max, false)) {
            return;
        }
        auto *sh = createSocketHandler(SLOT(handleStopList(QJsonObject)));
        sh->bind("cmd", sm_stoplist);
        sh->bind("f_dish", QString::number(dishid));
        sh->bind("f_stopqty", max);
        sh->bind("state", 1);
        sh->send();
    } else {
        if (dbdish->isExtra(dishid)) {
            addDishToOrder(menuid);
        } else {
            auto *sh = createSocketHandler(SLOT(handleStopList(QJsonObject)));
            sh->bind("cmd", sm_stoplist);
            sh->bind("f_dish", QString::number(dishid));
            sh->bind("f_menu", QString::number(menuid));
            sh->bind("f_qty", 1);
            sh->bind("state", sl_add);
            sh->bind("dish", QString::number(dishid));
            sh->send();
        }
        return;
    }
}

void DlgOrder::dishPart2Clicked()
{
    QDishPart2Button *btn = static_cast<QDishPart2Button *>(sender());
    int part2 = btn->property("id").toInt();
    Q_ASSERT(part2 > 0);
    buildMenu(fMenuID, fPart1, part2);
}

void DlgOrder::dishClicked()
{
    if (!worder()) {
    }
    QDishButton *btn = static_cast<QDishButton *>(sender());
    processMenuID(btn->property("id").toInt());
}

void DlgOrder::dishPartClicked()
{
    QPushButton *btn = static_cast<QPushButton *>(sender());
    int id = btn->property("id").toInt();
    buildMenu(fMenuID, fPart1, id);
}

void DlgOrder::on_btnExit_clicked()
{
    accept();
}

void DlgOrder::on_btnVoid_clicked()
{
    C5User *tmp = fUser;
    WOrder *wo = worder();
    if (!wo) {
        return;
    }
    int index = 0;
    if (!wo->currentRow(index)) {
        return;
    }
    if (wo->fOrderDriver->dishesValue("f_state", index).toInt() != DISH_STATE_OK
            && wo->fOrderDriver->dishesValue("f_state", index).toInt() != DISH_STATE_SET) {
        return;
    }
    if (wo->fOrderDriver->dishesValue("f_qty2", index).toDouble() < 0.001) {
        if (C5Message::question(QString("%1<br>%2(%3)")
                                .arg(tr("Confirm to remove"))
                                .arg(dbdish->name(wo->fOrderDriver->dishesValue("f_dish", index).toInt()))
                                .arg(float_str(wo->fOrderDriver->dishesValue("f_qty1", index).toDouble(), 2))) != QDialog::Accepted) {
            return;
        }
        restoreStoplistQty(wo->fOrderDriver->dishesValue("f_dish", index).toInt(), wo->fOrderDriver->dishesValue("f_qty1",
                           index).toDouble());
        wo->fOrderDriver->setDishesValue("f_state", DISH_STATE_NONE, index);
        wo->fOrderDriver->setDishesValue("f_removetime", QDateTime::currentDateTime(), index);
        wo->fOrderDriver->setDishesValue("f_removeuser", tmp->id(), index);
        wo->fOrderDriver->save();
        logRecord(fUser->fullName(), wo->fOrderDriver->headerValue("f_id").toString(),
                  wo->fOrderDriver->dishesValue("f_id", index).toString(),
                  "Remove not printed " + dbdish->name(wo->fOrderDriver->dishesValue("f_dish", index).toInt()),
                  "-",
                  wo->fOrderDriver->dishesValue("f_qty1", index).toString());
        wo->fOrderDriver->amountTotal();
        itemsToTable();
        return;
    }
    if (!tmp->check(cp_t5_remove_printed_service)) {
        if (!DlgPassword::getUserAndCheck(tr("Void dish"), tmp, cp_t5_remove_printed_service)) {
            return;
        }
    }
    QString oldQty = wo->fOrderDriver->dishesValue("f_qty1",
                     index).toString() + "/" + worder()->fOrderDriver->dishesValue("f_qty2", index).toString();
    double max = wo->fOrderDriver->dishesValue("f_qty1", index).toDouble();
    if (!DlgPassword::getAmount(dbdish->name(wo->fOrderDriver->dishesValue("f_dish", index).toInt()), max, true)) {
        return;
    }
    if (max < 0.01) {
        return;
    }
    QString reasonname;
    int reasonid;
    if (!DlgDishRemoveReason::getReason(reasonname, reasonid)) {
        return;
    }
    if (C5Message::question(QString("%1<br>%2(%3)")
                            .arg(tr("Confirm to remove"))
                            .arg(dbdish->name(wo->fOrderDriver->dishesValue("f_dish", index).toInt()))
                            .arg(float_str(max, 2))) != QDialog::Accepted) {
        return;
    }
    if (max < wo->fOrderDriver->dishesValue("f_qty1", index).toDouble()) {
        int nr = wo->fOrderDriver->duplicateDish(index);
        wo->fOrderDriver->setDishesValue("f_qty1", max, nr);
        if (wo->fOrderDriver->dishesValue("f_qty2", nr).toDouble() > wo->fOrderDriver->dishesValue("f_qty1", nr).toDouble()) {
            wo->fOrderDriver->setDishesValue("f_qty2", wo->fOrderDriver->dishesValue("f_qty1", nr), nr);
        }
        wo->fOrderDriver->setDishesValue("f_removetime", QDateTime::currentDateTime(), nr);
        wo->fOrderDriver->setDishesValue("f_removeuser", tmp->id(), nr);
        wo->fOrderDriver->setDishesValue("f_removereason", reasonname, nr);
        wo->fOrderDriver->setDishesValue("f_state", reasonid, nr);
        wo->fOrderDriver->setDishesValue("f_removeprecheck", wo->fOrderDriver->headerValue("f_precheck"), nr);
        wo->fOrderDriver->setDishesValue("f_qty1", wo->fOrderDriver->dishesValue("f_qty1", index).toDouble() - max, index);
        if (wo->fOrderDriver->dishesValue("f_qty2", index).toDouble() > wo->fOrderDriver->dishesValue("f_qty1",
                index).toDouble()) {
            wo->fOrderDriver->setDishesValue("f_qty2", wo->fOrderDriver->dishesValue("f_qty1", index), index);
        }
        wo->fOrderDriver->save();
        C5SocketHandler *sh = createSocketHandler(SLOT(handlePrintRemovedService(QJsonObject)));
        sh->bind("cmd", sm_print_removed_service);
        sh->bind("id", wo->fOrderDriver->dishesValue("f_id", nr).toString());
        sh->send();
    } else {
        wo->fOrderDriver->setDishesValue("f_state", reasonid, index);
        wo->fOrderDriver->setDishesValue("f_removetime", QDateTime::currentDateTime(), index);
        wo->fOrderDriver->setDishesValue("f_removeuser", tmp->id(), index);
        wo->fOrderDriver->setDishesValue("f_removereason", reasonname, index);
        wo->fOrderDriver->setDishesValue("f_removeprecheck", wo->fOrderDriver->headerValue("f_precheck"), index);
        wo->fOrderDriver->save();
        C5SocketHandler *sh = createSocketHandler(SLOT(handlePrintRemovedService(QJsonObject)));
        sh->bind("cmd", sm_print_removed_service);
        sh->bind("id", wo->fOrderDriver->dishesValue("f_id", index).toString());
        sh->send();
    }
    restoreStoplistQty(wo->fOrderDriver->dishesValue("f_dish", index).toInt(), max);
    logRecord(tmp->fullName(),
              wo->fOrderDriver->headerValue("f_id").toString(),
              wo->fOrderDriver->dishesValue("f_id", index).toString(),
              "Remove dish or change qty of " + dbdish->name(wo->fOrderDriver->dishesValue("f_dish", index).toInt()),
              oldQty, wo->fOrderDriver->dishesValue("f_qty1", index).toString() + "/" + wo->fOrderDriver->dishesValue("f_qty2",
                      index).toString());
    if (wo->fOrderDriver->headerValue("f_precheck").toInt() > 0) {
        wo->fOrderDriver->setHeader("f_precheck", wo->fOrderDriver->headerValue("f_precheck").toInt() * -1);
    }
    if (wo->fOrderDriver->headerValue("f_print").toInt() > 0) {
        wo->fOrderDriver->setHeader("f_print", wo->fOrderDriver->headerValue("f_print").toInt() * -1);
    }
    wo->fOrderDriver->amountTotal();
    itemsToTable();
    wo->fOrderDriver->save();
    //TODO: DELETE *TMP
    //TODO: OPTIMIZE THIS SAVES
}

void DlgOrder::on_btnComment_clicked()
{
    int index = 0;
    WOrder *wo = worder();
    if (!wo) {
        return;
    }
    if (!wo->currentRow(index)) {
        return;
    }
    if (wo->fOrderDriver->dishesValue("f_state", index).toInt() != DISH_STATE_OK) {
        return;
    }
    if (dbdish->isHourlyPayment(wo->fOrderDriver->dishesValue("f_dish", index).toInt())) {
        C5Message::error(tr("Cannot add comment to hourly payment"));
        return;
    }
    if (wo->fOrderDriver->dishesValue("f_qty2", index).toDouble() > 0.001) {
        C5Message::error(tr("Cannot add comment to dish that already printed"));
        return;
    }
    QString comment = wo->fOrderDriver->dishesValue("f_comment", index).toString();
    if (!DlgListOfDishComments::getComment(dbdish->name(wo->fOrderDriver->dishesValue("f_dish", index).toInt()), comment)) {
        return;
    }
    wo->fOrderDriver->setDishesValue("f_comment", comment, index);
    wo->updateItem(index);
    logRecord(fUser->fullName(), wo->fOrderDriver->headerValue("f_id").toString(),
              wo->fOrderDriver->dishesValue("f_id", index).toString(),
              "Comment for dish",
              dbdish->name(wo->fOrderDriver->dishesValue("f_dish", index).toInt()), comment);
}

void DlgOrder::on_btnChangeMenu_clicked()
{
    C5User *tmp = fUser;
    if (!tmp->check(cp_t5_change_menu)) {
        if (!DlgPassword::getUserAndCheck(tr("Change menu"), tmp, cp_t5_change_menu)) {
            return;
        }
    }
    int menuid;
    if (!DlgListOfMenu::getMenuId(menuid, fDBParams)) {
        return;
    }
    fMenuID = menuid;
    buildMenu(menuid, 0, 0);
}

void DlgOrder::on_btnSearchInMenu_clicked()
{
    DlgSearchInMenu *d = new DlgSearchInMenu();
    connect(d, SIGNAL(dish(int)), this, SLOT(processMenuID(int)));
    d->exec();
    delete d;
}

void DlgOrder::on_btnPackage_clicked()
{
    WOrder *wo = worder();
    if (!wo) {
        return;
    }
    int id;
    QString name;
    if (DlgListOfPackages::package(id, name)) {
        double max = 100;
        if (!DlgPassword::getAmount(name, max)) {
            return;
        }
        QList<int> lst = dbmenupackagelist->listOf(id);
        for (int id : lst) {
            wo->fOrderDriver->addDish2(id, max);
            DbMenuPackageList p(id);
            logRecord(fUser->fullName(), worder()->fOrderDriver->headerValue("f_id").toString(), "", "Dish in package",
                      QString("%1, %2 x %3").arg(dbdish->name(p.dish()))
                      .arg(p.qty())
                      .arg(p.price()), "");
        }
        logRecord(fUser->fullName(), worder()->fOrderDriver->headerValue("f_id").toString(), "", "New package", name, "");
    }
    itemsToTable();
}

void DlgOrder::on_btnPrintService_clicked()
{
    QList<WOrder *> orders = worders();
    for (WOrder *o : orders) {
        for (int i = 0; i < o->fOrderDriver->dishesCount(); i++) {
            if (o->fOrderDriver->dishesValue("f_qty2", i).toDouble() < 0.001) {
                o->fOrderDriver->setDishesValue("f_printtime", QDateTime::currentDateTime(), i);
            }
        }
        if (!o->fOrderDriver->save()) {
            C5Message::error(worder()->fOrderDriver->error());
            return;
        }
        ui->btnPrintService->setEnabled(false);
        C5SocketHandler *sh = createSocketHandler(SLOT(handlePrintService(QJsonObject)));
        sh->bind("cmd", sm_printservice);
        sh->bind("order", o->fOrderDriver->currentOrderId());
        sh->bind("booking", dbhall->booking(o->fOrderDriver->headerValue("f_hall").toInt()));
        sh->bind("alias", __c5config.getValue(param_force_use_print_alias).toInt());
        sh->send();
        logRecord(fUser->fullName(), o->fOrderDriver->headerValue("f_id").toString(), "", "Send to cooking", "", "");
    }
}

void DlgOrder::on_btnSit_clicked()
{
    DlgGuests(fDBParams, worder()->fOrderDriver).exec();
    itemsToTable();
}

void DlgOrder::on_btnMovement_clicked()
{
    C5User *tmp = fUser;
    if (!tmp->check(cp_t5_movetable)) {
        if (!DlgPassword::getUserAndCheck(tr("Move items"), tmp, cp_t5_movetable)) {
            return;
        }
    }
    DlgSplitOrder d(tmp);
    d.configOrder(worder()->fOrderDriver->currentOrderId());
    d.exec();
    for (WOrder *wo : worders()) {
        wo->fOrderDriver->reloadOrder();
    }
    itemsToTable();
}

void DlgOrder::on_btnRecent_clicked()
{
    if (fMenuID == 0) {
        return;
    }
    while (ui->grDish->itemAt(0)) {
        ui->grDish->itemAt(0)->widget()->deleteLater();
        ui->grDish->removeItem(ui->grDish->itemAt(0));
    }
    QRect scr = qApp->desktop()->screenGeometry();
    int dcolCount = scr.width() > 1024 ? 3 : 2;
    int dcol = 0;
    int drow = 0;
    for (DPart1 p1 : menu5->fMenuList.data[fMenuID]) {
        QList<DPart2> dpart2 = p1.data;
        for (const DPart2 &d2 : dpart2) {
            fetchDishes(d2, true, dcolCount, dcol, drow);
        }
    }
    ui->grDish->setRowStretch(drow + 1, 1);
}

void DlgOrder::on_btnTable_clicked()
{
    WOrder *wo = worder();
    if (!wo) {
        return;
    }
    if (wo->fOrderDriver->currentOrderId().isEmpty()) {
        return;
    }
    C5User *tmp = fUser;
    if (!tmp->check(cp_t5_movetable)) {
        if (!DlgPassword::getUserAndCheck(tr("Move table"), tmp, cp_t5_movetable)) {
            return;
        }
    }
    QString err;
    int tableId;
    if (!DlgFace::getTable(tableId, __c5config.defaultHall(), tmp)) {
        return;
    }
    if (tableId == fTable) {
        C5Message::error(tr("Same table"));
        return;
    }
    QStringList orders;
    if (!dbtable->openTable(tableId, orders, err)) {
        C5Message::error(err);
        return;
    }
    if (!dbtable->closeTable(fTable, err)) {
        C5Message::error(err);
        return;
    }
    if (wo->fOrderDriver->headerValue("f_state").toInt() == ORDER_STATE_PREORDER_EMPTY
            || wo->fOrderDriver->headerValue("f_state").toInt() == ORDER_STATE_PREORDER_WITH_ORDER) {
        wo->fOrderDriver->setHeader("f_state", ORDER_STATE_OPEN);
        wo->fOrderDriver->save();
    }
    C5Database db(__c5config.dbParams());
    if (orders.isEmpty()) {
        db[":f_table"] = tableId;
        db[":f_hall"] = dbtable->hall(tableId);
        db.update("o_header", "f_id", wo->fOrderDriver->currentOrderId());
        for (int i = 0; i < wo->fOrderDriver->dishesCount(); i++) {
            wo->fOrderDriver->setDishesValue("f_fromtable", fTable, i);
        }
        wo->fOrderDriver->save();
    } else {
        int o = DlgChoseSplitOrderOption::getOption();
        switch (o) {
            case JOIN_CANCEL:
                load(fTable);
                return;
            case JOIN_JOIN:
                db[":f_header"] = orders.at(0);
                db[":f_oldheader"] = wo->fOrderDriver->currentOrderId();
                db[":f_fromtable"] = fTable;
                db.exec("update o_body set f_header=:f_header, f_fromtable=:f_fromtable where f_header=:f_oldheader");
                wo->fOrderDriver->setHeader("f_state", ORDER_STATE_MOVED);
                wo->fOrderDriver->closeOrder();
                break;
            case JOIN_APPEND:
                wo->fOrderDriver->setHeader("f_table", tableId);
                wo->fOrderDriver->setHeader("f_hall", dbtable->hall(tableId));
                for (int i = 0; i < wo->fOrderDriver->dishesCount(); i++) {
                    wo->fOrderDriver->setDishesValue("f_fromtable", fTable, i);
                }
                wo->fOrderDriver->save();
                break;
        }
    }
    removeWOrder(wo);
    for (WOrder *w : worders()) {
        w->fOrderDriver->save();
    }
    load(tableId);
    //TODO: LOG THIS MOVEMETN
}

void DlgOrder::on_btnChangeStaff_clicked()
{
    WOrder *wo = worder();
    if (!wo) {
        return;
    }
    C5User *tmp = fUser;
    if (!tmp->check(cp_t5_change_staff_of_table)) {
        if (!DlgPassword::getUserAndCheck(tr("Change staff of order"), tmp, cp_t5_change_staff_of_table)) {
            return;
        }
    }
    QString code;
    if (!DlgPassword::getPassword(tr("Change staff"), code)) {
        return;
    }
    //QString code = QInputDialog::getText(this, tr("Change staff"), tr("Staff password"), QLineEdit::Password, "", &ok);
    //    if (!ok) {
    //        return;
    //    }
    code = code.replace("?", "");
    code = code.replace(";", "");
    code = code.replace(":", "");
    if (code.isEmpty()) {
        return;
    }
    C5User u(code);
    if (!u.isValid()) {
        C5Message::error(u.error());
        return;
    }
    wo->fOrderDriver->setHeader("f_staff", u.id());
    ui->btnChangeStaff->setText(QString("%1\n%2").arg(tr("Staff")).arg(u.fullName()));
}

void DlgOrder::on_btnScrollUp_clicked()
{
    ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->value() + 300);
}

void DlgOrder::on_btnScrollDown_clicked()
{
    ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->value() - 300);
}

void DlgOrder::on_btnDiscount_clicked()
{
    //TODO: FIX FULL LOG
    WOrder *wo = worder();
    if (!wo) {
        return;
    }
    C5User *tmp = fUser;
    C5Database db(__c5config.dbParams());
    if (wo->fOrderDriver->headerValue("f_discountfactor").toDouble() > 0.001) {
        if (!tmp->check(cp_t5_cancel_discount)) {
            if (!DlgPassword::getUserAndCheck(tr("Remove discount"), tmp, cp_t5_cancel_discount)) {
                return;
            }
        }
        db[":f_id"] = wo->fOrderDriver->currentOrderId();
        db.exec("delete from b_history where f_id=:f_id");
        wo->fOrderDriver->setHeader("f_discountfactor", 0);
        for (int i = 0; i < wo->fOrderDriver->dishesCount(); i++) {
            wo->fOrderDriver->setDishesValue("f_discount", wo->fOrderDriver->headerValue("f_discountfactor"), i);
        }
        wo->fOrderDriver->amountTotal();
        itemsToTable();
        wo->fOrderDriver->save();
        logRecord(tmp->fullName(), wo->fOrderDriver->currentOrderId(), "", "Discount removed", "", "");
        return;
    }
    bool ok;
    QString code = QInputDialog::getText(this, tr("Card"), tr("Card code"), QLineEdit::Password, "", &ok);
    if (!ok) {
        return;
    }
    code = code.replace("?", "");
    code = code.replace(";", "");
    code = code.replace(":", "");
    if (code.isEmpty()) {
        C5Message::error(tr("Card code is empty"));
        return;
    }
    discountOrder(tmp, code);
}

void DlgOrder::on_btnTotal_clicked()
{
    WOrder *wo = worder();
    if (wo->fOrderDriver->headerValue("f_precheck").toInt() < 1) {
        auto *tmp = fUser;
        if (!tmp->check(cp_t5_print_precheck)) {
            if (!DlgPassword::getUserAndCheck(tr("Print precheck"), tmp, cp_t5_print_precheck)) {
                return;
            }
        }
        int withoutprint = 0;
        if (__c5config.getValue(param_waiter_ask_for_precheck).toInt() > 0) {
            withoutprint = DlgAskForPrecheck::get();
        }
        bool empty = true;
        for (int i = 0; i < wo->fOrderDriver->dishesCount(); i++) {
            if (wo->fOrderDriver->dishesValue("f_state", i).toInt() != DISH_STATE_OK) {
                continue;
            }
            if (wo->fOrderDriver->dishesValue("f_qty1", i).toDouble() > wo->fOrderDriver->dishesValue("f_qty2", i).toDouble()
                    && !( wo->fOrderDriver->headerValue("f_state").toInt() == ORDER_STATE_PREORDER_EMPTY
                          || wo->fOrderDriver->headerValue("f_state").toInt() == ORDER_STATE_PREORDER_WITH_ORDER)) {
                C5Message::error(tr("Order is incomplete"));
                return;
            }
            empty = false;
        }
        if (empty) {
            if (tmp != fUser) {
                delete tmp;
            }
            C5Message::error(tr("Order is incomplete"));
            return;
        }
        wo->fOrderDriver->amountTotal();
        wo->fOrderDriver->save();
        C5SocketHandler *sh = createSocketHandler(SLOT(handleReceipt(QJsonObject)));
        sh->bind("cmd", sm_bill);
        sh->bind("station", hostinfo);
        sh->bind("printer", C5Config::localReceiptPrinter());
        sh->bind("order", worder()->fOrderDriver->currentOrderId());
        sh->bind("language", C5Config::getRegValue("receipt_language").toInt());
        sh->bind("receipt_printer", C5Config::fSettingsName);
        sh->bind("withoutprint", withoutprint);
        sh->bind("alias", __c5config.getValue(param_force_use_print_alias).toInt());
        sh->send();
        C5LogToServerThread::remember(LOG_WAITER, tmp->fullName(), "", wo->fOrderDriver->currentOrderId(), "", "Precheck",
                                      float_str(wo->fOrderDriver->headerValue("f_amounttotal").toDouble(), 2), "");
        if (tmp != fUser) {
            delete tmp;
        }
    } else {
        int o = DlgPrecheckOptions::precheck();
        C5User *tmp = fUser;
        switch (o) {
            case PRECHECK_CANCEL: {
                if (!tmp->check(cp_t5_cancel_precheck)) {
                    if (!DlgPassword::getUserAndCheck(tr("Cancel precheck"), tmp, cp_t5_cancel_precheck)) {
                        return;
                    }
                }
                if (C5Message::question(tr("Confirm to cancel bill")) != QDialog::Accepted) {
                    return;
                }
                C5Database db(__c5config.dbParams());
                db[":f_id"] = wo->fOrderDriver->currentOrderId();
                db[":f_precheck"] = abs(wo->fOrderDriver->headerValue("f_precheck").toInt()) * -1;
                db.exec("update o_header set f_precheck=:f_precheck where f_id=:f_id");
                wo->fOrderDriver->reloadOrder();
                setButtonsState();
                C5LogToServerThread::remember(LOG_WAITER, fUser->fullName(), "", wo->fOrderDriver->currentOrderId(), "",
                                              "Cancel precheck", "", "");
                break;
            }
            case PRECHECK_REPEAT:
                if (!tmp->check(cp_t5_repeat_precheck)) {
                    if (!DlgPassword::getUserAndCheck(tr("Repeat precheck"), tmp, cp_t5_print_precheck)) {
                        return;
                    }
                }
                C5SocketHandler *sh = createSocketHandler(SLOT(handleReceipt(QJsonObject)));
                sh->bind("cmd", sm_bill);
                sh->bind("station", hostinfo);
                sh->bind("printer", C5Config::localReceiptPrinter());
                sh->bind("order", worder()->fOrderDriver->currentOrderId());
                sh->bind("language", C5Config::getRegValue("receipt_language").toInt());
                sh->bind("receipt_printer", C5Config::fSettingsName);
                sh->send();
                C5LogToServerThread::remember(LOG_WAITER, fUser->fullName(), "", wo->fOrderDriver->currentOrderId(), "",
                                              "Repeat precheck", float_str(wo->fOrderDriver->headerValue("f_amounttotal").toDouble(), 2), "");
                break;
        }
        if (tmp != fUser) {
            delete tmp;
        }
    }
}

void DlgOrder::on_btnReceiptLanguage_clicked()
{
    int r = DlgReceiptLanguage::receipLanguage();
    if (r > -1) {
        C5Config::setRegValue("receipt_language", r);
        setLangIcon();
    }
}

void DlgOrder::on_btnCalcCash_clicked()
{
    calcAmount(ui->leCash);
}

void DlgOrder::setLangIcon()
{
    switch (C5Config::getRegValue("receipt_language").toInt()) {
        case 0:
            ui->btnReceiptLanguage->setIcon(QIcon(":/armenia.png"));
            C5Config::setRegValue("receipt_language", LANG_AM);
            break;
        case 1:
            ui->btnReceiptLanguage->setIcon(QIcon(":/usa.png"));
            C5Config::setRegValue("receipt_language", LANG_EN);
            break;
        case 2:
            ui->btnReceiptLanguage->setIcon(QIcon(":/russia.png"));
            C5Config::setRegValue("receipt_language", LANG_RU);
            break;
    }
}

void DlgOrder::calcAmount(C5LineEdit *l)
{
    WOrder *wo = worder();
    double max = wo->fOrderDriver->headerValue("f_amounttotal").toDouble();
    if (!DlgPassword::getAmount(l->property("pay").toString(), max)) {
        return;
    }
    l->setDouble(max);
    lineEditToHeader();
}

void DlgOrder::lineEditToHeader()
{
    auto *wo = worder();
    if (!wo) {
        return;
    }
    wo->fOrderDriver->setHeader("f_amountcash", ui->leCash->getDouble());
    wo->fOrderDriver->setHeader("f_amountcard", ui->leCard->getDouble());
    wo->fOrderDriver->setHeader("f_amountbank", ui->leBank->getDouble());
    wo->fOrderDriver->setHeader("f_amountprepaid", ui->lePrepaid->getDouble());
    wo->fOrderDriver->setHeader("f_amountother", ui->leOther->getDouble());
    wo->fOrderDriver->setHeader("f_amountidram", ui->leIDRAM->getDouble());
    wo->fOrderDriver->setHeader("f_amountpayx", ui->lePayX->getDouble());
    ui->leRemain->setDouble(wo->fOrderDriver->headerValue("f_amounttotal").toDouble()
                            - ui->leCash->getDouble()
                            - ui->leCard->getDouble()
                            - ui->leBank->getDouble()
                            - ui->lePrepaid->getDouble()
                            - ui->leOther->getDouble()
                            - ui->leIDRAM->getDouble()
                            - ui->lePayX->getDouble());
}

void DlgOrder::headerToLineEdit()
{
    WOrder *wo = worder();
    ui->leCash->setDouble(wo->fOrderDriver->headerValue("f_amountcash").toDouble());
    ui->leCard->setDouble(wo->fOrderDriver->headerValue("f_amountcard").toDouble());
    ui->leBank->setDouble(wo->fOrderDriver->headerValue("f_amountbank").toDouble());
    ui->lePrepaid->setDouble(wo->fOrderDriver->headerValue("f_amountprepaid").toDouble());
    ui->leOther->setDouble(wo->fOrderDriver->headerValue("f_amountother").toDouble() +
                           wo->fOrderDriver->headerValue("f_hotel").toDouble());
    ui->leIDRAM->setDouble(wo->fOrderDriver->headerValue("f_amountidram").toDouble());
    ui->lePayX->setDouble(wo->fOrderDriver->headerValue("f_amountpayx").toDouble());
    if (wo->fOrderDriver->preorder("f_prepaidcash").toDouble() > 0) {
        if (wo->fOrderDriver->headerValue("f_amountcash").toDouble() < 1) {
            wo->fOrderDriver->setHeader("f_amountcash",
                                        wo->fOrderDriver->preorder("f_prepaidcash").toDouble() > wo->fOrderDriver->headerValue("f_amounttotal").toDouble()
                                        ? wo->fOrderDriver->headerValue("f_amounttotal").toDouble()
                                        : wo->fOrderDriver->preorder("f_prepaidcash").toDouble());
            ui->leCash->setDouble(wo->fOrderDriver->headerValue("f_amountcash").toDouble());
        }
    }
    if (wo->fOrderDriver->preorder("f_prepaidcard").toDouble() > 0) {
        if (wo->fOrderDriver->headerValue("f_amountcard").toDouble() < 1) {
            wo->fOrderDriver->setHeader("f_amountcard",
                                        wo->fOrderDriver->preorder("f_prepaidcard").toDouble() > wo->fOrderDriver->headerValue("f_amounttotal").toDouble()
                                        ? wo->fOrderDriver->headerValue("f_amounttotal").toDouble()
                                        : wo->fOrderDriver->preorder("f_prepaidcard").toDouble());
            ui->leCard->setDouble(wo->fOrderDriver->headerValue("f_amountcard").toDouble());
        }
    }
    if (wo->fOrderDriver->preorder("f_prepaidpayx").toDouble() > 0) {
        if (wo->fOrderDriver->headerValue("f_amountpayx").toDouble() < 1) {
            wo->fOrderDriver->setHeader("f_amountpayx",
                                        wo->fOrderDriver->preorder("f_prepaidpayx").toDouble() > wo->fOrderDriver->headerValue("f_amounttotal").toDouble()
                                        ? wo->fOrderDriver->headerValue("f_amounttotal").toDouble()
                                        : wo->fOrderDriver->preorder("f_prepaidpayx").toDouble());
            ui->lePayX->setDouble(wo->fOrderDriver->headerValue("f_amountpayx").toDouble());
        }
    }
    ui->leRemain->setDouble(wo->fOrderDriver->headerValue("f_amounttotal").toDouble()
                            - ui->leCash->getDouble()
                            - ui->leCard->getDouble()
                            - ui->lePrepaid->getDouble()
                            - ui->leBank->getDouble()
                            - ui->leIDRAM->getDouble()
                            - ui->lePayX->getDouble()
                            - ui->leOther->getDouble());
    ui->leReceived->clear();
    ui->leChange->clear();
    if (wo->fOrderDriver->headerValue("f_cash").toDouble() > 0.01) {
        wo->fOrderDriver->setHeader("f_change",
                                    wo->fOrderDriver->headerValue("f_cash").toDouble() - wo->fOrderDriver->headerValue("f_amounttotal").toDouble());
        ui->leReceived->setDouble(wo->fOrderDriver->headerValue("f_cash").toDouble());
        ui->leChange->setDouble(wo->fOrderDriver->headerValue("f_change").toDouble());
    }
}

void DlgOrder::clearOther()
{
    WOrder *wo = worder();
    wo->fOrderDriver->setRoom("", "", "", "");
    wo->fOrderDriver->setCL("", "");
    ui->leRoomComment->clear();
    ui->leRoomComment->setVisible(false);
    ui->lbRoom->setVisible(false);
}

void DlgOrder::setCLComment()
{
    if (!worder()) {
        return;
    }
    bool v = !worder()->fOrderDriver->clValue("f_code").toString().isEmpty();
    ui->leRoomComment->setVisible(v);
    ui->lbRoom->setVisible(v);
    if (v) {
        ui->leRoomComment->setText(worder()->fOrderDriver->clValue("f_code").toString() + ", " +
                                   worder()->fOrderDriver->clValue("f_name").toString());
    }
}

void DlgOrder::setComplimentary()
{
    if (!worder()) {
        return;
    }
    if (worder()->fOrderDriver->headerValue("f_otherid").toInt() == PAYOTHER_COMPLIMENTARY) {
        ui->leRoomComment->setVisible(true);
        ui->leRoomComment->setText(tr("Complimentary"));
        ui->lbRoom->setVisible(true);
    }
}

void DlgOrder::setRoomComment()
{
    if (WOrder *wo = worder()) {
        bool v = !wo->fOrderDriver->payRoomValue("f_room").toString().isEmpty();
        ui->leRoomComment->setVisible(v);
        ui->lbRoom->setVisible(v);
        if (v) {
            ui->leRoomComment->setText(wo->fOrderDriver->payRoomValue("f_room").toString() + ", " +
                                       wo->fOrderDriver->payRoomValue("f_guest").toString());
        }
    }
}

void DlgOrder::setSelfcost()
{
    WOrder *w = worder();
    if (!w) {
        return;
    }
    if (w->fOrderDriver->headerValue("f_otherid").toInt() == PAYOTHER_PRIMECOST) {
        ui->leRoomComment->setVisible(true);
        ui->lbRoom->setVisible(true);
        ui->leRoomComment->setText(tr("Prime cost"));
    }
}

void DlgOrder::setDiscountComment()
{
    if (!worder()) {
        return;
    }
    ui->leDiscountComment->setVisible(false);
    ui->lbDiscount->setVisible(false);
    //TODO: DISCOUTN COMMENT
    //    bool v = (fOrder->hInt("f_bonusid") > 0);
    //    ui->leDiscountComment->setVisible(v);
    //    if (v) {
    //        ui->leDiscountComment->setText(QString("%1, N%2, %3%, %4")
    //                                       .arg(fOrder->hString("f_bonusname"))
    //                                       .arg(fOrder->hString("f_bonusid"))
    //                                       .arg(fOrder->hDouble("f_bonusvalue"))
    //                                       .arg(fOrder->hString("f_bonusholder")));
    //    }
}

bool DlgOrder::worderPaymentOK()
{
    if (!worder()) {
        return false;
    }
    if (ui->leCash->getDouble()
            + ui->leCard->getDouble()
            + ui->leBank->getDouble()
            + ui->lePrepaid->getDouble()
            + ui->leOther->getDouble() < worder()->fOrderDriver->headerValue("f_amounttotal").toDouble()) {
        return false;
    }
    return true;
}

bool DlgOrder::buildDishes(int part2, const QList<DPart2> &dpart2)
{
    while (ui->grDish->itemAt(0)) {
        ui->grDish->itemAt(0)->widget()->deleteLater();
        ui->grDish->removeItem(ui->grDish->itemAt(0));
    }
    for (const DPart2 &d2 : dpart2) {
        if (d2.data1.count() > 0) {
            if (buildDishes(part2, d2.data1)) {
                return true;
            }
        }
        // C5Message::info(QString("%1-%2").arg(part2).arg(d2.fId));
        QRect scr = qApp->desktop()->screenGeometry();
        if (d2.fId == part2) {
            int dcolCount = scr.width() > 1024 ? 3 : 2;
            int dcol = 0;
            int drow = 0;
            for (int i = 0; i < d2.data1.count(); i++) {
                QDishPart2Button *btn = new QDishPart2Button();
                btn->setProperty("bgcolor", dbdishpart2->color(d2.fId));
                btn->setProperty("id", d2.data1.at(i).fId);
                btn->setText(dbdishpart2->name(d2.data1.at(i).fId));
                connect(btn, &QDishPart2Button::clicked, this, &DlgOrder::dishPartClicked);
                ui->grDish->addWidget(btn, drow, dcol++, 1, 1);
                if (dcol == dcolCount) {
                    dcol = 0;
                    drow ++;
                }
            }
            for (int i = 0; i < d2.data2.data.count(); i++) {
                int w = (ui->wdishes->width() - 10)  / (scr.width() > 1024 ? 3 : 2 );
                QDishButton *btn = new QDishButton(w);
                btn->setProperty("id", d2.data2.data.at(i));
                connect(btn, &QDishButton::clicked, this, &DlgOrder::dishClicked);
                ui->grDish->addWidget(btn, drow, dcol++, 1, 1);
                if (dcol == dcolCount) {
                    dcol = 0;
                    drow ++;
                }
            }
            ui->grDish->setRowStretch(drow + 1, 1);
            return true;
        }
    }
    return false;
}

bool DlgOrder::fetchDishes(const DPart2 &part2, bool recent, int colcount, int &col, int &row)
{
    for (int i = 0; i < part2.data1.count(); i++) {
        fetchDishes(part2.data1[i], recent, colcount, col, row);
    }
    for (int i = 0; i < part2.data2.data.count(); i++) {
        if (recent) {
            if (!dbmenu->recent(part2.data2.data.at(i))) {
                continue;
            }
        }
        QRect scr = qApp->desktop()->screenGeometry();
        int w = ui->wdishes->width()  / (scr.width() > 1024 ? 3 : 2 );
        QDishButton *btn = new QDishButton(w);
        btn->setProperty("id", part2.data2.data.at(i));
        connect(btn, &QDishButton::clicked, this, &DlgOrder::dishClicked);
        ui->grDish->addWidget(btn, row, col++, 1, 1);
        if (col == colcount) {
            col = 0;
            row ++;
        }
    }
    return true;
}

void DlgOrder::discountOrder(C5User *u, const QString &code)
{
    WOrder *wo = worder();
    if (!wo) {
        return;
    }
    C5Database db(__c5config.dbParams());
    db[":f_id"] = worder()->fOrderDriver->currentOrderId();
    db.exec("select * from b_history where f_id=:f_id");
    if (db.nextRow()) {
        C5Message::error(tr("Bonus system alreay exists for this order"));
        return;
    }
    db[":f_code"] = code;
    db.exec("select c.f_id, c.f_value, c.f_mode, cn.f_name, p.f_contact "
            "from b_cards_discount c "
            "left join c_partners p on p.f_id=c.f_client "
            "left join b_card_types cn on cn.f_id=c.f_mode "
            "where f_code=:f_code");
    if (db.nextRow()) {
        switch (db.getInt("f_mode")) {
            case CARD_TYPE_DISCOUNT:
                wo->fOrderDriver->setHeader("f_discountfactor", db.getDouble("f_value") / 100.0);
                for (int i = 0; i < wo->fOrderDriver->dishesCount(); i++) {
                    wo->fOrderDriver->setDishesValue("f_discount", wo->fOrderDriver->headerValue("f_discountfactor"), i);
                }
                break;
            case CARD_TYPE_ACCUMULATIVE:
                break;
        }
        db[":f_type"] = db.getInt("f_mode");
        db[":f_value"] = db.getDouble("f_value");
        db[":f_card"] = db.getInt("f_id");
        db[":f_data"] = 0;
        db[":f_id"] = wo->fOrderDriver->currentOrderId();
        db.insert("b_history");
        wo->fOrderDriver->amountTotal();
        itemsToTable();
        wo->fOrderDriver->save();
        logRecord(u->fullName(), wo->fOrderDriver->currentOrderId(), "", "Discount", "", "");
    } else {
        C5Message::error(tr("Cannot find card"));
        return;
    }
}

void DlgOrder::on_btnCalcCard_clicked()
{
    calcAmount(ui->leCard);
}

void DlgOrder::on_btnCalcBank_clicked()
{
    calcAmount(ui->leBank);
}

void DlgOrder::on_btnCalcPrepaid_clicked()
{
    calcAmount(ui->lePrepaid);
}

void DlgOrder::on_btnCalcOther_clicked()
{
    calcAmount(ui->leOther);
}

void DlgOrder::on_btnPaymentCash_clicked()
{
    worder()->fOrderDriver->setHeader("f_amountcash", worder()->fOrderDriver->headerValue("f_amounttotal"));
    worder()->fOrderDriver->setHeader("f_amountcard", 0);
    worder()->fOrderDriver->setHeader("f_amountbank", 0);
    worder()->fOrderDriver->setHeader("f_amountprepaid", 0);
    worder()->fOrderDriver->setHeader("f_amountother", 0);
    worder()->fOrderDriver->setHeader("f_amountpayx", 0);
    worder()->fOrderDriver->setHeader("f_amountidram", 0);
    worder()->fOrderDriver->setHeader("f_hotel", 0);
    worder()->fOrderDriver->setHeader("f_otherid", 0);
    ui->leRemain->setDouble(0);
    clearOther();
    headerToLineEdit();
    setButtonsState();
}

void DlgOrder::on_btnPaymentCard_clicked()
{
    ui->btnTax->setChecked(true);
    worder()->fOrderDriver->setHeader("f_amountcash", 0);
    worder()->fOrderDriver->setHeader("f_amountcard", worder()->fOrderDriver->headerValue("f_amounttotal"));
    worder()->fOrderDriver->setHeader("f_amountbank", 0);
    worder()->fOrderDriver->setHeader("f_amountprepaid", 0);
    worder()->fOrderDriver->setHeader("f_amountother", 0);
    worder()->fOrderDriver->setHeader("f_amountpayx", 0);
    worder()->fOrderDriver->setHeader("f_amountidram", 0);
    worder()->fOrderDriver->setHeader("f_hotel", 0);
    worder()->fOrderDriver->setHeader("f_otherid", 0);
    ui->leRemain->setDouble(0);
    clearOther();
    headerToLineEdit();
    setButtonsState();
}

void DlgOrder::on_btnPaymentBank_clicked()
{
    ui->btnTax->setChecked(false);
    worder()->fOrderDriver->setHeader("f_amountcash", 0);
    worder()->fOrderDriver->setHeader("f_amountcard", 0);
    worder()->fOrderDriver->setHeader("f_amountbank", worder()->fOrderDriver->headerValue("f_amounttotal"));
    worder()->fOrderDriver->setHeader("f_amountprepaid", 0);
    worder()->fOrderDriver->setHeader("f_amountother", 0);
    worder()->fOrderDriver->setHeader("f_amountpayx", 0);
    worder()->fOrderDriver->setHeader("f_amountidram", 0);
    worder()->fOrderDriver->setHeader("f_otherid", 0);
    worder()->fOrderDriver->setHeader("f_hotel", 0);
    ui->leRemain->setDouble(0);
    clearOther();
    headerToLineEdit();
    setButtonsState();
}

void DlgOrder::on_btnPrepayment_clicked()
{
    ui->btnTax->setChecked(true);
    worder()->fOrderDriver->setHeader("f_amountcash", 0);
    worder()->fOrderDriver->setHeader("f_amountcard", 0);
    worder()->fOrderDriver->setHeader("f_amountbank", 0);
    worder()->fOrderDriver->setHeader("f_amountprepaid", worder()->fOrderDriver->headerValue("f_amounttotal"));
    worder()->fOrderDriver->setHeader("f_amountother", 0);
    worder()->fOrderDriver->setHeader("f_amountpayx", 0);
    worder()->fOrderDriver->setHeader("f_amountidram", 0);
    worder()->fOrderDriver->setHeader("f_otherid", 0);
    worder()->fOrderDriver->setHeader("f_hotel", 0);
    ui->leRemain->setDouble(0);
    clearOther();
    headerToLineEdit();
    setButtonsState();
}

void DlgOrder::on_btnPaymentOther_clicked()
{
    ui->btnTax->setChecked(false);
    worder()->fOrderDriver->setHeader("f_amountcash", 0);
    worder()->fOrderDriver->setHeader("f_amountcard", 0);
    worder()->fOrderDriver->setHeader("f_amountbank", 0);
    worder()->fOrderDriver->setHeader("f_amountprepaid", 0);
    worder()->fOrderDriver->setHeader("f_amountidram", 0);
    worder()->fOrderDriver->setHeader("f_hotel", 0);
    worder()->fOrderDriver->setHeader("f_amountother", worder()->fOrderDriver->headerValue("f_amounttotal"));
    ui->leRemain->setDouble(0);
    headerToLineEdit();
    setButtonsState();
}

void DlgOrder::on_btnPayCityLedger_clicked()
{
    QString clCode, clName;
    if (DlgCL::getCL(clCode, clName)) {
        worder()->fOrderDriver->setHeader("f_otherid", PAYOTHER_CL);
        worder()->fOrderDriver->setCL(clCode, clName);
        setCLComment();
    }
}

void DlgOrder::on_btnPayComplimentary_clicked()
{
    clearOther();
    worder()->fOrderDriver->setHeader("f_otherid", PAYOTHER_COMPLIMENTARY);
    setComplimentary();
}

void DlgOrder::on_btnPayTransferToRoom_clicked()
{
    if (WOrder *wo = worder()) {
        QString res, inv, room, guest;
        if (DlgGuest::getGuest(res, inv, room, guest)) {
            if (!wo->fOrderDriver->setRoom(res, inv, room, guest)) {
                C5Message::error(wo->fOrderDriver->error());
                return;
            }
            wo->fOrderDriver->setHeader("f_amountother", 0);
            wo->fOrderDriver->setHeader("f_hotel", wo->fOrderDriver->amountTotal());
            setRoomComment();
        }
    }
}

void DlgOrder::on_btnReceipt_clicked()
{
    WOrder *wo = worder();
    if (!wo) {
        return;
    }
    C5User *tmp = fUser;
    if (!tmp->check(cp_t5_print_receipt)) {
        if (!DlgPassword::getUserAndCheck(tr("Print receipt"), tmp, cp_t5_print_receipt)) {
            return;
        }
    }
    if (C5Message::question(tr("Confirm to close order")) != QDialog::Accepted) {
        return;
    }
    if (wo->fOrderDriver->headerValue("f_amountother").toDouble() > 0.001) {
        if (wo->fOrderDriver->headerValue("f_otherid").toInt() == 0) {
            C5Message::error(tr("Other method is not selected"));
            return;
        }
    }
    if (wo->fOrderDriver->headerValue("f_amountcash").toDouble()
            + wo->fOrderDriver->headerValue("f_amountcard").toDouble()
            + wo->fOrderDriver->headerValue("f_amountother").toDouble()
            + wo->fOrderDriver->headerValue("f_amountprepaid").toDouble()
            + wo->fOrderDriver->headerValue("f_amountbank").toDouble()
            + wo->fOrderDriver->headerValue("f_amountidram").toDouble()
            + wo->fOrderDriver->headerValue("f_hotel").toDouble()
            + wo->fOrderDriver->headerValue("f_amountpayx").toDouble() <
            wo->fOrderDriver->headerValue("f_amounttotal").toDouble()) {
        C5Message::error(tr("Check the all payment methods"));
        return;
    }
    double a = wo->fOrderDriver->headerValue("f_amountcash").toDouble()
               + wo->fOrderDriver->headerValue("f_amountcard").toDouble()
               + wo->fOrderDriver->headerValue("f_amountother").toDouble()
               + wo->fOrderDriver->headerValue("f_hotel").toDouble()
               + wo->fOrderDriver->headerValue("f_amountprepaid").toDouble()
               + wo->fOrderDriver->headerValue("f_amountbank").toDouble()
               + wo->fOrderDriver->headerValue("f_amountidram").toDouble()
               + wo->fOrderDriver->headerValue("f_amountpayx").toDouble();
    if (a > wo->fOrderDriver->headerValue("f_amounttotal").toDouble()) {
        C5Message::error(QString("%1, %2/%3").arg(tr("Total amount of payments methods greater than total amount"))
                         .arg(a)
                         .arg(wo->fOrderDriver->headerValue("f_amounttotal").toDouble()));
        return;
    }
    if (!wo->fOrderDriver->save()) {
        C5Message::error(wo->fOrderDriver->error());
        return;
    }
    ui->btnExit->setEnabled(false);
    //LOG
    QString payMethods;
    QString amounts;
    if (wo->fOrderDriver->headerValue("f_amountcash").toDouble() > 0.001) {
        payMethods += payMethods.isEmpty() ? "Cash" : payMethods + " / Cash";
        amounts += amounts.isEmpty() ? float_str(wo->fOrderDriver->headerValue("f_amountcash").toDouble(),
                   2) : amounts + " / " + float_str(wo->fOrderDriver->headerValue("f_amountcash").toDouble(), 2);
    }
    if (wo->fOrderDriver->headerValue("f_amountcard").toDouble() > 0.001) {
        payMethods += payMethods.isEmpty() ? "Cash" : payMethods + " / Card";
        //amounts += amounts.isEmpty() ? fOrder->hString("f_amountcard") : amounts + " / " + fOrder->hString("f_amountcard");
        amounts += amounts.isEmpty() ? float_str(wo->fOrderDriver->headerValue("f_amountcard").toDouble(),
                   2) : amounts + " / " + float_str(wo->fOrderDriver->headerValue("f_amountcard").toDouble(), 2);
    }
    if (wo->fOrderDriver->headerValue("f_amountbank").toDouble() > 0.001) {
        payMethods = "Bank";
        amounts = float_str(wo->fOrderDriver->headerValue("f_amountbank").toDouble(), 2);
    }
    if (wo->fOrderDriver->headerValue("f_amountother").toDouble() > 0.001) {
        switch (wo->fOrderDriver->headerValue("f_otherid").toInt()) {
            case PAYOTHER_TRANSFER_TO_ROOM:
                payMethods = wo->fOrderDriver->payRoomValue("f_room").toString() + ","
                             + wo->fOrderDriver->payRoomValue("f_inv").toString() + ","
                             + wo->fOrderDriver->payRoomValue("f_guest").toString();
                break;
            case PAYOTHER_CL:
                payMethods = wo->fOrderDriver->clValue("f_code").toString() + "," + wo->fOrderDriver->clValue("f_name").toString();
                break;
            case PAYOTHER_COMPLIMENTARY:
                payMethods = "Complimentary";
                break;
            case PAYOTHER_PRIMECOST:
                payMethods = "Selfcost";
                break;
            default:
                payMethods = QString("%1 (%2)").arg("Unknown").arg(wo->fOrderDriver->headerValue("f_otherid").toInt());
                break;
        }
        amounts = float_str(wo->fOrderDriver->headerValue("f_amountother").toDouble(), 2);
    }
    amounts += ui->btnTax->isChecked() ? " Fiscal: yes" : " Fiscal: no";
    C5LogToServerThread::remember(LOG_WAITER, tmp->fullName(), "", wo->fOrderDriver->currentOrderId(), "", "Receipt",
                                  payMethods, amounts);
    C5Database db(__c5config.dbParams());
    db[":f_id"] = wo->fOrderDriver->headerValue("f_id");
    db.exec("select f_type, f_value, f_card from b_history where f_id=:f_id");
    if (db.nextRow()) {
        int mode = db.getInt("f_mode");
        int card = db.getInt("f_card");
        double value = db.getDouble("f_value") / 100;
        switch (mode) {
            case CARD_TYPE_ACCUMULATIVE:
                value = value *wo->fOrderDriver->headerValue("f_amounttotal").toDouble();
                db[":f_card"] = card;
                db[":f_trsale"] = wo->fOrderDriver->headerValue("f_id");
                db[":f_amount"] = value;
                db.insert("b_gift_card_history", false);
                db[":f_data"] = value;
                db.update("b_history", "f_id", wo->fOrderDriver->headerValue("f_id"));
        }
    }
    C5SocketHandler *sh = createSocketHandler(SLOT(handleReceipt(QJsonObject)));
    sh->bind("cmd", sm_printreceipt);
    sh->bind("station", hostinfo);
    sh->bind("printer", C5Config::localReceiptPrinter());
    sh->bind("order", wo->fOrderDriver->currentOrderId());
    sh->bind("language", C5Config::getRegValue("receipt_language").toInt());
    sh->bind("printtax", ui->btnTax->isChecked() ? 1 : 0);
    sh->bind("receipt_printer", C5Config::fSettingsName);
    sh->bind("alias", __c5config.getValue(param_force_use_print_alias).toInt());
    sh->bind("close", 1);
    sh->bind("nofinalreceipt", __c5config.getValue(param_waiter_dontprint_final_receipt).toInt());
    sh->send();
}

void DlgOrder::on_btnCloseOrder_clicked()
{
    if (C5Message::question(tr("Confirm to close order")) != QDialog::Accepted) {
        return;
    }
    WOrder *wo = worder();
    QString orderid = wo->fOrderDriver->currentOrderId();
    if (!wo->fOrderDriver->closeOrder()) {
        C5Message::error(wo->fOrderDriver->error());
        return;
    }
    C5LogToServerThread::remember(LOG_WAITER, fUser->fullName(), "", orderid, "", "Close order", "", "");
    removeWOrder(wo);
    if (!worder()) {
        accept();
        return;
    }
    itemsToTable();
    setButtonsState();
}

void DlgOrder::on_btnService_clicked()
{
    C5User *tmp = fUser;
    if (!tmp->check(cp_t5_change_service_value)) {
        if (!DlgPassword::getUserAndCheck(tr("Change service value"), tmp, cp_t5_change_service_value)) {
            return;
        }
    }
    C5Database db(__c5config.dbParams());
    db.exec("select * from o_service_values");
    QStringList lst;
    while (db.nextRow()) {
        lst.append(float_str(db.getDouble("f_value") * 100, 2));
    }
    int index = 0;
    if (!DlgList::getValue(lst, index)) {
        return;
    }
    QList<WOrder *> wos = worders();
    for (WOrder *wo : qAsConst(wos)) {
        QString oldValue = wo->fOrderDriver->headerValue("f_servicefactor").toString();
        wo->fOrderDriver->setHeader("f_servicefactor", lst.at(index).toDouble() / 100);
        wo->fOrderDriver->amountTotal();
        wo->fOrderDriver->save();
        logRecord(tmp->fullName(), wo->fOrderDriver->headerValue("f_id").toString(), "", "Change service factor", oldValue,
                  wo->fOrderDriver->headerValue("f_servicefactor").toString());
    }
    itemsToTable();
}

void DlgOrder::on_btnTax_clicked()
{
    QSettings _ls(qApp->applicationDirPath() + "/ls.inf", QSettings::IniFormat);
    bool taxforce = _ls.value("ls/forcetax").toInt() == 1;
    bool notax = _ls.value("ls/notax").toInt() == 1;
    if (ui->leOther->getDouble() > 0.001) {
        ui->btnTax->setChecked(false);
    }
    if (ui->leCard->getDouble() > 0.001) {
        ui->btnTax->setChecked(true && !taxforce);
    }
    if (ui->lePrepaid->getDouble() > 0.001) {
        ui->btnTax->setChecked(true && !taxforce);
    }
    if (notax) {
        ui->btnTax->setChecked(false);
    }
    C5Config::setRegValue("btn_tax_state", ui->btnTax->isChecked() ? 1 : 2);
}

void DlgOrder::on_btnStopListMode_clicked()
{
    C5User *tmp = fUser;
    if (tmp->check(cp_t5_stoplist) == false) {
        if (!DlgPassword::getUserAndCheck(tr("Reprint service check"), tmp, cp_t5_stoplist)) {
            return;
        }
    }
    DlgStopListOption d(this, tmp);
    d.exec();
    if (tmp != fUser) {
        delete tmp;
    }
}

void DlgOrder::on_btnOrderComment_clicked()
{
    if (WOrder *wo = worder()) {
        QString comment = wo->fOrderDriver->headerValue("f_comment").toString();
        if (!DlgText::getText(tr("Order comment"), comment)) {
            return;
        }
        wo->fOrderDriver->setHeader("f_comment", comment);
        setButtonsState();
    }
}

void DlgOrder::on_btnPlus1_clicked()
{
    int index = 0;
    WOrder *wo = worder();
    if (!wo) {
        return;
    }
    if (!wo->currentRow(index)) {
        return;
    }
    if (wo->fOrderDriver->dishesValue("f_state", index).toInt() != DISH_STATE_OK) {
        return;
    }
    if (dbdish->isHourlyPayment(wo->fOrderDriver->dishesValue("f_dish", index).toInt())) {
        C5Message::error(tr("Cannot add comment to hourly payment"));
        return;
    }
    if (wo->fOrderDriver->dishesValue("f_qty2", index).toDouble() > 0.0001) {
        QString special;
        if (!wo->fOrderDriver->dishesValue("f_comment2", index).toString().isEmpty()) {
            if (!DlgListDishSpecial::getSpecial(wo->fOrderDriver->dishesValue("f_dish", index).toInt(), fDBParams, special)) {
                return;
            }
        }
        int newindex = wo->fOrderDriver->duplicateDish(index);
        wo->fOrderDriver->setDishesValue("f_qty1", 1, newindex);
        wo->fOrderDriver->setDishesValue("f_qty2", 0, newindex);
        wo->fOrderDriver->setDishesValue("f_comment2", special, newindex);
        wo->fOrderDriver->setDishesValue("f_comment", "", newindex);
        logRecord(fUser->fullName(),
                  wo->fOrderDriver->headerValue("f_id").toString(),
                  wo->fOrderDriver->dishesValue("f_id", newindex).toString(),
                  "Copy dish " + dbdish->name(wo->fOrderDriver->dishesValue("f_dish", index).toInt()),
                  "from: " + wo->fOrderDriver->dishesValue("f_id", index).toString(),
                  wo->fOrderDriver->dishesValue("f_qty1", newindex).toString());
        itemsToTable();
        wo->setCurrentRow(newindex);
    } else {
        QString oldQty = wo->fOrderDriver->dishesValue("f_qty1", index).toString();
        wo->fOrderDriver->setDishesValue("f_qty1", wo->fOrderDriver->dishesValue("f_qty1", index).toDouble() + 1, index);
        itemsToTable();
        logRecord(fUser->fullName(), wo->fOrderDriver->headerValue("f_id").toString(),
                  wo->fOrderDriver->dishesValue("f_id", index).toString(),
                  "Qty of " + dbdish->name(wo->fOrderDriver->dishesValue("f_dish", index).toInt()),
                  oldQty,
                  wo->fOrderDriver->dishesValue("f_qty1", index).toString());
    }
}

void DlgOrder::on_btnMinus1_clicked()
{
    int index = 0;
    WOrder *wo = worder();
    if (!wo) {
        return;
    }
    if (!wo->currentRow(index)) {
        return;
    }
    if (wo->fOrderDriver->dishesValue("f_state", index).toInt() != DISH_STATE_OK) {
        return;
    }
    if (dbdish->isHourlyPayment(wo->fOrderDriver->dishesValue("f_dish", index).toInt())) {
        C5Message::error(tr("Cannot add comment to hourly payment"));
        return;
    }
    if (wo->fOrderDriver->dishesValue("f_qty2", index).toDouble() > 0.0001) {
        C5Message::error(tr("Use removal tool"));
        return;
    } else {
        if (wo->fOrderDriver->dishesValue("f_qty1", index).toDouble() - 1 > 0.001) {
            QString oldQty = wo->fOrderDriver->dishesValue("f_qty1", index).toString();
            wo->fOrderDriver->setDishesValue("f_qty1", wo->fOrderDriver->dishesValue("f_qty1", index).toDouble() - 1, index);
            logRecord(fUser->fullName(), wo->fOrderDriver->headerValue("f_id").toString(),
                      wo->fOrderDriver->dishesValue("f_id", index).toString(),
                      "Qty of " + dbdish->name(wo->fOrderDriver->dishesValue("f_dish", index).toInt()),
                      oldQty,
                      wo->fOrderDriver->dishesValue("f_qty1", index).toString());
        } else if (C5Message::question(tr("Do you want to remove this item")) == QDialog::Accepted) {
            wo->fOrderDriver->setDishesValue("f_state", DISH_STATE_NONE, index);
            logRecord(fUser->fullName(), wo->fOrderDriver->headerValue("f_id").toString(),
                      wo->fOrderDriver->dishesValue("f_id", index).toString(),
                      "Remove not printed " + dbdish->name(wo->fOrderDriver->dishesValue("f_dish", index).toInt()),
                      "-",
                      wo->fOrderDriver->dishesValue("f_qty1", index).toString());
        }
        itemsToTable();
    }
}

void DlgOrder::on_btnAnyqty_clicked()
{
    int index = 0;
    WOrder *wo = worder();
    if (!wo) {
        return;
    }
    if (!wo->currentRow(index)) {
        return;
    }
    if (wo->fOrderDriver->dishesValue("f_state", index).toInt() != DISH_STATE_OK) {
        return;
    }
    if (dbdish->isHourlyPayment(wo->fOrderDriver->dishesValue("f_dish", index).toInt())) {
        C5Message::error(tr("Cannot add comment to hourly payment"));
        return;
    }
    if (wo->fOrderDriver->dishesValue("f_qty2", index).toDouble() > 0.0001) {
        C5Message::error(tr("Use removal tool"));
        return;
    }
    //TODO: PACKAGE HANDLER
    //    if (wo->fOrderDriver->dishesValue("d_package", index).toInt() > 0) {
    //        C5Message::error(tr("You cannot change the quantity of items of package"));
    //        return;
    //    }
    if (dbdish->isHourlyPayment(wo->fOrderDriver->dishesValue("f_dish", index).toInt())) {
        C5Message::error(tr("This is hourly payment item"));
        return;
    }
    double max = 999;
    if (!DlgQty::getQty(max, dbdish->name(wo->fOrderDriver->dishesValue("f_dish", index).toInt()))) {
        return;
    }
    QString oldQty = wo->fOrderDriver->dishesValue("f_qty1", index).toString();
    wo->fOrderDriver->setDishesValue("f_qty1", max, index);
    wo->updateItem(index);
    itemsToTable();
    logRecord(fUser->fullName(), wo->fOrderDriver->headerValue("f_id").toString(),
              wo->fOrderDriver->dishesValue("f_id", index).toString(),
              "Qty of " + dbdish->name(wo->fOrderDriver->dishesValue("f_dish", index).toInt()),
              oldQty,
              wo->fOrderDriver->dishesValue("f_qty1", index).toString());
}

void DlgOrder::on_btnDishPart2Down_clicked()
{
    ui->scrollAreaPart2->verticalScrollBar()->setValue(ui->scrollAreaPart2->verticalScrollBar()->value() + 300);
    for (QObject *o : ui->scrollAreaWidgetContents->children()) {
        QWidget *w = dynamic_cast<QWidget *>(o);
        if (w) {
            w->repaint();
        }
    }
}

void DlgOrder::on_btnDishPart2Up_clicked()
{
    ui->scrollAreaPart2->verticalScrollBar()->setValue(ui->scrollAreaPart2->verticalScrollBar()->value() - 300);
    for (QObject *o : ui->scrollAreaWidgetContents->children()) {
        QWidget *w = dynamic_cast<QWidget *>(o);
        if (w) {
            w->repaint();
        }
    }
}

void DlgOrder::on_btnBillWithoutService_clicked()
{
    WOrder *wo = worder();
    if (!wo) {
        return;
    }
    C5Message::info(QString("%1<br>%2").arg(tr("Counted")).arg(float_str(wo->fOrderDriver->clearAmount(), 2)));
}

void DlgOrder::on_btnFillCash_clicked()
{
    worder()->fOrderDriver->setHeader("f_amountcash",
                                      worder()->fOrderDriver->headerValue("f_amounttotal").toDouble()
                                      - worder()->fOrderDriver->headerHotel("f_amountbank").toDouble()
                                      - worder()->fOrderDriver->headerValue("f_amountcard").toDouble()
                                      - worder()->fOrderDriver->headerValue("f_amountprepaid").toDouble()
                                      - worder()->fOrderDriver->headerValue("f_amountidram").toDouble()
                                      - worder()->fOrderDriver->headerValue("f_amountpayx").toDouble());
    worder()->fOrderDriver->setHeader("f_amountother", 0);
    ui->leRemain->setDouble(0);
    clearOther();
    headerToLineEdit();
    setButtonsState();
}

void DlgOrder::on_btnFillCard_clicked()
{
    worder()->fOrderDriver->setHeader("f_amountcard",
                                      worder()->fOrderDriver->headerValue("f_amounttotal").toDouble()
                                      - worder()->fOrderDriver->headerValue("f_amountbank").toDouble()
                                      - worder()->fOrderDriver->headerValue("f_amountcash").toDouble()
                                      - worder()->fOrderDriver->headerValue("f_amountprepaid").toDouble()
                                      - worder()->fOrderDriver->headerValue("f_amountidram").toDouble()
                                      - worder()->fOrderDriver->headerValue("f_amountpayx").toDouble());
    worder()->fOrderDriver->setHeader("f_amountother", 0);
    ui->leRemain->setDouble(0);
    clearOther();
    headerToLineEdit();
    setButtonsState();
}

void DlgOrder::on_btnFillPrepaiment_clicked()
{
    worder()->fOrderDriver->setHeader("f_amountprepaid",
                                      worder()->fOrderDriver->headerValue("f_amounttotal").toDouble()
                                      - worder()->fOrderDriver->headerValue("f_amountcash").toDouble()
                                      - worder()->fOrderDriver->headerValue("f_amountcard").toDouble()
                                      - worder()->fOrderDriver->headerValue("f_amountbank").toDouble()
                                      - worder()->fOrderDriver->headerValue("f_amountidram").toDouble()
                                      - worder()->fOrderDriver->headerValue("f_amountpayx").toDouble());
    worder()->fOrderDriver->setHeader("f_amountother", 0);
    ui->leRemain->setDouble(0);
    clearOther();
    headerToLineEdit();
    setButtonsState();
}

void DlgOrder::on_btnDishDown_clicked()
{
    ui->scrollAreaDish->verticalScrollBar()->setValue(ui->scrollAreaDish->verticalScrollBar()->value() + 300);
    for (QObject *o : ui->scrollAreaWidgetContentsDish->children()) {
        QWidget *w = dynamic_cast<QWidget *>(o);
        if (w) {
            w->repaint();
        }
    }
    ui->scrollAreaDish->viewport()->update();
}

void DlgOrder::on_btnDishUp_clicked()
{
    ui->scrollAreaDish->verticalScrollBar()->setValue(ui->scrollAreaDish->verticalScrollBar()->value() - 300);
    for (QObject *o : ui->scrollAreaWidgetContentsDish->children()) {
        QWidget *w = dynamic_cast<QWidget *>(o);
        if (w) {
            w->repaint();
        }
    }
    ui->scrollAreaDish->viewport()->update();
}

void DlgOrder::on_btnPreorder_clicked()
{
    C5User *tmp = fUser;
    if (!tmp->check(cp_t5_preorder)) {
        if (!DlgPassword::getUserAndCheck(tr("Preorder"), tmp, cp_t5_preorder)) {
            return;
        }
    }
    WOrder *wo = worder();
    if (!wo) {
        return;
    }
    DlgPreorder dlg(wo->fOrderDriver, tmp, fDBParams);
    dlg.exec();
    if (tmp != fUser) {
        delete tmp;
    }
    if (wo->fOrderDriver->preorder("f_guest").toInt() > 0) {
        ui->leGuest->setText(dbpartner->name(wo->fOrderDriver->preorder("f_guest").toInt()));
    }
    ui->lePrepaidAmount->setDouble(wo->fOrderDriver->preorder("f_prepaidcash").toDouble()
                                   + wo->fOrderDriver->preorder("f_prepaidcard").toDouble()
                                   + wo->fOrderDriver->preorder("f_prepaidpayx").toDouble());
    if (dlg.result() == DlgPreorder::RESULT_CANCELED) {
        wo->fOrderDriver->setHeader("f_state", ORDER_STATE_VOID);
        for (int i = 0; i < wo->fOrderDriver->dishesCount(); i++) {
            if (wo->fOrderDriver->dishesValue("f_state", i).toInt() == DISH_STATE_OK) {
                wo->fOrderDriver->setDishesValue("f_state", DISH_STATE_NONE, i);
            }
        }
        accept();
    }
}

void DlgOrder::on_btnPaymentIdram_clicked()
{
    auto *wo = worder();
    if (!wo) {
        return;
    }
    QByteArray out;
    double v;
    if (Idram::check(__c5config.getValue(param_idram_server), __c5config.getValue(param_idram_session_id),
                     wo->fOrderDriver->headerValue("f_id").toString(), v, out)) {
        ui->leIDRAM->setDouble(v);
        //        ui->leCash->setText("0");
        //        ui->leCard->setText("0");
        //        ui->leBank->setText("0");
        //        ui->leOther->setText("0");
        worder()->fOrderDriver->setHeader("f_amountcash", 0);
        worder()->fOrderDriver->setHeader("f_amountcard", 0);
        worder()->fOrderDriver->setHeader("f_amountbank", 0);
        worder()->fOrderDriver->setHeader("f_amountprepaid", 0);
        worder()->fOrderDriver->setHeader("f_amountother", 0);
        worder()->fOrderDriver->setHeader("f_amountpayx", 0);
        worder()->fOrderDriver->setHeader("f_amountidram", v);
        worder()->fOrderDriver->setHeader("f_hotel", 0);
        worder()->fOrderDriver->setHeader("f_otherid", 0);
        ui->leRemain->setDouble(worder()->fOrderDriver->headerValue("f_amounttotal").toDouble() - v);
        clearOther();
        headerToLineEdit();
        setButtonsState();
        return ;
        worder()->fOrderDriver->setHeader("f_otherid", 0);
        lineEditToHeader();
    } else {
        QString err = out;
        if (v < 0) {
            err = tr("Idram payment was not received");
        }
        C5Message::error(err);
    }
    C5LogToServerThread::remember(LOG_WAITER, fUser->fullName(), "",
                                  wo->fOrderDriver->currentOrderId(), "",
                                  "Idram request: " + __c5config.getValue(param_idram_server),
                                  QString(out), "");
}

void DlgOrder::on_btnFillPayX_clicked()
{
    WOrder *wo = worder();
    if (!wo) {
        return;
    }
    ui->lePayX->setDouble(
        wo->fOrderDriver->headerValue("f_amounttotal").toDouble()
        - wo->fOrderDriver->headerValue("f_amountcash").toDouble()
        - wo->fOrderDriver->headerValue("f_amountcard").toDouble()
        - wo->fOrderDriver->headerValue("f_amountbank").toDouble()
        - wo->fOrderDriver->headerValue("f_amountprepaid").toDouble()
        - wo->fOrderDriver->headerValue("f_amountidram").toDouble());
    wo->fOrderDriver->setHeader("f_amountpayx", ui->lePayX->getDouble());
    ui->leRemain->setDouble(0);
    headerToLineEdit();
    setButtonsState();
}

void DlgOrder::on_btnCalcPayX_clicked()
{
    auto *wo = worder();
    if (!wo) {
        return;
    }
    double max = wo->fOrderDriver->headerValue("f_amounttotal").toDouble();
    if (!DlgPassword::getAmount(tr("PayX"), max)) {
        return;
    }
    ui->lePayX->setDouble(max);
    lineEditToHeader();
    headerToLineEdit();
}

void DlgOrder::on_btnPayX_clicked()
{
    auto *wo = worder();
    if (!wo) {
        return;
    }
    worder()->fOrderDriver->setHeader("f_otherid", 0);
    ui->lePayX->setDouble(wo->fOrderDriver->headerValue("f_amounttotal").toDouble() - ui->leIDRAM->getDouble());
    ui->leCash->setDouble(0);
    ui->leCard->setDouble(0);
    ui->leBank->setDouble(0);
    ui->leOther->setDouble(0);
    lineEditToHeader();
}

void DlgOrder::on_btnFillIdram_clicked()
{
    auto *wo = worder();
    if (!wo) {
        return;
    }
    ui->leIDRAM->setDouble(wo->fOrderDriver->headerValue("f_amounttotal").toDouble()
                           - worder()->fOrderDriver->headerValue("f_amontprepaid").toDouble()
                           - worder()->fOrderDriver->headerValue("f_amountcash").toDouble()
                           - worder()->fOrderDriver->headerValue("f_amountcard").toDouble()
                           - worder()->fOrderDriver->headerValue("f_amountbank").toDouble()
                           - worder()->fOrderDriver->headerValue("f_amountpayx").toDouble());
    worder()->fOrderDriver->setHeader("f_amountother", 0);
    ui->lePayX->setDouble(0);
    ui->leCash->setDouble(0);
    ui->leCard->setDouble(0);
    ui->leBank->setDouble(0);
    ui->leOther->setDouble(0);
    lineEditToHeader();
}

void DlgOrder::on_btnCloseCheckAll_clicked()
{
    disableForCheckall(false);
}

void DlgOrder::on_btnCheckAll_clicked()
{
    WOrder *wo = worder();
    if (wo) {
        wo->checkAllItems(true);
    }
}

void DlgOrder::on_btnUncheckAll_clicked()
{
    WOrder *wo = worder();
    if (wo) {
        wo->checkAllItems(false);
    }
}

void DlgOrder::on_btnReprintSelected_clicked()
{
    disableForCheckall(false);
    C5User *tmp = fUser;
    if (!tmp->check(cp_t5_repeat_service)) {
        if (!DlgPassword::getUserAndCheck(tr("Reprint service check"), tmp, cp_t5_repeat_service)) {
            return;
        }
    }
    ui->btnPrintService->setEnabled(true);
    if (tmp != fUser) {
        delete tmp;
    }
    WOrder *wo = worder();
    if (!wo) {
        return;
    }
    QStringList reprintList;
    for (int i = 0; i < wo->fOrderDriver->dishesCount(); i++) {
        if (wo->dishWidget(i)->isChecked()) {
            wo->fOrderDriver->setDishesValue("f_reprint", i, wo->fOrderDriver->dishesValue("f_reprint", i).toInt() * -1);
            reprintList.append(QString("'%1'").arg(wo->fOrderDriver->dishesValue("f_id", i).toString()));
        }
    }
    for (int i = 0; i < wo->fOrderDriver->dishesCount(); i++) {
        if (wo->fOrderDriver->dishesValue("f_qty2", i).toDouble() < 0.001) {
            wo->fOrderDriver->setDishesValue("f_printtime", QDateTime::currentDateTime(), i);
        }
    }
    if (!wo->fOrderDriver->save()) {
        C5Message::error(worder()->fOrderDriver->error());
        return;
    }
    C5SocketHandler *sh = createSocketHandler(SLOT(handlePrintService(QJsonObject)));
    sh->bind("cmd", sm_printservice);
    sh->bind("order", wo->fOrderDriver->currentOrderId());
    sh->bind("reprint", reprintList.join(","));
    sh->bind("booking", dbhall->booking(wo->fOrderDriver->headerValue("f_hall").toInt()));
    sh->send();
    logRecord(fUser->fullName(), wo->fOrderDriver->headerValue("f_id").toString(), "", "Duplicate ---Send to cooking", "",
              "");
}

void DlgOrder::on_btnGroupSelect_clicked()
{
    disableForCheckall(true);
}

void DlgOrder::on_btnRemoveSelected_clicked()
{
    disableForCheckall(false);
    WOrder *wo = worder();
    if (!wo) {
        return;
    }
    QList<int> indexes = wo->checkedItems();
    if (indexes.isEmpty()) {
        return;
    }
    QString reasonname;
    int reasonid = 0;
    bool needreason = false;
    for (int i : indexes) {
        if (wo->fOrderDriver->dishesValue("f_qty2", i).toDouble() > 0.001) {
            needreason = true;
            break;
        }
    }
    C5User *tmp = fUser;
    if (needreason) {
        if (!tmp->check(cp_t5_remove_printed_service)) {
            if (!DlgPassword::getUserAndCheck(tr("Void dish"), tmp, cp_t5_remove_printed_service)) {
                return;
            }
        }
        if (!DlgDishRemoveReason::getReason(reasonname, reasonid)) {
            if (tmp != fUser) {
                tmp->deleteLater();
            }
            return;
        }
    }
    if (C5Message::question(tr("Confirm to remove selected dishes")) != QDialog::Accepted) {
        if (tmp != fUser) {
            tmp->deleteLater();
        }
        return;
    }
    for (int in = 0; in < indexes.count(); in++) {
        int i = indexes.at(in);
        if (wo->fOrderDriver->dishesValue("f_qty2", i).toDouble() < 0.001) {
            wo->fOrderDriver->setDishesValue("f_state", DISH_STATE_NONE, i);
        } else {
            wo->fOrderDriver->setDishesValue("f_state", reasonid, i);
            wo->fOrderDriver->setDishesValue("f_removetime", QDateTime::currentDateTime(), i);
            wo->fOrderDriver->setDishesValue("f_removeuser", tmp->id(), i);
            wo->fOrderDriver->setDishesValue("f_removereason", reasonname, i);
            wo->fOrderDriver->setDishesValue("f_removeprecheck", wo->fOrderDriver->headerValue("f_precheck"), i);
            wo->fOrderDriver->save();
            C5SocketHandler *sh = createSocketHandler(SLOT(handlePrintRemovedService(QJsonObject)));
            sh->bind("cmd", sm_print_removed_service);
            sh->bind("id", wo->fOrderDriver->dishesValue("f_id", i).toString());
            sh->send();
        }
    }
    if (tmp != fUser) {
        tmp->deleteLater();
    }
    wo->fOrderDriver->save();
    wo->updateDishes();
}

void DlgOrder::on_btnSetPrecent_clicked()
{
    disableForCheckall(false);
    C5User *tmp = fUser;
    WOrder *wo = worder();
    if (!wo) {
        return;
    }
    bool needproceed = false;
    for (int i = 0; i < wo->fOrderDriver->dishesCount(); i++) {
        if (wo->fOrderDriver->dishesValue("f_state", i).toInt() != DISH_STATE_OK) {
            continue;
        }
        if (wo->fOrderDriver->dishesValue("f_price", i) < 0.01) {
            continue;
        }
        if (wo->dishWidget(i)->isChecked()) {
            needproceed = true;
            break;
        }
    }
    if (!needproceed) {
        return;
    }
    if (!tmp->check(cp_t5_present)) {
        if (!DlgPassword::getUserAndCheck(tr("Present dish"), tmp, cp_t5_present)) {
            return;
        }
    }
    if (C5Message::question(tr("Are you sure to present selected dish?")) != QDialog::Accepted) {
        return;
    }
    for (int i = 0; i < wo->fOrderDriver->dishesCount(); i++) {
        if (wo->fOrderDriver->dishesValue("f_state", i).toInt() != DISH_STATE_OK) {
            continue;
        }
        if (wo->fOrderDriver->dishesValue("f_price", i) < 0.01) {
            continue;
        }
        if (wo->dishWidget(i)->isChecked()) {
            wo->fOrderDriver->setDishesValue("f_price", 0, i);
            logRecord(tmp->fullName(),
                      wo->fOrderDriver->headerValue("f_id").toString(),
                      wo->fOrderDriver->dishesValue("f_id", i).toString(),
                      "Present dish",
                      dbdish->name(wo->fOrderDriver->dishesValue("f_dish", i).toInt()),
                      "");
        }
    }
    itemsToTable();
    if (tmp != fUser) {
        delete tmp;
    }
}

void DlgOrder::on_btnReceived_clicked()
{
    WOrder *wo = worder();
    if (!wo) {
        return;
    }
    double v = wo->fOrderDriver->headerValue("f_amounttotal").toDouble();
    if (Change::getReceived(v)) {
        wo->fOrderDriver->setHeader("f_cash", v);
        headerToLineEdit();
    }
}

void DlgOrder::on_btnSelfCost_clicked()
{
    WOrder *w = worder();
    if (!w) {
        return;
    }
    w->fOrderDriver->setHeader("f_otherid", PAYOTHER_PRIMECOST);
    setSelfcost();
    if (w->fOrderDriver->save() == false) {
        C5Message::error(w->fOrderDriver->error());
    }
}

void DlgOrder::on_btnDelivery_clicked()
{
    WOrder *w = worder();
    if (!w) {
        return;
    }
    QString phone, address, name;
    int id;
    if (CustomerInfo::getCustomer(id, name, phone, address)) {
        w->fOrderDriver->setHeader("f_partner", id);
        ui->btnDelivery->setText(QString("%1, %2, %3").arg(phone, name, address));
    }
}
void DlgOrder::on_btnCalcIdram_clicked()
{
    calcAmount(ui->leIDRAM);
}

void DlgOrder::on_btnFillBank_clicked()
{
    worder()->fOrderDriver->setHeader("f_amountbank",
                                      worder()->fOrderDriver->headerValue("f_amounttotal").toDouble()
                                      - worder()->fOrderDriver->headerValue("f_amountbank").toDouble()
                                      - worder()->fOrderDriver->headerValue("f_amountcash").toDouble()
                                      - worder()->fOrderDriver->headerValue("f_amountprepaid").toDouble()
                                      - worder()->fOrderDriver->headerValue("f_amountidram").toDouble()
                                      - worder()->fOrderDriver->headerValue("f_amountpayx").toDouble());
    worder()->fOrderDriver->setHeader("f_amountother", 0);
    ui->leRemain->setDouble(0);
    clearOther();
    headerToLineEdit();
    setButtonsState();
}

void DlgOrder::on_btnCashout_clicked()
{
    WOrder *wo = worder();
    double max = wo->fOrderDriver->headerValue("f_amounttotal").toDouble();
    if (!DlgPassword::getAmount(tr("Cashout"), max)) {
        return;
    }
    QString comment;
    if (!DlgListOfDishComments::getComment(0, comment)) {
        return;
    }
    QString purpose = tr("") + " #"
                      + wo->fOrderDriver->headerValue("f_prefix").toString()
                      + wo->fOrderDriver->headerValue("f_hallid").toString() + " " + comment;
    C5Database db(__c5config.dbParams());
    db[":f_oheader"] = wo->fOrderDriver->headerValue("f_id");
    db.exec("select e.f_id as f_idout, a.f_id "
            "from a_header_cash a "
            "left join e_cash e on e.f_header=a.f_id "
            "where f_oheader=:f_oheader");
    QString cashUUID;
    QString idout;
    if (db.nextRow()) {
        cashUUID = db.getString("f_id");
        idout = db.getString("f_idout");
    }
    C5StoreDraftWriter dw(db);
    dw.writeAHeader(cashUUID, tr("Delivery"), DOC_STATE_SAVED, DOC_TYPE_CASH,
                    wo->fOrderDriver->headerValue("f_currentstaff").toInt(),
                    QDate::currentDate(), QDate::currentDate(), QTime::currentTime(), 0, max, purpose, 1, 1);
    dw.writeAHeaderCash(cashUUID, 0, 1, 0, "", wo->fOrderDriver->headerValue("f_id").toString(), 0);
    dw.writeECash(idout, cashUUID, 1, -1, purpose, max, idout, 1);
    C5Message::info(purpose);
}

void DlgOrder::on_btnMenuSet_clicked()
{
    int index = 0;
    WOrder *wo = worder();
    if (!wo) {
        return;
    }
    if (!wo->currentRow(index)) {
        return;
    }
    if (wo->fOrderDriver->dishesValue("f_state", index).toInt() != DISH_STATE_OK) {
        return;
    }
    DlgMenuSet d(wo->fOrderDriver->dishesValue("f_id", index).toString());
    d.exec();
    wo->fOrderDriver->reloadOrder();
    itemsToTable();
}

void DlgOrder::on_btnQR_clicked()
{
    int index = 0;
    WOrder *wo = worder();
    if (!wo) {
        return;
    }
    if (!wo->currentRow(index)) {
        return;
    }
    if (wo->fOrderDriver->dishesValue("f_emarks", index).toInt() != DISH_STATE_OK) {
        return;
    }
    if (wo->fOrderDriver->dishesValue("f_qty2", index).toDouble() > 0.0001) {
        QString special;
        if (!wo->fOrderDriver->dishesValue("f_comment2", index).toString().isEmpty()) {
            if (!DlgListDishSpecial::getSpecial(wo->fOrderDriver->dishesValue("f_dish", index).toInt(), fDBParams, special)) {
                return;
            }
        }
        int newindex = wo->fOrderDriver->duplicateDish(index);
        wo->fOrderDriver->setDishesValue("f_qty1", 1, newindex);
        wo->fOrderDriver->setDishesValue("f_qty2", 0, newindex);
        wo->fOrderDriver->setDishesValue("f_comment2", special, newindex);
        wo->fOrderDriver->setDishesValue("f_comment", "", newindex);
        logRecord(fUser->fullName(),
                  wo->fOrderDriver->headerValue("f_id").toString(),
                  wo->fOrderDriver->dishesValue("f_id", newindex).toString(),
                  "Copy dish " + dbdish->name(wo->fOrderDriver->dishesValue("f_dish", index).toInt()),
                  "from: " + wo->fOrderDriver->dishesValue("f_id", index).toString(),
                  wo->fOrderDriver->dishesValue("f_qty1", newindex).toString());
        itemsToTable();
        wo->setCurrentRow(newindex);
    } else {
        QString oldQty = wo->fOrderDriver->dishesValue("f_qty1", index).toString();
        wo->fOrderDriver->setDishesValue("f_qty1", wo->fOrderDriver->dishesValue("f_qty1", index).toDouble() + 1, index);
        itemsToTable();
        logRecord(fUser->fullName(), wo->fOrderDriver->headerValue("f_id").toString(),
                  wo->fOrderDriver->dishesValue("f_id", index).toString(),
                  "Qty of " + dbdish->name(wo->fOrderDriver->dishesValue("f_dish", index).toInt()),
                  oldQty,
                  wo->fOrderDriver->dishesValue("f_qty1", index).toString());
    }
}
