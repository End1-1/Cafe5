#include "dlgorder.h"
#include "ui_dlgorder.h"
#include "c5cafecommon.h"
#include "c5user.h"
#include "c5tabledata.h"
#include "c5orderdriver.h"
#include "c5ordertabledelegate.h"
#include "dlgmenuset.h"
#include "dlgsearchinmenu.h"
#include "dlgpassword.h"
#include "dlgqty.h"
#include "c5logtoserverthread.h"
#include "dlglistofdishcomments.h"
#include "customerinfo.h"
#include "c5config.h"
#include "c5permissions.h"
#include "ndataprovider.h"
#include "dlgreceiptlanguage.h"
#include "c5utils.h"
#include "c5translator.h"
#include "dlgpreorderw.h"
#include "dlgsplitorder.h"
#include "dlgpreorderw.h"
#include "dlglistdishspecial.h"
#include "dlgdishremovereason.h"
#include "dlglistofpackages.h"
#include "dlgcl.h"
#include "dlgguest.h"
#include "dlgguests.h"
#include "dlglist.h"
#include "dlgtext.h"
#include "cashboxconfig.h"
#include "dlgprecheckoptions.h"
#include "worderw.h"
#include "dlgface.h"
#include "dlglistofmenu.h"
#include "dishitem.h"
#include "datadriver.h"
#include "cpartners.h"
#include "change.h"
#include "idram.h"
#include "qdishpart2button.h"
#include "qdishbutton.h"
#include "c5message.h"
#include "dlgstoplistoption.h"
#include "dlgaskforprecheck.h"
#include "dlgviewstoplist.h"
#include <QToolButton>
#include <QCloseEvent>
#include <QScreen>
#include <QScrollBar>
#include <QInputDialog>
#include <QPaintEvent>
#include <QJsonObject>
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
    C5Dialog(),
    fUser(user),
    ui(new Ui::DlgOrder)
{
    ui->setupUi(this);
    ui->leReceived->clear();
    ui->leChange->clear();
    fCarNumber = 0;
    ui->lbVisit->setVisible(false);
    ui->lbStaff->setText(user->fullName());
    ui->wqtypaneldown->setVisible(false);
    ui->btnBillWithoutService->setEnabled(user->check(cp_t5_bill_without_service));
    fTimerCounter = 0;
    connect(&fTimer, &QTimer::timeout, this, &DlgOrder::timeout);
    fTimer.start(1000);
    ui->wpayment->setVisible(false);
    ui->btnPayTransferToRoom->setVisible(fUser->check(cp_t5_pay_transfer_to_room));
    ui->btnPayComplimentary->setVisible(fUser->check(cp_t5_pay_complimentary));
    ui->btnPayCityLedger->setVisible(fUser->check(cp_t5_pay_cityledger));
    ui->btnSelfCost->setVisible(fUser->check(cp_t5_pay_breakfast));

    if(!cp_t5_pay_idram) {
        ui->btnPaymentIdram->setVisible(false);
        ui->leIDRAM->setVisible(false);
    }

    if(!cp_t5_pay_payx) {
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

    switch(C5Config::getRegValue("btn_tax_state").toInt()) {
    case 0:
    case 1:
        ui->btnTax->setChecked(true);
        break;

    case 2:
        ui->btnTax->setChecked(false);
        break;
    }

    if(__c5config.getValue(param_tax_print_always_offer).toInt() == 1) {
        ui->btnTax->setChecked(true);
        ui->btnTax->setEnabled(false);
    }

    fMenuID = C5Config::defaultMenu();
    fPart1 = 0;
    fPart2Parent = 0;
    fStoplistMode = false;
    connect(ui->scrollAreaDish->verticalScrollBar(), &QScrollBar::valueChanged, this, [this](int value) {
        for(QObject *o : ui->scrollAreaWidgetContentsDish->children()) {
            QWidget *w = dynamic_cast<QWidget*>(o);

            if(w) {
                w->repaint();
            }
        }

        ui->scrollAreaDish->viewport()->update();
    });
    connect(ui->leCmd, &C5LineEdit::focusIn, this, [this]() {
        ui->leCmd->setFocus();
    });
    auto *t = new QTimer(this);
    connect(t, &QTimer::timeout, this, [this]() {
        ui->leCmd->setFocus();
    });
    t->start(1000);
}

DlgOrder::~DlgOrder()
{
    delete ui;
}

DlgOrder* DlgOrder::openTable(int table, C5User *user)
{
    auto *d = new DlgOrder(user);
    d->setModal(true);

    if(C5Config::isAppFullScreen()) {
        d->setWindowState(Qt::WindowFullScreen);
    }

    d->buildMenu(d->fMenuID, 0, 0);
    d->fHttp->createHttpQueryLambda("/engine/picasso.waiter/",
    QJsonObject{
        {"class", "waiter"},
        {"method", "opentable"},
        {"f_table", table},
        {"f_current_staff", user->id()},
        {"f_current_staff_name", user->fullName()},
        {"f_locksrc", hostinfo}},
    [d](const QJsonObject & jdoc) {
        if(jdoc["status"].toInt() == 0) {
            C5Message::error(jdoc["message"].toString());
            emit(d->allDone());
            return;
        }

        d->fHeader = jdoc["header"].toObject().toVariantMap();

        for(const QJsonValue& v : jdoc["body"].toArray()) {
            d->fBody.append(v.toObject().toVariantMap());
        }

        d->updateData();
        d->show();
    }, [d](const QJsonObject & je) {
        d->accept();
        emit(d->allDone());
        Q_UNUSED(je);
    });
    return d;
}

void DlgOrder::accept()
{
    for(int i = 0; i < ui->vs->count(); i++) {
        WOrder *wo = dynamic_cast<WOrder*>(ui->vs->itemAt(i)->widget());

        if(wo) {
            if(wo->fOrderDriver->isEmpty()
                    && __c5config.getValue(param_waiter_donotclose_empty_order).toInt() == 0
                    && wo->fOrderDriver->headerValue("f_state").toInt() != ORDER_STATE_PREORDER_EMPTY
                    && wo->fOrderDriver->headerValue("f_state").toInt() != ORDER_STATE_PREORDER_WITH_ORDER) {
                wo->fOrderDriver->setCloseHeader();
            }

            for(int i = 0; i < wo->fOrderDriver->dishesCount(); i++) {
                if(wo->fOrderDriver->dishesValue("f_reprint", i).toInt() < 0) {
                    wo->fOrderDriver->setDishesValue("f_reprint", abs(wo->fOrderDriver->dishesValue("f_reprint", i).toInt() - 1), i);
                }
            }

            if(!wo->fOrderDriver->save()) {
                C5Message::error(wo->fOrderDriver->error());
                return;
            }

            logRecord(fUser->fullName(), wo->fOrderDriver->headerValue("f_id").toString(), "", "Close window", "", "");
        }
    }

    QString err;

    if(!dbtable->closeTable(fTable, err)) {
        C5Message::error(err);
        return;
    }

    if(!ui->btnExit->isEnabled()) {
        return;
    }

    C5Dialog::accept();

    //TODO: CHECK L10
    if(!property("reprint").toString().isEmpty()) {
        if(__c5config.getValue(param_waiter_receipt_qty).toInt() > 0 && fUser->check(cp_t5_multiple_receipt)) {
            while(C5Message::question(tr("Print additional receipt for this order?")) == QDialog::Accepted) {
                fHttp->createHttpQueryLambda("/engine/waiter/printreceipt.php", {
                    {"staffid", fUser->id()},
                    {"staffname", fUser->fullName()},
                    {"f_receiptlanguage", C5Config::getRegValue("receipt_language").toInt()},
                    {"order", property("reprint").toString()},
                    {"printtax", false},
                    {"receipt_printer", C5Config::fSettingsName}
                }, [this](const QJsonObject & jo) {
                    auto np = new NDataProvider(this);
                    np->overwriteHost("http", "127.0.0.1", 8080);
                    connect(np, &NDataProvider::done, this, [](const QJsonObject & jjo) {
                    });
                    connect(np, &NDataProvider::error, this, [](const QString & err) {
                    });
                    np->getData("/printjson", jo);
                }, [](const QJsonObject & je) {
                    Q_UNUSED(je);
                });
            }
        }
    }

    emit allDone();
    deleteLater();
}

void DlgOrder::reject()
{
    accept();
}

QList<WOrder*> DlgOrder::worders()
{
    QList<WOrder*> lst;

    for(int i = 0; i < ui->vs->count(); i++) {
        WOrder *o = dynamic_cast<WOrder*>(ui->vs->itemAt(i)->widget());

        if(o) {
            lst.append(o);
        }
    }

    return lst;
}

void DlgOrder::updateData()
{
    QString selectedId;
    ui->btnTable->setText(fHeader["f_table_name"].toString());
    ui->wleft->setVisible(fHeader["f_precheck"].toInt() < 1);
    ui->wdish->setVisible(fHeader["f_precheck"].toInt() < 1);

    while(ui->vlDishes->count() > 0) {
        QLayoutItem *item = ui->vlDishes->takeAt(0);

        if(item) {
            auto *w = static_cast<DishItem*>(item->widget());

            if(w) {
                if(w->isFocused()) {
                    selectedId = w->fData["f_id"].toString();
                }

                delete w;
            }

            delete item;
        }
    }

    for(int i = 0 ; i <  fBody.count(); i++) {
        auto *dw = new DishItem(fBody.at(i), i);
        connect(dw, &DishItem::focused, this, &DlgOrder::handleOrderDishClick);
        connect(this, &DlgOrder::orderDishClicked, dw, &DishItem::checkFocus);
        ui->vlDishes->addWidget(dw);
    }

    //TODO CHECK
    ui->btnService->setText(QString("%1 %2%: %3")
                            .arg(tr("Service amount"))
                            .arg(fHeader["f_servicefactor"].toDouble() * 100)
                            .arg(float_str(fHeader["f_amountservice"].toDouble(), 2)));
    ui->btnDiscount->setText(QString("%1 %2%: %3")
                             .arg(tr("Discount"))
                             .arg(fHeader["f_discountfactor"].toDouble() * 100)
                             .arg(float_str(fHeader["f_amountdiscount"].toDouble(), 2)));
    ui->leTaxNumber->setText(fHeader["f_receiptnumber"].toString());
    ui->lePrepaidAmount->setDouble(fHeader["f_prepaidcash"].toDouble() + fHeader["f_prepaidcard"].toDouble());
    ui->leGuest->setText(fHeader["f_guestname"].toString());

    if(ui->lePrepaidAmount->getDouble() > 0.01) {
        ui->wPreorder->setVisible(true);
    } else {
        ui->wPreorder->setVisible(false);
    }

    ui->vlDishes->addStretch();
}

void DlgOrder::disableForCheckall(bool v)
{
    ui->wqtypanelup->setEnabled(!v);
    ui->wqtypaneldown->setVisible(v);
    ui->wpaneldown->setEnabled(!v);
    ui->wleft->setEnabled(!v);
    ui->wdish->setEnabled(!v);
    ui->wmenu->setEnabled(!v);

    for(int i = 0, count = ui->vlDishes->count(); i < count; i++) {
        QLayoutItem *l = ui->vlDishes->itemAt(i);
        DishItem *d = dynamic_cast<DishItem*>(l->widget());

        if(d) {
            d->setCheckMode(v);
        }
    }
}

void DlgOrder::buildMenu(int menuid, int part1, int part2)
{
    if(menuid == 0) {
        C5Message::error(tr("Menu is not defined"));
        return;
    }

    ui->btnChangeMenu->setText(QString("%1\n%2").arg(tr("Menu"), dbmenuname->name(menuid)));
    fMenuID = menuid;
    fPart1 = part1;

    while(ui->hlDishPart1->itemAt(0)) {
        ui->hlDishPart1->itemAt(0)->widget()->deleteLater();
        ui->hlDishPart1->removeItem(ui->hlDishPart1->itemAt(0));
    }

    for(const QString &p1 : C5TableData::instance()->ids("d_part1")) {
        QPushButton *btn = new QPushButton(tds("d_part1", "f_name", p1.toInt()));
        btn->setProperty("btn_part1", true);
        connect(btn, &QPushButton::clicked, this, &DlgOrder::dishpart1Clicked);
        btn->setProperty("id", p1.toInt());
        ui->hlDishPart1->addWidget(btn);
    }

    if(fPart1 == 0) {
        if(!ui->hlDishPart1->itemAt(0)) {
            C5Message::error(tr("Menu is not defined"));
            return;
        }

        fPart1 = static_cast<QPushButton*>(ui->hlDishPart1->itemAt(0)->widget())->property("id").toInt();
    }

    while(ui->glDishPart2->itemAt(0)) {
        ui->glDishPart2->itemAt(0)->widget()->deleteLater();
        ui->glDishPart2->removeItem(ui->glDishPart2->itemAt(0));
    }

    int colCount = 2;
    int col = 0;
    int row = 0;
    QJsonArray jpart2 = C5TableData::instance()->part2(fMenuID, fPart1, fPart2Parent);

    for(int i = 0; i  <  jpart2.size(); i++) {
        const QJsonObject &j = jpart2.at(i).toObject();
        QDishPart2Button *btn = new QDishPart2Button();
        connect(btn, &QDishPart2Button::clicked, this, &DlgOrder::dishPart2Clicked);
        btn->setText(j["f_name"].toString());
        btn->setProperty("bgcolor", j["f_color"].toInt());
        btn->setProperty("id", j["f_id"].toInt());
        btn->setProperty("children", j["f_children"].toString());
        ui->glDishPart2->addWidget(btn, row, col++, 1, 1);

        if(col == colCount) {
            col = 0;
            row ++;
        }
    }

    ui->glDishPart2->setRowStretch(row + 1, 1);
    buildDishes(menuid, part2);
}

void DlgOrder::addDishToOrder(int menuid, const QString &emark)
{
    int dishid = dbmenu->dishid(menuid);

    if(dbdish->isHourlyPayment(dishid)) {
        for(int i = 0; i < fBody.count(); i++) {
            const QMap<QString, QVariant> jb = fBody.at(i);

            if(jb["f_state"].toInt() != DISH_STATE_OK) {
                continue;
            }

            if(dbdish->isHourlyPayment(jb["f_dish"].toInt())) {
                C5Message::error(tr("Hourly payment already exists"));
                return;
            }
        }
    }

    double price = 0;

    if(dbdish->isExtra(dishid)) {
        if(!DlgPassword::getAmount(tr("Extra price"), price)) {
            return;
        }

        if(price < 0.01) {
            C5Message::error(tr("Extra price is not defined"));
            return;
        }
    } else {
        price = dbmenu->price(menuid);
    }

    QString special;

    if(!DlgListDishSpecial::getSpecial(dishid, special)) {
        return;
    }

    double qty = 1;

    if(__c5config.getValue(param_rest_qty_before_add_dish).toInt() == 1 && emark.isEmpty()) {
        on_btnAnyqty_clicked();
    }

    QString comment = "";
    comment = special;
    fHttp->createHttpQueryLambda("/engine/picasso.waiter/",
    QJsonObject{
        {"class", "waiter"},
        {"method", "adddish"},
        {"f_header", fHeader["f_id"].toString()},
        {"f_dish", menuid},
        {"f_qty1", qty},
        {"f_price", price},
        {"f_special_comment", special},
        {"f_emarks", QJsonValue::Null},
        {"f_current_staff", fUser->id()},
        {"f_current_staff_name", fUser->fullName()},
        {"f_locksrc", hostinfo}},
    [this](const QJsonObject & jdoc) {
        updateData();
        ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->maximum() + 1000);
    }, [](const QJsonObject & je) {
        Q_UNUSED(je);
    });
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
    fHttp->createHttpQuery("/engine/waiter/stoplist.php", QJsonObject{{"action", "get"}}, SLOT(handleStopList(
                QJsonObject)));
}
void DlgOrder::logRecord(const QString & username, const QString & orderid, const QString & rec, const QString & action,
                         const QString & value1, const QString & value2)
{
    C5LogToServerThread::remember(LOG_WAITER, username, rec, orderid, "", action, value1, value2);
}

void DlgOrder::setButtonsState()
{
    bool btnSendToCooking = false;
    bool btnPayment = true;
    bool orderEmpty = true;
    QList<WOrder*> orders = worders();

    for(WOrder *o : orders) {
        for(int i = 0; i < o->fOrderDriver->dishesCount(); i++) {
            if(o->fOrderDriver->dishesValue("f_state", i).toInt() != DISH_STATE_OK) {
                continue;
            }

            orderEmpty = false;

            if(o->fOrderDriver->dishesValue("f_qty1", i).toDouble() > o->fOrderDriver->dishesValue("f_qty2", i).toDouble()) {
                btnSendToCooking = true;
            }
        }
    }

    if(btnSendToCooking || orderEmpty) {
        btnPayment = false;
    }

    ui->btnPrintService->setEnabled(btnSendToCooking);
    ui->btnTotal->setEnabled(false);
    ui->btnTotal->setProperty("stylesheet_button_redalert", false);
    ui->btnTotal->style()->polish(ui->btnTotal);
    ui->btnTotal->setText(QString("%1\n%2").arg(tr("Bill"), "-"));
    ui->wdtOrder->setEnabled(true);
    setRoomComment();
    setCLComment();
    setSelfcost();
    setDiscountComment();
    //TODO: CHECK
    //MOVE ALL LOGIC TO UPDATE DATE
    // ui->btnSit->setText(QString("%1: %2").arg(tr("Guests count"),
    //                     wo->fOrderDriver->headerOptionsValue("f_guests").toString()));
    // headerToLineEdit();
    // QString orderComment = wo->fOrderDriver->headerValue("f_comment").toString().isEmpty() ?
    //                        tr("Comment") : wo->fOrderDriver->headerValue("f_comment").toString();
    // ui->btnOrderComment->setText(orderComment);
    // auto *cbc = Configs::construct<CashboxConfig> (2);
    // if(wo->fOrderDriver->headerValue("f_precheck").toInt() > 0) {
    //     ui->btnTotal->setProperty("stylesheet_button_redalert", true);
    //     ui->btnTotal->style()->polish(ui->btnTotal);
    //     ui->btnTotal->setText(QString("%1\n%2")
    //                           .arg(tr("Bill"), float_str(wo->fOrderDriver->headerValue("f_amounttotal").toDouble(), 2)));
    //     ui->wpayment->setVisible(fUser->check(cp_t5_print_receipt));
    //     ui->wdtOrder->setEnabled(false);
    //     ui->btnService->setEnabled(false);
    //     ui->btnDiscount->setEnabled(false);
    //     ui->btnPaymentCash->setText(cbc->cash1Name);
    //     ui->btnPaymentCard->setText(cbc->cash2Name);
    //     ui->btnPaymentBank->setText(cbc->cash3Name);
    //     ui->btnPrepayment->setText(cbc->cash4Name);
    //     ui->btnPaymentIdram->setText(cbc->cash5Name);
    //     ui->btnPayX->setText(cbc->cash6Name);
    //     ui->btnPaymentOther->setText(cbc->cash7Name);
    //     headerToLineEdit();
    //     if(ui->wpayment->isVisible()) {
    //         ui->btnPayCityLedger->setEnabled(ui->leOther->getDouble() > 0.01);
    //         ui->btnPayComplimentary->setEnabled(ui->leOther->getDouble() > 0.01);
    //         ui->btnPayTransferToRoom->setEnabled(ui->leOther->getDouble() > 0.01 && cbc->cash6Hotel);
    //         ui->btnSelfCost->setEnabled(ui->leOther->getDouble() > 0.01);
    //     }
    // } else {
    //     ui->btnTotal->setProperty("stylesheet_button_redalert", false);
    //     ui->btnTotal->style()->polish(ui->btnTotal);
    //     ui->btnTotal->setText(QString("%1\n%2").arg(tr("Bill"),
    //                           float_str(wo->fOrderDriver->headerValue("f_amounttotal").toDouble(), 2)));
    //     ui->wpayment->setVisible(false);
    // }
    // ui->btnTotal->style()->polish(ui->btnTotal);
    // if(wo->fOrderDriver->headerValue("f_partner").toInt() > 0) {
    //     CPartners p;
    //     C5Database db;
    //     db[":f_id"] = wo->fOrderDriver->headerValue("f_partner");
    //     db.exec("select * from c_partners where f_id=:f_id");
    //     p.getRecord(db);
    //     ui->btnDelivery->setText(QString("%1, %2, %3").arg(p.phone, p.taxName, p.address));
    // }
    // ui->btnTotal->setEnabled(btnPayment
    //                          || wo->fOrderDriver->headerValue("f_state").toInt() == ORDER_STATE_PREORDER_EMPTY
    //                          || wo->fOrderDriver->headerValue("f_state").toInt() == ORDER_STATE_PREORDER_WITH_ORDER);
    // ui->wleft->setVisible(wo->fOrderDriver->headerValue("f_precheck").toInt() < 1);
    // ui->wdish->setVisible(wo->fOrderDriver->headerValue("f_precheck").toInt() < 1);
}
void DlgOrder::removeWOrder(WOrder * wo)
{
    for(int i = 0; i < ui->vs->count(); i++) {
        if(wo == ui->vs->itemAt(i)->widget()) {
            ui->vs->itemAt(i)->widget()->deleteLater();
            ui->vs->removeItem(ui->vs->itemAt(i));
            return;
        }
    }
}
void DlgOrder::restoreStoplistQty(int dish, double qty)
{
    fHttp->createHttpQuery("/engine/waiter/stoplist.php",
    QJsonObject{{"action", "restoreqty"}, {"f_dish", dish}, {"f_qty", qty}},
    SLOT(restoreStoplistQtyResponse(QJsonObject)));
}
void DlgOrder::timeout()
{
    ui->leCmd->setFocus();
    ui->lbTime->setText(QTime::currentTime().toString(FORMAT_TIME_TO_SHORT_STR));
    fTimerCounter++;

    if((fTimerCounter % 60) == 0) {
        for(WOrder *o : worders()) {
            o->fOrderDriver->amountTotal();
        }
    }
}
void DlgOrder::worderActivated()
{
    WOrder *a = static_cast<WOrder*>(sender());
    WOrder *o = nullptr;

    for(int i = 0; i < ui->vs->count(); i++) {
        o = dynamic_cast<WOrder*>(ui->vs->itemAt(i)->widget());

        if(o) {
            if(o == a) {
                o->setSelected();
            } else {
                o->setSelected(false);
            }
        }
    }

    setButtonsState();
}
void DlgOrder::handlePrintService(const QJsonObject & obj)
{
    fHttp->httpQueryFinished(sender());

    if(__c5config.getValue(param_waiter_closeorder_after_serviceprint).toInt() == 1) {
        on_btnExit_clicked();
        return;
    }

    //TODO FUCK RELOAD ORDER AFTER PRINT
    //load(worder()->fOrderDriver->headerValue("f_table").toInt());
}
void DlgOrder::handleStopList(const QJsonObject & obj)
{
    C5TableData::instance()->setStopList(obj["stoplist"].toArray());

    for(QObject *o : ui->wdishes->children()) {
        QWidget *w = dynamic_cast<QWidget*>(o);

        if(w) {
            w->repaint();
        }
    }

    fHttp->httpQueryFinished(sender());
}
void DlgOrder::restoreStoplistQtyResponse(const QJsonObject & jdoc)
{
    if(jdoc["ok"].toBool()) {
        C5TableData::instance()->mStopList[jdoc["f_dish"].toInt()] = jdoc["f_qty"].toDouble();
    } else {
        C5TableData::instance()->mStopList.remove(jdoc["f_dish"].toInt());
    }

    fHttp->httpQueryFinished(sender());
}
void DlgOrder::addStopListResponse(const QJsonObject & jdoc)
{
    C5TableData::instance()->mStopList[jdoc["f_dish"].toInt()] = jdoc["f_qty"].toDouble();
    fHttp->httpQueryFinished(sender());
}
void DlgOrder::checkStopListResponse(const QJsonObject & jdoc)
{
    if(jdoc["ok"].toBool()) {
        addDishToOrder(jdoc["f_menu"].toInt(), jdoc["emark"].toString());
        fHttp->httpQueryFinished(sender());
        return;
    }

    C5TableData::instance()->mStopList[jdoc["f_dish"].toInt()] = jdoc["f_newqty"].toDouble();
    addDishToOrder(jdoc["f_menu"].toInt(), "");
    fHttp->httpQueryFinished(sender());
}
void DlgOrder::handleError(int err, const QString & msg)
{
    Q_UNUSED(err);
    sender()->deleteLater();
    C5Message::error(msg);
}
void DlgOrder::dishpart1Clicked()
{
    fPart2Parent = 0;
    QPushButton *btn = static_cast<QPushButton*>(sender());
    buildMenu(fMenuID, btn->property("id").toInt(), 0);
}

void DlgOrder::qrListResponse(const QJsonObject & obj)
{
    //TODO: CHECK
    int index = -1;

    // for(int i = 0; i < wo->fOrderDriver->dishesCount(); i++) {
    //     // if(wo->fOrderDriver->dishesValue("f_id", i).toString() == obj["bodyid"].toString()) {
    //     //     index = i;
    //     //     break;
    //     // }
    // }

    if(index > -1) {
        //TODO: CHECK
        //wo->fOrderDriver->setDishesValue("f_emarks", obj["emarks"].toString(), index);
        updateData();
    }

    fHttp->httpQueryFinished(sender());
}

void DlgOrder::dishPart2Clicked()
{
    QDishPart2Button *btn = static_cast<QDishPart2Button*>(sender());
    int part2 = btn->property("id").toInt();
    QString children = btn->property("children").toString();
    Q_ASSERT(part2 > 0);

    if(!children.isEmpty()) {
        fPart2Parent = part2;
    }

    buildMenu(fMenuID, fPart1, part2);
}

void DlgOrder::dishClicked()
{
    QDishButton *btn = static_cast<QDishButton*>(sender());

    //TODO: CHECK
    if(fHeader["f_precheck"].toInt() > 0) {
        C5Message::error(tr("Cannot add new dish if precheck was printed"));
        return;
    }

    int menuid = btn->property("id").toInt();
    int dishid = dbmenu->dishid(menuid);

    if(fStoplistMode) {
        if(dbdish->isExtra(dishid)) {
            C5Message::error(tr("Cannot add special dish to stoplist"));
            return;
        }

        double max = 999;

        if(!DlgPassword::getAmount(dbdish->name(dishid), max, false)) {
            return;
        }

        fHttp->createHttpQuery("/engine/waiter/stoplist.php",
        QJsonObject{{"action", "add"}, {"f_dish", dishid}, {"f_qty", max}},
        SLOT(addStopListResponse(QJsonObject)));
    } else {
        if(dbdish->get(dishid, "f_addbyqr").toInt() > 0) {
            C5Message::error(tr("Add only with QR code"));
            return;
        }

        double price = 0;

        if(dbdish->isExtra(dishid)) {
            if(!DlgPassword::getAmount(tr("Extra price"), price)) {
                return;
            }

            if(price < 0.01) {
                C5Message::error(tr("Extra price is not defined"));
                return;
            }
        } else {
            price = dbmenu->price(menuid);
        }

        QString special, emarks;

        if(!DlgListDishSpecial::getSpecial(dishid, special)) {
            return;
        }

        double qty = 1;

        if(__c5config.getValue(param_rest_qty_before_add_dish).toInt() == 1 && emarks.isEmpty()) {
            if(!dbdish->isHourlyPayment(dishid)) {
                double max = 999;

                if(!DlgQty::getQty(max, dbdish->name(dishid))) {
                    return;
                }

                qty = max;
            }
        }

        fHttp->createHttpQueryLambda("/engine/v2/waiter/order/adddish",
        QJsonObject{{"action", "check"},
            {"f_header", fHeader["f_id"].toString()},
            {"f_menu", menuid},
            {"f_dish", dishid},
            {"f_row", 1},
            {"f_guest", 1},
            {"f_current_staff", __user->id()},
            {"emark", emarks},
            {"f_qty", 1}}, [](const QJsonObject & jdoc) {
        },
        [](const QJsonObject & err) {
        });
        //  checkStopListResponse(QJsonObject{});
        return;
    }
}

void DlgOrder::dishPartClicked()
{
    QPushButton *btn = static_cast<QPushButton*>(sender());
    int id = btn->property("id").toInt();
    buildMenu(fMenuID, fPart1, id);
}

void DlgOrder::on_btnExit_clicked()
{
    fHttp->createHttpQueryLambda("/engine/picasso.waiter/",
    QJsonObject{
        {"class", "waiter"},
        {"method", "closetable"},
        {"f_table", fHeader["f_table"].toInt()},
        {"f_id", fHeader["f_id"].toString()},
        {"f_current_staff", fUser->id()},
        {"f_current_staff_name", fUser->fullName()},
        {"f_locksrc", hostinfo}},
    [this](const QJsonObject & jdoc) {
        Q_UNUSED(jdoc);
        accept();
        emit(allDone());
    }, [this](const QJsonObject & je) {
        accept();
        emit(allDone());
        Q_UNUSED(je);
    }
                                );
}
void DlgOrder::on_btnVoid_clicked()
{
    C5User *tmp = fUser;
    int index = -1;

    for(int i = 0; i < ui->vlDishes->count(); i++) {
        auto dw = dynamic_cast<DishItem*>(ui->vlDishes->itemAt(i)->widget());

        if(!dw)  {
            continue;
        }

        if(dw->isFocused()) {
            index = i;
            break;
        }
    }

    if(index < 0) {
        return;
    }

    //TODO: DELETE *TMP
    //TODO: OPTIMIZE THIS SAVES
}
void DlgOrder::on_btnComment_clicked()
{
    //  int index = 0;
    //  //TODO: FIX MIGRATION
    //  // if(!wo->currentRow(index)) {
    //  //     return;
    //  // }
    //  if(wo->fOrderDriver->dishesValue("f_state", index).toInt() != DISH_STATE_OK) {
    //      return;
    //  }
    //  if(dbdish->isHourlyPayment(wo->fOrderDriver->dishesValue("f_dish", index).toInt())) {
    //      C5Message::error(tr("Cannot add comment to hourly payment"));
    //      return;
    //  }
    //  if(wo->fOrderDriver->dishesValue("f_qty2", index).toDouble() > 0.001) {
    //      C5Message::error(tr("Cannot add comment to dish that already printed"));
    //      return;
    //  }
    //  QString comment = wo->fOrderDriver->dishesValue("f_comment", index).toString();
    //  if(!DlgListOfDishComments::getComment(dbdish->name(wo->fOrderDriver->dishesValue("f_dish", index).toInt()), comment)) {
    //      return;
    //  }
    //  wo->fOrderDriver->setDishesValue("f_comment", comment, index);
    //  wo->updateItem(index);
    //  logRecord(fUser->fullName(), wo->fOrderDriver->headerValue("f_id").toString(),
    //            wo->fOrderDriver->dishesValue("f_id", index).toString(),
    //            "Comment for dish",
    //            dbdish->name(wo->fOrderDriver->dishesValue("f_dish", index).toInt()), comment);
}
void DlgOrder::on_btnChangeMenu_clicked()
{
    C5User *tmp = fUser;

    if(!tmp->check(cp_t5_change_menu)) {
        if(!DlgPassword::getUserAndCheck(tr("Change menu"), tmp, cp_t5_change_menu)) {
            return;
        }
    }

    int menuid;

    if(!DlgListOfMenu::getMenuId(menuid)) {
        return;
    }

    fMenuID = menuid;
    buildMenu(menuid, 0, 0);
}
void DlgOrder::on_btnSearchInMenu_clicked()
{
    DlgSearchInMenu *d = new DlgSearchInMenu();
    connect(d, SIGNAL(dish(int, QString)), this, SLOT(processMenuID(int, QString)));
    d->exec();
    delete d;
}
void DlgOrder::on_btnPackage_clicked()
{
    // int id;
    // QString name;
    // if(DlgListOfPackages::package(id, name)) {
    //     double max = 100;
    //     if(!DlgPassword::getAmount(name, max)) {
    //         return;
    //     }
    //     QList<int> lst = dbmenupackagelist->listOf(id);
    //     for(int id : lst) {
    //         wo->fOrderDriver->addDish2(id, max);
    //         DbMenuPackageList p(id);
    //         logRecord(fUser->fullName(), worder()->fOrderDriver->headerValue("f_id").toString(), "", "Dish in package",
    //                   QString("%1, %2 x %3").arg(dbdish->name(p.dish()))
    //                   .arg(p.qty())
    //                   .arg(p.price()), "");
    //     }
    // }
    // //TODO MIX FIX
    // updateData();
}
void DlgOrder::on_btnPrintService_clicked()
{
    // QList<WOrder*> orders = worders();
    // for(WOrder *o : orders) {
    //     for(int i = 0; i < o->fOrderDriver->dishesCount(); i++) {
    //         if(o->fOrderDriver->dishesValue("f_qty2", i).toDouble() < 0.001) {
    //             o->fOrderDriver->setDishesValue("f_printtime", QDateTime::currentDateTime(), i);
    //         }
    //     }
    //     if(!o->fOrderDriver->save()) {
    //         C5Message::error(worder()->fOrderDriver->error());
    //         return;
    //     }
    //     ui->btnPrintService->setEnabled(false);
    //     fHttp->createHttpQueryLambda("/engine/waiter/printservice.php", {
    //         {"order", o->fOrderDriver->currentOrderId()},
    //         {"alias", __c5config.getValue(param_force_use_print_alias).toInt()},
    //         {"timeorder", __c5config.getValue(param_print_dish_timeorder).toInt()},
    //         {"isBooking", false}
    //     }, [this](const QJsonObject & jo) {
    //         for(int i = 0; i  < jo["print"].toArray().size(); i++) {
    //             auto np = new NDataProvider(this);
    //             np->overwriteHost("http", "127.0.0.1", 8080);
    //             connect(np, &NDataProvider::done, this, [this](const QJsonObject & jjo) {
    //                 if(__c5config.getValue(param_waiter_closeorder_after_serviceprint).toInt() == 1) {
    //                     on_btnExit_clicked();
    //                     return;
    //                 }
    //                 //TODO FUCK
    //                 //load(worder()->fOrderDriver->headerValue("f_table").toInt());
    //             });
    //             connect(np, &NDataProvider::error, this, [this](const QString & err) {
    //                 ui->btnPrintService->setEnabled(true);
    //             });
    //             QJsonObject jp = jo["print"].toArray().at(i).toObject();
    //             np->getData("/printservice", jp);
    //         }
    //     }, [this](const QJsonObject & je) {
    //         ui->btnPrintService->setEnabled(true);
    //     });
    // }
}

void DlgOrder::on_btnSit_clicked()
{
    //TODO: CHECK
    // DlgGuests(worder()->fOrderDriver).exec();
    // itemsToTable();
}

void DlgOrder::on_btnMovement_clicked()
{
    C5User *tmp = fUser;

    if(!tmp->check(cp_t5_movetable)) {
        if(!DlgPassword::getUserAndCheck(tr("Move items"), tmp, cp_t5_movetable)) {
            return;
        }
    }

    DlgSplitOrder d(tmp);
    d.configOrder(fTable);
    d.exec();
    //TODO FUCK
    //load(fTable);
}

void DlgOrder::on_btnRecent_clicked()
{
    if(fMenuID == 0) {
        return;
    }

    while(ui->grDish->itemAt(0)) {
        ui->grDish->itemAt(0)->widget()->deleteLater();
        ui->grDish->removeItem(ui->grDish->itemAt(0));
    }

    QRect scr = qApp->screens().at(0)->geometry();
    int dcolCount = scr.width() > 1024 ? 3 : 2;
    int dcol = 0;
    int drow = 0;
    // for (DPart1 p1 : menu5->fMenuList.data[fMenuID]) {
    //     QList<DPart2> dpart2 = p1.data;
    //     for (const DPart2 &d2 : dpart2) {
    //         fetchDishes(d2, true, dcolCount, dcol, drow);
    //     }
    // }
    ui->grDish->setRowStretch(drow + 1, 1);
}

void DlgOrder::on_btnChangeStaff_clicked()
{
    //TODO: CHECK
    // WOrder *wo = worder();
    // if(!wo) {
    //     return;
    // }
    // C5User *tmp = fUser;
    // if(!tmp->check(cp_t5_change_staff_of_table)) {
    //     if(!DlgPassword::getUserAndCheck(tr("Change staff of order"), tmp, cp_t5_change_staff_of_table)) {
    //         return;
    //     }
    // }
    // QString code;
    // if(!DlgPassword::getPassword(tr("Change staff"), code)) {
    //     return;
    // }
    // //QString code = QInputDialog::getText(this, tr("Change staff"), tr("Staff password"), QLineEdit::Password, "", &ok);
    // //    if (!ok) {
    // //        return;
    // //    }
    // code = code.replace("?", "");
    // code = code.replace(";", "");
    // code = code.replace(":", "");
    // if(code.isEmpty()) {
    //     return;
    // }
    // C5User u(code);
    // if(!u.isValid()) {
    //     C5Message::error(u.error());
    //     return;
    // }
    // wo->fOrderDriver->setHeader("f_staff", u.id());
    // ui->btnChangeStaff->setText(QString("%1\n%2").arg(tr("Staff")).arg(u.fullName()));
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
    //TODO: CHECK L4
    // C5User *tmp = fUser;
    // C5Database db;
    // if(wo->fOrderDriver->headerValue("f_discountfactor").toDouble() > 0.001) {
    //     if(!tmp->check(cp_t5_cancel_discount)) {
    //         if(!DlgPassword::getUserAndCheck(tr("Remove discount"), tmp, cp_t5_cancel_discount)) {
    //             return;
    //         }
    //     }
    //     db[":f_id"] = wo->fOrderDriver->currentOrderId();
    //     db.exec("delete from b_history where f_id=:f_id");
    //     wo->fOrderDriver->setHeader("f_discountfactor", 0);
    //     for(int i = 0; i < wo->fOrderDriver->dishesCount(); i++) {
    //         wo->fOrderDriver->setDishesValue("f_discount", wo->fOrderDriver->headerValue("f_discountfactor"), i);
    //     }
    //     wo->fOrderDriver->amountTotal();
    //     updateData();
    //     wo->fOrderDriver->save();
    //     logRecord(tmp->fullName(), wo->fOrderDriver->currentOrderId(), "", "Discount removed", "", "");
    //     return;
    // }
    // bool ok;
    // QString code = QInputDialog::getText(this, tr("Card"), tr("Card code"), QLineEdit::Password, "", &ok);
    // if(!ok) {
    //     return;
    // }
    // code = code.replace("?", "");
    // code = code.replace(";", "");
    // code = code.replace(":", "");
    // if(code.isEmpty()) {
    //     C5Message::error(tr("Card code is empty"));
    //     return;
    // }
    // discountOrder(tmp, code);
}

void DlgOrder::on_btnTotal_clicked()
{
    // if(wo->fOrderDriver->headerValue("f_precheck").toInt() < 1) {
    //     auto *tmp = fUser;
    //     if(!tmp->check(cp_t5_print_precheck)) {
    //         if(!DlgPassword::getUserAndCheck(tr("Print precheck"), tmp, cp_t5_print_precheck)) {
    //             return;
    //         }
    //     }
    //     int withoutprint = 0;
    //     if(__c5config.getValue(param_waiter_ask_for_precheck).toInt() > 0) {
    //         withoutprint = DlgAskForPrecheck::get();
    //     }
    //     bool empty = true;
    //     for(int i = 0; i < wo->fOrderDriver->dishesCount(); i++) {
    //         if(wo->fOrderDriver->dishesValue("f_state", i).toInt() != DISH_STATE_OK) {
    //             continue;
    //         }
    //         if(wo->fOrderDriver->dishesValue("f_qty1", i).toDouble() > wo->fOrderDriver->dishesValue("f_qty2", i).toDouble()
    //                 && !(wo->fOrderDriver->headerValue("f_state").toInt() == ORDER_STATE_PREORDER_EMPTY
    //                      || wo->fOrderDriver->headerValue("f_state").toInt() == ORDER_STATE_PREORDER_WITH_ORDER)) {
    //             C5Message::error(tr("Order is incomplete"));
    //             return;
    //         }
    //         empty = false;
    //     }
    //     if(empty) {
    //         if(tmp != fUser) {
    //             delete tmp;
    //         }
    //         C5Message::error(tr("Order is incomplete"));
    //         return;
    //     }
    //     wo->fOrderDriver->amountTotal();
    //     wo->fOrderDriver->save();
    //     //TODO: CHECK L14
    //     fHttp->createHttpQueryLambda("/engine/waiter/printreceipt.php", {
    //         {"station", hostinfo},
    //         {"printer", C5Config::localReceiptPrinter()},
    //         {"order", worder()->fOrderDriver->currentOrderId()},
    //         {"language", C5Config::getRegValue("receipt_language").toInt()},
    //         {"receipt_printer", C5Config::fSettingsName},
    //         {"withoutprint", withoutprint},
    //         {"alias", __c5config.getValue(param_force_use_print_alias).toInt()}
    //     }, [this](const QJsonObject & jo) {
    //         auto np = new NDataProvider(this);
    //         np->overwriteHost("http", "127.0.0.1", 8080);
    //         connect(np, &NDataProvider::done, this, [](const QJsonObject & jjo) {
    //         });
    //         connect(np, &NDataProvider::error, this, [](const QString & err) {
    //         });
    //         np->getData("/printjson", jo);
    //     }, [](const QJsonObject & je) {
    //         Q_UNUSED(je);
    //     });
    //     if(tmp != fUser) {
    //         delete tmp;
    //     }
    // } else {
    //     int o = DlgPrecheckOptions::precheck();
    //     C5User *tmp = fUser;
    //     switch(o) {
    //     case PRECHECK_CANCEL: {
    //         if(!tmp->check(cp_t5_cancel_precheck)) {
    //             if(!DlgPassword::getUserAndCheck(tr("Cancel precheck"), tmp, cp_t5_cancel_precheck)) {
    //                 return;
    //             }
    //         }
    //         if(C5Message::question(tr("Confirm to cancel bill")) != QDialog::Accepted) {
    //             return;
    //         }
    //         C5Database db;
    //         db[":f_id"] = wo->fOrderDriver->currentOrderId();
    //         db[":f_precheck"] = abs(wo->fOrderDriver->headerValue("f_precheck").toInt()) * -1;
    //         db.exec("update o_header set f_precheck=:f_precheck where f_id=:f_id");
    //         //TODO FUCK
    //         //load(fTable);
    //         break;
    //     }
    //     case PRECHECK_REPEAT:
    //         if(!tmp->check(cp_t5_repeat_precheck)) {
    //             if(!DlgPassword::getUserAndCheck(tr("Repeat precheck"), tmp, cp_t5_print_precheck)) {
    //                 return;
    //             }
    //         }
    //         //TODO: CHECK L15
    //         fHttp->createHttpQueryLambda("/engine/waiter/printreceipt.php", {
    //             {"station", hostinfo},
    //             {"printer", C5Config::localReceiptPrinter()},
    //             {"order", worder()->fOrderDriver->currentOrderId()},
    //             {"language", C5Config::getRegValue("receipt_language").toInt()},
    //             {"receipt_printer", C5Config::fSettingsName},
    //             {"repeath_precheck", true}
    //         }, [this](const QJsonObject & jo) {
    //             auto np = new NDataProvider(this);
    //             np->overwriteHost("http", "127.0.0.1", 8080);
    //             connect(np, &NDataProvider::done, this, [](const QJsonObject & jjo) {
    //             });
    //             connect(np, &NDataProvider::error, this, [](const QString & err) {
    //             });
    //             np->getData("/printjson", jo);
    //         }, [](const QJsonObject & je) {
    //             Q_UNUSED(je);
    //         });
    //         break;
    //     }
    //     if(tmp != fUser) {
    //         delete tmp;
    //     }
    // }
}

void DlgOrder::on_btnReceiptLanguage_clicked()
{
    int r = DlgReceiptLanguage::receipLanguage();

    if(r > -1) {
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
    switch(C5Config::getRegValue("receipt_language").toInt()) {
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

void DlgOrder::calcAmount(C5LineEdit * l)
{
    //TODO: CHECK L15
    // WOrder *wo = worder();
    // double max = wo->fOrderDriver->headerValue("f_amounttotal").toDouble();
    // if(!DlgPassword::getAmount(l->property("pay").toString(), max)) {
    //     return;
    // }
    // l->setDouble(max);
    // lineEditToHeader();
}

void DlgOrder::lineEditToHeader()
{
    //TODO: CHECK L15
    // wo->fOrderDriver->setHeader("f_amountcash", ui->leCash->getDouble());
    // wo->fOrderDriver->setHeader("f_amountcard", ui->leCard->getDouble());
    // wo->fOrderDriver->setHeader("f_amountbank", ui->leBank->getDouble());
    // wo->fOrderDriver->setHeader("f_amountprepaid", ui->lePrepaid->getDouble());
    // wo->fOrderDriver->setHeader("f_amountother", ui->leOther->getDouble());
    // wo->fOrderDriver->setHeader("f_amountidram", ui->leIDRAM->getDouble());
    // wo->fOrderDriver->setHeader("f_amountpayx", ui->lePayX->getDouble());
    // ui->leRemain->setDouble(wo->fOrderDriver->headerValue("f_amounttotal").toDouble()
    //                         - ui->leCash->getDouble()
    //                         - ui->leCard->getDouble()
    //                         - ui->leBank->getDouble()
    //                         - ui->lePrepaid->getDouble()
    //                         - ui->leOther->getDouble()
    //                         - ui->leIDRAM->getDouble()
    //                         - ui->lePayX->getDouble());
}

void DlgOrder::headerToLineEdit()
{
    //TODO: CHECK
    // ui->leCash->setDouble(wo->fOrderDriver->headerValue("f_amountcash").toDouble());
    // ui->leCard->setDouble(wo->fOrderDriver->headerValue("f_amountcard").toDouble());
    // ui->leBank->setDouble(wo->fOrderDriver->headerValue("f_amountbank").toDouble());
    // ui->lePrepaid->setDouble(wo->fOrderDriver->headerValue("f_amountprepaid").toDouble());
    // ui->leOther->setDouble(wo->fOrderDriver->headerValue("f_amountother").toDouble() +
    //                        wo->fOrderDriver->headerValue("f_hotel").toDouble());
    // ui->leIDRAM->setDouble(wo->fOrderDriver->headerValue("f_amountidram").toDouble());
    // ui->lePayX->setDouble(wo->fOrderDriver->headerValue("f_amountpayx").toDouble());
    // if(wo->fOrderDriver->preorder("f_prepaidcash").toDouble() > 0) {
    //     if(wo->fOrderDriver->headerValue("f_amountcash").toDouble() < 1) {
    //         wo->fOrderDriver->setHeader("f_amountcash",
    //                                     wo->fOrderDriver->preorder("f_prepaidcash").toDouble() > wo->fOrderDriver->headerValue("f_amounttotal").toDouble()
    //                                     ? wo->fOrderDriver->headerValue("f_amounttotal").toDouble()
    //                                     : wo->fOrderDriver->preorder("f_prepaidcash").toDouble());
    //         ui->leCash->setDouble(wo->fOrderDriver->headerValue("f_amountcash").toDouble());
    //     }
    // }
    // if(wo->fOrderDriver->preorder("f_prepaidcard").toDouble() > 0) {
    //     if(wo->fOrderDriver->headerValue("f_amountcard").toDouble() < 1) {
    //         wo->fOrderDriver->setHeader("f_amountcard",
    //                                     wo->fOrderDriver->preorder("f_prepaidcard").toDouble() > wo->fOrderDriver->headerValue("f_amounttotal").toDouble()
    //                                     ? wo->fOrderDriver->headerValue("f_amounttotal").toDouble()
    //                                     : wo->fOrderDriver->preorder("f_prepaidcard").toDouble());
    //         ui->leCard->setDouble(wo->fOrderDriver->headerValue("f_amountcard").toDouble());
    //     }
    // }
    // if(wo->fOrderDriver->preorder("f_prepaidpayx").toDouble() > 0) {
    //     if(wo->fOrderDriver->headerValue("f_amountpayx").toDouble() < 1) {
    //         wo->fOrderDriver->setHeader("f_amountpayx",
    //                                     wo->fOrderDriver->preorder("f_prepaidpayx").toDouble() > wo->fOrderDriver->headerValue("f_amounttotal").toDouble()
    //                                     ? wo->fOrderDriver->headerValue("f_amounttotal").toDouble()
    //                                     : wo->fOrderDriver->preorder("f_prepaidpayx").toDouble());
    //         ui->lePayX->setDouble(wo->fOrderDriver->headerValue("f_amountpayx").toDouble());
    //     }
    // }
    // ui->leRemain->setDouble(wo->fOrderDriver->headerValue("f_amounttotal").toDouble()
    //                         - ui->leCash->getDouble()
    //                         - ui->leCard->getDouble()
    //                         - ui->lePrepaid->getDouble()
    //                         - ui->leBank->getDouble()
    //                         - ui->leIDRAM->getDouble()
    //                         - ui->lePayX->getDouble()
    //                         - ui->leOther->getDouble());
    // ui->leReceived->clear();
    // ui->leChange->clear();
    // if(wo->fOrderDriver->headerValue("f_cash").toDouble() > 0.01) {
    //     wo->fOrderDriver->setHeader("f_change",
    //                                 wo->fOrderDriver->headerValue("f_cash").toDouble() - wo->fOrderDriver->headerValue("f_amounttotal").toDouble());
    //     ui->leReceived->setDouble(wo->fOrderDriver->headerValue("f_cash").toDouble());
    //     ui->leChange->setDouble(wo->fOrderDriver->headerValue("f_change").toDouble());
    // }
}

void DlgOrder::clearOther()
{
    //TODO: CHECK
    // wo->fOrderDriver->setRoom("", "", "", "");
    // wo->fOrderDriver->setCL("", "");
    // ui->leRoomComment->clear();
    // ui->leRoomComment->setVisible(false);
    // ui->lbRoom->setVisible(false);
}

void DlgOrder::setCLComment()
{
    //TODO: CHECK
    // bool v = !worder()->fOrderDriver->clValue("f_code").toString().isEmpty();
    // ui->leRoomComment->setVisible(v);
    // ui->lbRoom->setVisible(v);
    // if(v) {
    //     ui->leRoomComment->setText(worder()->fOrderDriver->clValue("f_code").toString() + ", " +
    //                                worder()->fOrderDriver->clValue("f_name").toString());
    // }
}
void DlgOrder::setComplimentary()
{
    //TODO: CHECK
    // if(worder()->fOrderDriver->headerValue("f_otherid").toInt() == PAYOTHER_COMPLIMENTARY) {
    //     ui->leRoomComment->setVisible(true);
    //     ui->leRoomComment->setText(tr("Complimentary"));
    //     ui->lbRoom->setVisible(true);
    // }
}

void DlgOrder::setRoomComment()
{
    //TODO: CHECK
    // if(WOrder *wo = worder()) {
    //     bool v = !wo->fOrderDriver->payRoomValue("f_room").toString().isEmpty();
    //     ui->leRoomComment->setVisible(v);
    //     ui->lbRoom->setVisible(v);
    //     if(v) {
    //         ui->leRoomComment->setText(wo->fOrderDriver->payRoomValue("f_room").toString() + ", " +
    //                                    wo->fOrderDriver->payRoomValue("f_guest").toString());
    //     }
    // }
}

void DlgOrder::setSelfcost()
{
    //TODO: CHECK
    // if(w->fOrderDriver->headerValue("f_otherid").toInt() == PAYOTHER_PRIMECOST) {
    //     ui->leRoomComment->setVisible(true);
    //     ui->lbRoom->setVisible(true);
    //     ui->leRoomComment->setText(tr("Prime cost"));
    // }
}

void DlgOrder::setDiscountComment()
{
//TODO: CHECK
    // ui->leDiscountComment->setVisible(false);
    // ui->lbDiscount->setVisible(false);
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
    //TODO: CHECK
    // if(ui->leCash->getDouble()
    //         + ui->leCard->getDouble()
    //         + ui->leBank->getDouble()
    //         + ui->lePrepaid->getDouble()
    //         + ui->leOther->getDouble() < worder()->fOrderDriver->headerValue("f_amounttotal").toDouble()) {
    //     return false;
    // }
    return true;
}

bool DlgOrder::buildDishes(int menuid, int part2)
{
    while(ui->grDish->itemAt(0)) {
        ui->grDish->itemAt(0)->widget()->deleteLater();
        ui->grDish->removeItem(ui->grDish->itemAt(0));
    }

    QRect scr = qApp->screens().at(0)->geometry();
    int dcolCount = scr.width() > 1024 ? 3 : 2;
    int dcol = 0;
    int drow = 0;
    QJsonArray jmenu = C5TableData::instance()->dishes(menuid, part2);

    for(int i = 0; i < jmenu.size(); i++) {
        const QJsonObject &j = jmenu.at(i).toObject();
        int w = (ui->wdishes->width() - 10)  / (scr.width() > 1024 ? 3 : 2);
        QDishButton *btn = new QDishButton(w);
        btn->setProperty("id", j["f_id"].toInt());
        btn->setProperty("price", j["f_price"].toDouble());
        btn->setProperty("name", j["f_name"].toString());
        btn->setProperty("color", j["f_color"].toInt());
        btn->setProperty("emarkrequired", j["f_addbyqr"].toInt() > 0);
        connect(btn, &QDishButton::clicked, this, &DlgOrder::dishClicked);
        ui->grDish->addWidget(btn, drow, dcol++, 1, 1);

        if(dcol == dcolCount) {
            dcol = 0;
            drow ++;
        }

        ui->grDish->setRowStretch(drow + 1, 1);
    }

    return true;
}

void DlgOrder::discountOrder(C5User * u, const QString & code)
{
    // C5Database db;
    // db[":f_id"] = worder()->fOrderDriver->currentOrderId();
    // db.exec("select * from b_history where f_id=:f_id");
    // if(db.nextRow()) {
    //     C5Message::error(tr("Bonus system alreay exists for this order"));
    //     return;
    // }
    // db[":f_code"] = code;
    // db.exec("select c.f_id, c.f_value, c.f_mode, cn.f_name, p.f_contact "
    //         "from b_cards_discount c "
    //         "left join c_partners p on p.f_id=c.f_client "
    //         "left join b_card_types cn on cn.f_id=c.f_mode "
    //         "where f_code=:f_code");
    // if(db.nextRow()) {
    //     switch(db.getInt("f_mode")) {
    //     case CARD_TYPE_DISCOUNT:
    //         wo->fOrderDriver->setHeader("f_discountfactor", db.getDouble("f_value") / 100.0);
    //         for(int i = 0; i < wo->fOrderDriver->dishesCount(); i++) {
    //             wo->fOrderDriver->setDishesValue("f_discount", wo->fOrderDriver->headerValue("f_discountfactor"), i);
    //         }
    //         break;
    //     case CARD_TYPE_ACCUMULATIVE:
    //         break;
    //     }
    //     db[":f_type"] = db.getInt("f_mode");
    //     db[":f_value"] = db.getDouble("f_value");
    //     db[":f_card"] = db.getInt("f_id");
    //     db[":f_data"] = 0;
    //     db[":f_id"] = wo->fOrderDriver->currentOrderId();
    //     db.insert("b_history");
    //     wo->fOrderDriver->amountTotal();
    //     itemsToTable();
    //     wo->fOrderDriver->save();
    //     logRecord(u->fullName(), wo->fOrderDriver->currentOrderId(), "", "Discount", "", "");
    // } else {
    //     C5Message::error(tr("Cannot find card"));
    //     return;
    // }
}
void DlgOrder::handleOrderDishClick(const QString & id)
{
    emit(orderDishClicked(id));
}
void DlgOrder::openReserveError(const QString & err)
{
    fHttp->fErrorSlot = nullptr;
    fHttp->httpQueryFinished(sender());

    if(err.contains("open reservation")) {
        emit allDone();
        reject();
        deleteLater();
    } else {
        C5Message::error(err);
    }
}
void DlgOrder::openReservationResponse(const QJsonObject & jdoc)
{
    fHttp->httpQueryFinished(sender());
    DlgPreorder d(jdoc);

    if(d.exec()  == QDialog::Accepted) {
        //TODO::: update info, guest name and
    }

    if(d.property("canceled").toBool()) {
        on_btnExit_clicked();
    }
}
void DlgOrder::openTableResponse(const QJsonObject & jdoc)
{
    // wo->fOrderDriver->fromJson(jdoc);
    // C5TableData::instance()->setStopList(jdoc["stopList"].toArray());
    // fTable = wo->fOrderDriver->headerValue("f_table").toInt();
    // fMenuID = C5Config::defaultMenu();
    // buildMenu(fMenuID, 0, 0);
    // on_btnRecent_clicked();
    // ui->btnTable->setText(tr("Table") + ": " + dbtable->name(wo->fOrderDriver->headerValue("f_table").toInt()));
    // ui->btnChangeStaff->setText(QString("%1\n%2").arg(tr("Staff"),
    //                             fUser->fullName())); //.arg(__c5config.autoDateCash() ? "" : " [" + __c5config.dateCash() + " / " + QString::number(__c5config.dateShift()) + "]"));
    // ui->lbVisit->clear();
    // ui->lbVisit->setVisible(false);
    // if(!__c5config.getValue(param_auto_discount).isEmpty()) {
    //     discountOrder(fUser, __c5config.getValue(param_auto_discount));
    // }
    // if(dbtable->hourlyId(wo->fOrderDriver->headerValue("f_table").toInt()) > 0) {
    //     addDishToOrder(dbtable->hourlyId(wo->fOrderDriver->headerValue("f_table").toInt()), "");
    // }
    // ui->vs->addStretch();
    // itemsToTable();
    // fHttp->httpQueryFinished(sender());
    // if(ex) {
    //     exec();
    // }
}
void DlgOrder::moveTableResponse(const QJsonObject & jdoc)
{
    fHttp->httpQueryFinished(sender());
    //TODO FUCK
    //load(jdoc["newTableId"].toInt());
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
    //TODO: CHECK
    // worder()->fOrderDriver->setHeader("f_amountcash", worder()->fOrderDriver->headerValue("f_amounttotal"));
    // worder()->fOrderDriver->setHeader("f_amountcard", 0);
    // worder()->fOrderDriver->setHeader("f_amountbank", 0);
    // worder()->fOrderDriver->setHeader("f_amountprepaid", 0);
    // worder()->fOrderDriver->setHeader("f_amountother", 0);
    // worder()->fOrderDriver->setHeader("f_amountpayx", 0);
    // worder()->fOrderDriver->setHeader("f_amountidram", 0);
    // worder()->fOrderDriver->setHeader("f_hotel", 0);
    // worder()->fOrderDriver->setHeader("f_otherid", 0);
    ui->leRemain->setDouble(0);
    clearOther();
    headerToLineEdit();
    setButtonsState();
}

void DlgOrder::on_btnPaymentCard_clicked()
{
    //TODO: CHECK
    ui->btnTax->setChecked(true);
    // worder()->fOrderDriver->setHeader("f_amountcash", 0);
    // worder()->fOrderDriver->setHeader("f_amountcard", worder()->fOrderDriver->headerValue("f_amounttotal"));
    // worder()->fOrderDriver->setHeader("f_amountbank", 0);
    // worder()->fOrderDriver->setHeader("f_amountprepaid", 0);
    // worder()->fOrderDriver->setHeader("f_amountother", 0);
    // worder()->fOrderDriver->setHeader("f_amountpayx", 0);
    // worder()->fOrderDriver->setHeader("f_amountidram", 0);
    // worder()->fOrderDriver->setHeader("f_hotel", 0);
    // worder()->fOrderDriver->setHeader("f_otherid", 0);
    ui->leRemain->setDouble(0);
    clearOther();
    headerToLineEdit();
    setButtonsState();
}

void DlgOrder::on_btnPaymentBank_clicked()
{
    ui->btnTax->setChecked(false);
    //TODO: CHECK
    // worder()->fOrderDriver->setHeader("f_amountcash", 0);
    // worder()->fOrderDriver->setHeader("f_amountcard", 0);
    // worder()->fOrderDriver->setHeader("f_amountbank", worder()->fOrderDriver->headerValue("f_amounttotal"));
    // worder()->fOrderDriver->setHeader("f_amountprepaid", 0);
    // worder()->fOrderDriver->setHeader("f_amountother", 0);
    // worder()->fOrderDriver->setHeader("f_amountpayx", 0);
    // worder()->fOrderDriver->setHeader("f_amountidram", 0);
    // worder()->fOrderDriver->setHeader("f_otherid", 0);
    // worder()->fOrderDriver->setHeader("f_hotel", 0);
    ui->leRemain->setDouble(0);
    clearOther();
    headerToLineEdit();
    setButtonsState();
}

void DlgOrder::on_btnPrepayment_clicked()
{
    ui->btnTax->setChecked(true);
    //TODO: CHECK
    // worder()->fOrderDriver->setHeader("f_amountcash", 0);
    // worder()->fOrderDriver->setHeader("f_amountcard", 0);
    // worder()->fOrderDriver->setHeader("f_amountbank", 0);
    // worder()->fOrderDriver->setHeader("f_amountprepaid", worder()->fOrderDriver->headerValue("f_amounttotal"));
    // worder()->fOrderDriver->setHeader("f_amountother", 0);
    // worder()->fOrderDriver->setHeader("f_amountpayx", 0);
    // worder()->fOrderDriver->setHeader("f_amountidram", 0);
    // worder()->fOrderDriver->setHeader("f_otherid", 0);
    // worder()->fOrderDriver->setHeader("f_hotel", 0);
    ui->leRemain->setDouble(0);
    clearOther();
    headerToLineEdit();
    setButtonsState();
}

void DlgOrder::on_btnPaymentOther_clicked()
{
    ui->btnTax->setChecked(false);
    //TODO: CHECK
    fHeader["f_amountcash"] = 0;
    fHeader["f_amountcard"] = 0;
    fHeader["f_amountbank"] = 0;
    fHeader["f_amountprepaid"] = 0;
    fHeader["f_amountidram"] = 0;
    fHeader["f_hotel"] =  0;
    fHeader["f_amountother"] = fHeader["f_amounttotal"];
    ui->leRemain->setDouble(0);
    headerToLineEdit();
    setButtonsState();
}

void DlgOrder::on_btnPayCityLedger_clicked()
{
    // QString clCode, clName;
    // if(DlgCL::getCL(clCode, clName)) {
    //     worder()->fOrderDriver->setHeader("f_otherid", PAYOTHER_CL);
    //     worder()->fOrderDriver->setCL(clCode, clName);
    //     setCLComment();
    // }
}

void DlgOrder::on_btnPayComplimentary_clicked()
{
    // clearOther();
    // worder()->fOrderDriver->setHeader("f_otherid", PAYOTHER_COMPLIMENTARY);
    // setComplimentary();
}

void DlgOrder::on_btnPayTransferToRoom_clicked()
{
    // QString res, inv, room, guest;
    // if(DlgGuest::getGuest(res, inv, room, guest)) {
    //     if(!wo->fOrderDriver->setRoom(res, inv, room, guest)) {
    //         C5Message::error(wo->fOrderDriver->error());
    //         return;
    //     }
    //     wo->fOrderDriver->setHeader("f_amountother", 0);
    //     wo->fOrderDriver->setHeader("f_hotel", wo->fOrderDriver->amountTotal());
    //     setRoomComment();
    // }
}
void DlgOrder::on_btnReceipt_clicked()
{
    // C5User *tmp = fUser;
    // if(!tmp->check(cp_t5_print_receipt)) {
    //     if(!DlgPassword::getUserAndCheck(tr("Print receipt"), tmp, cp_t5_print_receipt)) {
    //         return;
    //     }
    // }
    // if(C5Message::question(tr("Confirm to close order")) != QDialog::Accepted) {
    //     return;
    // }
    // if(wo->fOrderDriver->headerValue("f_amountother").toDouble() > 0.001) {
    //     if(wo->fOrderDriver->headerValue("f_otherid").toInt() == 0) {
    //         C5Message::error(tr("Other method is not selected"));
    //         return;
    //     }
    // }
    // if(wo->fOrderDriver->headerValue("f_amountcash").toDouble()
    //         + wo->fOrderDriver->headerValue("f_amountcard").toDouble()
    //         + wo->fOrderDriver->headerValue("f_amountother").toDouble()
    //         + wo->fOrderDriver->headerValue("f_amountprepaid").toDouble()
    //         + wo->fOrderDriver->headerValue("f_amountbank").toDouble()
    //         + wo->fOrderDriver->headerValue("f_amountidram").toDouble()
    //         + wo->fOrderDriver->headerValue("f_hotel").toDouble()
    //         + wo->fOrderDriver->headerValue("f_amountpayx").toDouble() <
    //         wo->fOrderDriver->headerValue("f_amounttotal").toDouble()) {
    //     C5Message::error(tr("Check the all payment methods"));
    //     return;
    // }
    // double a = wo->fOrderDriver->headerValue("f_amountcash").toDouble()
    //            + wo->fOrderDriver->headerValue("f_amountcard").toDouble()
    //            + wo->fOrderDriver->headerValue("f_amountother").toDouble()
    //            + wo->fOrderDriver->headerValue("f_hotel").toDouble()
    //            + wo->fOrderDriver->headerValue("f_amountprepaid").toDouble()
    //            + wo->fOrderDriver->headerValue("f_amountbank").toDouble()
    //            + wo->fOrderDriver->headerValue("f_amountidram").toDouble()
    //            + wo->fOrderDriver->headerValue("f_amountpayx").toDouble();
    // if(a > wo->fOrderDriver->headerValue("f_amounttotal").toDouble()) {
    //     C5Message::error(QString("%1, %2/%3").arg(tr("Total amount of payments methods greater than total amount"))
    //                      .arg(a)
    //                      .arg(wo->fOrderDriver->headerValue("f_amounttotal").toDouble()));
    //     return;
    // }
    // if(!wo->fOrderDriver->save()) {
    //     C5Message::error(wo->fOrderDriver->error());
    //     return;
    // }
    // ui->btnExit->setEnabled(false);
    // //LOG
    // QString payMethods;
    // QString amounts;
    // if(wo->fOrderDriver->headerValue("f_amountcash").toDouble() > 0.001) {
    //     payMethods += payMethods.isEmpty() ? "Cash" : payMethods + " / Cash";
    //     amounts += amounts.isEmpty() ? float_str(wo->fOrderDriver->headerValue("f_amountcash").toDouble(),
    //                2) : amounts + " / " + float_str(wo->fOrderDriver->headerValue("f_amountcash").toDouble(), 2);
    // }
    // if(wo->fOrderDriver->headerValue("f_amountcard").toDouble() > 0.001) {
    //     payMethods += payMethods.isEmpty() ? "Cash" : payMethods + " / Card";
    //     //amounts += amounts.isEmpty() ? fOrder->hString("f_amountcard") : amounts + " / " + fOrder->hString("f_amountcard");
    //     amounts += amounts.isEmpty() ? float_str(wo->fOrderDriver->headerValue("f_amountcard").toDouble(),
    //                2) : amounts + " / " + float_str(wo->fOrderDriver->headerValue("f_amountcard").toDouble(), 2);
    // }
    // if(wo->fOrderDriver->headerValue("f_amountbank").toDouble() > 0.001) {
    //     payMethods = "Bank";
    //     amounts = float_str(wo->fOrderDriver->headerValue("f_amountbank").toDouble(), 2);
    // }
    // if(wo->fOrderDriver->headerValue("f_amountother").toDouble() > 0.001) {
    //     switch(wo->fOrderDriver->headerValue("f_otherid").toInt()) {
    //     case PAYOTHER_TRANSFER_TO_ROOM:
    //         payMethods = wo->fOrderDriver->payRoomValue("f_room").toString() + ","
    //                      + wo->fOrderDriver->payRoomValue("f_inv").toString() + ","
    //                      + wo->fOrderDriver->payRoomValue("f_guest").toString();
    //         break;
    //     case PAYOTHER_CL:
    //         payMethods = wo->fOrderDriver->clValue("f_code").toString() + "," + wo->fOrderDriver->clValue("f_name").toString();
    //         break;
    //     case PAYOTHER_COMPLIMENTARY:
    //         payMethods = "Complimentary";
    //         break;
    //     case PAYOTHER_PRIMECOST:
    //         payMethods = "Selfcost";
    //         break;
    //     default:
    //         payMethods = QString("%1 (%2)").arg("Unknown").arg(wo->fOrderDriver->headerValue("f_otherid").toInt());
    //         break;
    //     }
    //     amounts = float_str(wo->fOrderDriver->headerValue("f_amountother").toDouble(), 2);
    // }
    // amounts += ui->btnTax->isChecked() ? " Fiscal: yes" : " Fiscal: no";
    // C5LogToServerThread::remember(LOG_WAITER, tmp->fullName(), "", wo->fOrderDriver->currentOrderId(), "", "Receipt",
    //                               payMethods, amounts);
    // C5Database db;
    // db[":f_id"] = wo->fOrderDriver->headerValue("f_id");
    // db.exec("select f_type, f_value, f_card from b_history where f_id=:f_id");
    // if(db.nextRow()) {
    //     int mode = db.getInt("f_mode");
    //     int card = db.getInt("f_card");
    //     double value = db.getDouble("f_value") / 100;
    //     switch(mode) {
    //     case CARD_TYPE_ACCUMULATIVE:
    //         value = value * wo->fOrderDriver->headerValue("f_amounttotal").toDouble();
    //         db[":f_card"] = card;
    //         db[":f_trsale"] = wo->fOrderDriver->headerValue("f_id");
    //         db[":f_amount"] = value;
    //         db.insert("b_gift_card_history", false);
    //         db[":f_data"] = value;
    //         db.update("b_history", "f_id", wo->fOrderDriver->headerValue("f_id"));
    //     }
    // }
    // //TODO: CHECK L16
    // fHttp->createHttpQueryLambda("/engine/waiter/printreceipt", {
    //     { "station", hostinfo},
    //     {"printer", C5Config::localReceiptPrinter()},
    //     {"order", wo->fOrderDriver->currentOrderId()},
    //     {"language", C5Config::getRegValue("receipt_language").toInt()},
    //     {"printtax", ui->btnTax->isChecked() ? 1 : 0},
    //     {"receipt_printer", C5Config::fSettingsName},
    //     {"alias", __c5config.getValue(param_force_use_print_alias).toInt()},
    //     {"close", 1},
    //     {"nofinalreceipt", __c5config.getValue(param_waiter_dontprint_final_receipt).toInt()}
    // },
    // [this](const QJsonObject & jo) {
    //     ui->btnExit->setEnabled(true);
    //     fHttp->httpQueryFinished(sender());
    //     if(jo["reply"].toInt() == 0) {
    //         C5Message::error(jo["msg"].toString());
    //         return;
    //     }
    //     auto *wo = worder();
    //     if(jo["close"].toInt() == 1) {
    //         C5LogToServerThread::remember(LOG_WAITER, fUser->fullName(), "", wo->fOrderDriver->currentOrderId(), "", "Close order",
    //                                       "", "");
    //         if(!wo->fOrderDriver->closeOrder()) {
    //             C5Message::error(wo->fOrderDriver->error());
    //             return;
    //         }
    //         removeWOrder(wo);
    //         if(!worder()) {
    //             setProperty("reprint", jo["order"].toString());
    //             accept();
    //             return;
    //         }
    //     } else {
    //         if(__c5config.getValue(param_waiter_close_order_after_precheck).toInt() == 1) {
    //             ui->btnExit->click();
    //             return;
    //         }
    //         //TODO FUCK
    //         //load(wo->fOrderDriver->headerValue("f_table").toInt());
    //     }
    // }, [](const QJsonObject & je) {
    //     Q_UNUSED(je);
    // });
}

void DlgOrder::on_btnCloseOrder_clicked()
{
    //TODO: CHECK
    // if(C5Message::question(tr("Confirm to close order")) != QDialog::Accepted) {
    //     return;
    // }
    // WOrder *wo = worder();
    // QString orderid = wo->fOrderDriver->currentOrderId();
    // if(!wo->fOrderDriver->closeOrder()) {
    //     C5Message::error(wo->fOrderDriver->error());
    //     return;
    // }
    // C5LogToServerThread::remember(LOG_WAITER, fUser->fullName(), "", orderid, "", "Close order", "", "");
    // removeWOrder(wo);
    // if(!worder()) {
    //     accept();
    //     return;
    // }
    // updateData();
    // setButtonsState();
}

void DlgOrder::on_btnService_clicked()
{
    C5User *tmp = fUser;

    if(!tmp->check(cp_t5_change_service_value)) {
        if(!DlgPassword::getUserAndCheck(tr("Change service value"), tmp, cp_t5_change_service_value)) {
            return;
        }
    }

    C5Database db;
    db.exec("select * from o_service_values");
    QStringList lst;

    while(db.nextRow()) {
        lst.append(float_str(db.getDouble("f_value") * 100, 2));
    }

    int index = 0;

    if(!DlgList::getValue(lst, index)) {
        return;
    }

    QList<WOrder*> wos = worders();

    for(WOrder *wo : qAsConst(wos)) {
        QString oldValue = wo->fOrderDriver->headerValue("f_servicefactor").toString();
        wo->fOrderDriver->setHeader("f_servicefactor", lst.at(index).toDouble() / 100);
        wo->fOrderDriver->amountTotal();
        wo->fOrderDriver->save();
        logRecord(tmp->fullName(), wo->fOrderDriver->headerValue("f_id").toString(), "", "Change service factor", oldValue,
                  wo->fOrderDriver->headerValue("f_servicefactor").toString());
    }

    updateData();
}

void DlgOrder::on_btnTax_clicked()
{
    QSettings _ls(qApp->applicationDirPath() + "/ls.inf", QSettings::IniFormat);
    bool taxforce = _ls.value("ls/forcetax").toInt() == 1;
    bool notax = _ls.value("ls/notax").toInt() == 1;

    if(ui->leOther->getDouble() > 0.001) {
        ui->btnTax->setChecked(false);
    }

    if(ui->leCard->getDouble() > 0.001) {
        ui->btnTax->setChecked(true && !taxforce);
    }

    if(ui->lePrepaid->getDouble() > 0.001) {
        ui->btnTax->setChecked(true && !taxforce);
    }

    if(notax) {
        ui->btnTax->setChecked(false);
    }

    C5Config::setRegValue("btn_tax_state", ui->btnTax->isChecked() ? 1 : 2);
}

void DlgOrder::on_btnStopListMode_clicked()
{
    C5User *tmp = fUser;

    if(tmp->check(cp_t5_stoplist) == false) {
        if(!DlgPassword::getUserAndCheck(tr("Reprint service check"), tmp, cp_t5_stoplist)) {
            return;
        }
    }

    DlgStopListOption d(this, tmp);
    d.exec();

    if(tmp != fUser) {
        delete tmp;
    }
}

void DlgOrder::on_btnOrderComment_clicked()
{
    // QString comment = wo->fOrderDriver->headerValue("f_comment").toString();
    // if(!DlgText::getText(tr("Order comment"), comment)) {
    //     return;
    // }
    // wo->fOrderDriver->setHeader("f_comment", comment);
    // setButtonsState();
}

void DlgOrder::on_btnPlus1_clicked()
{
    //   int index = 0;
    //   //TODO: FIX MIGRATION
    //   // if(!wo->currentRow(index)) {
    //   //     return;
    //   // }
    //   if(wo->fOrderDriver->dishesValue("f_state", index).toInt() != DISH_STATE_OK) {
    //       return;
    //   }
    //   if(dbdish->isHourlyPayment(wo->fOrderDriver->dishesValue("f_dish", index).toInt())) {
    //       C5Message::error(tr("Cannot add comment to hourly payment"));
    //       return;
    //   }
    //   if(wo->fOrderDriver->dishesValue("f_emarks", index).toString().isEmpty() == false) {
    //       C5Message::error(tr("Cannot change qty of dishes thats contains emarks"));
    //       return;
    //   }
    //   if(wo->fOrderDriver->dishesValue("f_qty2", index).toDouble() > 0.0001) {
    //       QString special;
    //       if(!wo->fOrderDriver->dishesValue("f_comment2", index).toString().isEmpty()) {
    //           if(!DlgListDishSpecial::getSpecial(wo->fOrderDriver->dishesValue("f_dish", index).toInt(), special)) {
    //               return;
    //           }
    //       }
    //       int newindex = wo->fOrderDriver->duplicateDish(index);
    //       wo->fOrderDriver->setDishesValue("f_qty1", 1, newindex);
    //       wo->fOrderDriver->setDishesValue("f_qty2", 0, newindex);
    //       wo->fOrderDriver->setDishesValue("f_comment2", special, newindex);
    //       wo->fOrderDriver->setDishesValue("f_comment", "", newindex);
    //       logRecord(fUser->fullName(),
    //                 wo->fOrderDriver->headerValue("f_id").toString(),
    //                 wo->fOrderDriver->dishesValue("f_id", newindex).toString(),
    //                 "Copy dish " + dbdish->name(wo->fOrderDriver->dishesValue("f_dish", index).toInt()),
    //                 "from: " + wo->fOrderDriver->dishesValue("f_id", index).toString(),
    //                 wo->fOrderDriver->dishesValue("f_qty1", newindex).toString());
    //       itemsToTable();
    //       wo->setCurrentRow(newindex);
    //   } else {
    //       QString oldQty = wo->fOrderDriver->dishesValue("f_qty1", index).toString();
    //       wo->fOrderDriver->setDishesValue("f_qty1", wo->fOrderDriver->dishesValue("f_qty1", index).toDouble() + 1, index);
    //       itemsToTable();
    //       logRecord(fUser->fullName(), wo->fOrderDriver->headerValue("f_id").toString(),
    //                 wo->fOrderDriver->dishesValue("f_id", index).toString(),
    //                 "Qty of " + dbdish->name(wo->fOrderDriver->dishesValue("f_dish", index).toInt()),
    //                 oldQty,
    //                 wo->fOrderDriver->dishesValue("f_qty1", index).toString());
    //   }
}

void DlgOrder::on_btnMinus1_clicked()
{
    //  int index = 0;
    //  //TODO: FIX MIGRATION
    //  // if(!wo->currentRow(index)) {
    //  //     return;
    //  // }
    //  if(wo->fOrderDriver->dishesValue("f_state", index).toInt() != DISH_STATE_OK) {
    //      return;
    //  }
    //  if(dbdish->isHourlyPayment(wo->fOrderDriver->dishesValue("f_dish", index).toInt())) {
    //      C5Message::error(tr("Cannot add comment to hourly payment"));
    //      return;
    //  }
    //  if(wo->fOrderDriver->dishesValue("f_qty2", index).toDouble() > 0.0001) {
    //      C5Message::error(tr("Use removal tool"));
    //      return;
    //  } else {
    //      if(wo->fOrderDriver->dishesValue("f_qty1", index).toDouble() - 1 > 0.001) {
    //          QString oldQty = wo->fOrderDriver->dishesValue("f_qty1", index).toString();
    //          wo->fOrderDriver->setDishesValue("f_qty1", wo->fOrderDriver->dishesValue("f_qty1", index).toDouble() - 1, index);
    //          logRecord(fUser->fullName(), wo->fOrderDriver->headerValue("f_id").toString(),
    //                    wo->fOrderDriver->dishesValue("f_id", index).toString(),
    //                    "Qty of " + dbdish->name(wo->fOrderDriver->dishesValue("f_dish", index).toInt()),
    //                    oldQty,
    //                    wo->fOrderDriver->dishesValue("f_qty1", index).toString());
    //      } else if(C5Message::question(tr("Do you want to remove this item")) == QDialog::Accepted) {
    //          wo->fOrderDriver->setDishesValue("f_state", DISH_STATE_NONE, index);
    //          wo->fOrderDriver->setDishesValue("f_emarks", QVariant(), index);
    //          wo->fOrderDriver->save();
    //          logRecord(fUser->fullName(), wo->fOrderDriver->headerValue("f_id").toString(),
    //                    wo->fOrderDriver->dishesValue("f_id", index).toString(),
    //                    "Remove not printed " + dbdish->name(wo->fOrderDriver->dishesValue("f_dish", index).toInt()),
    //                    "-",
    //                    wo->fOrderDriver->dishesValue("f_qty1", index).toString());
    //      }
    //      itemsToTable();
    //  }
}

void DlgOrder::on_btnAnyqty_clicked()
{
    //  int index = 0;
    //  //TODO: FIX MIGRATION
    //  // if(!wo->currentRow(index)) {
    //  //     return;
    //  // }
    //  if(wo->fOrderDriver->dishesValue("f_state", index).toInt() != DISH_STATE_OK) {
    //      return;
    //  }
    //  if(dbdish->isHourlyPayment(wo->fOrderDriver->dishesValue("f_dish", index).toInt())) {
    //      C5Message::error(tr("Cannot add comment to hourly payment"));
    //      return;
    //  }
    //  if(wo->fOrderDriver->dishesValue("f_qty2", index).toDouble() > 0.0001) {
    //      C5Message::error(tr("Use removal tool"));
    //      return;
    //  }
    //  if(wo->fOrderDriver->dishesValue("f_emarks", index).toString().isEmpty() == false) {
    //      C5Message::error(tr("Cannot change qty of dishes thats contains emarks"));
    //      return;
    //  }
    //  if(dbdish->emarks(wo->fOrderDriver->dishesValue("f_dish", index).toInt()).isEmpty() == false) {
    //      C5Message::error(tr("Cannot change qty of dishes thats contains emarks"));
    //      return;
    //  }
    //  //TODO: PACKAGE HANDLER
    //  //    if (wo->fOrderDriver->dishesValue("d_package", index).toInt() > 0) {
    //  //        C5Message::error(tr("You cannot change the quantity of items of package"));
    //  //        return;
    //  //    }
    //  if(dbdish->isHourlyPayment(wo->fOrderDriver->dishesValue("f_dish", index).toInt())) {
    //      C5Message::error(tr("This is hourly payment item"));
    //      return;
    //  }
    //  double max = 999;
    //  if(!DlgQty::getQty(max, dbdish->name(wo->fOrderDriver->dishesValue("f_dish", index).toInt()))) {
    //      return;
    //  }
    //  QString oldQty = wo->fOrderDriver->dishesValue("f_qty1", index).toString();
    //  wo->fOrderDriver->setDishesValue("f_qty1", max, index);
    //  wo->updateItem(index);
    //  updateData();
    //  logRecord(fUser->fullName(), wo->fOrderDriver->headerValue("f_id").toString(),
    //            wo->fOrderDriver->dishesValue("f_id", index).toString(),
    //            "Qty of " + dbdish->name(wo->fOrderDriver->dishesValue("f_dish", index).toInt()),
    //            oldQty,
    //            wo->fOrderDriver->dishesValue("f_qty1", index).toString());
}

void DlgOrder::on_btnDishPart2Down_clicked()
{
    ui->scrollAreaPart2->verticalScrollBar()->setValue(ui->scrollAreaPart2->verticalScrollBar()->value() + 300);

    for(QObject *o : ui->scrollAreaWidgetContents->children()) {
        QWidget *w = dynamic_cast<QWidget*>(o);

        if(w) {
            w->repaint();
        }
    }
}

void DlgOrder::on_btnDishPart2Up_clicked()
{
    ui->scrollAreaPart2->verticalScrollBar()->setValue(ui->scrollAreaPart2->verticalScrollBar()->value() - 300);

    for(QObject *o : ui->scrollAreaWidgetContents->children()) {
        QWidget *w = dynamic_cast<QWidget*>(o);

        if(w) {
            w->repaint();
        }
    }
}

void DlgOrder::on_btnBillWithoutService_clicked()
{
    //TODO: CHECK
    //C5Message::info(QString("%1<br>%2").arg(tr("Counted")).arg(float_str(wo->fOrderDriver->clearAmount(), 2)));
}

void DlgOrder::on_btnFillCash_clicked()
{
    //TODO: CHECK
    // worder()->fOrderDriver->setHeader("f_amountcash",
    //                                   worder()->fOrderDriver->headerValue("f_amounttotal").toDouble()
    //                                   - worder()->fOrderDriver->headerHotel("f_amountbank").toDouble()
    //                                   - worder()->fOrderDriver->headerValue("f_amountcard").toDouble()
    //                                   - worder()->fOrderDriver->headerValue("f_amountprepaid").toDouble()
    //                                   - worder()->fOrderDriver->headerValue("f_amountidram").toDouble()
    //                                   - worder()->fOrderDriver->headerValue("f_amountpayx").toDouble());
    // worder()->fOrderDriver->setHeader("f_amountother", 0);
    ui->leRemain->setDouble(0);
    clearOther();
    headerToLineEdit();
    setButtonsState();
}

void DlgOrder::on_btnFillCard_clicked()
{
    //TODO: CHECK
    // worder()->fOrderDriver->setHeader("f_amountcard",
    //                                   worder()->fOrderDriver->headerValue("f_amounttotal").toDouble()
    //                                   - worder()->fOrderDriver->headerValue("f_amountbank").toDouble()
    //                                   - worder()->fOrderDriver->headerValue("f_amountcash").toDouble()
    //                                   - worder()->fOrderDriver->headerValue("f_amountprepaid").toDouble()
    //                                   - worder()->fOrderDriver->headerValue("f_amountidram").toDouble()
    //                                   - worder()->fOrderDriver->headerValue("f_amountpayx").toDouble());
    // worder()->fOrderDriver->setHeader("f_amountother", 0);
    ui->leRemain->setDouble(0);
    clearOther();
    headerToLineEdit();
    setButtonsState();
}

void DlgOrder::on_btnFillPrepaiment_clicked()
{
    //TODO: CHECK
    // worder()->fOrderDriver->setHeader("f_amountprepaid",
    //                                   worder()->fOrderDriver->headerValue("f_amounttotal").toDouble()
    //                                   - worder()->fOrderDriver->headerValue("f_amountcash").toDouble()
    //                                   - worder()->fOrderDriver->headerValue("f_amountcard").toDouble()
    //                                   - worder()->fOrderDriver->headerValue("f_amountbank").toDouble()
    //                                   - worder()->fOrderDriver->headerValue("f_amountidram").toDouble()
    //                                   - worder()->fOrderDriver->headerValue("f_amountpayx").toDouble());
    // worder()->fOrderDriver->setHeader("f_amountother", 0);
    ui->leRemain->setDouble(0);
    clearOther();
    headerToLineEdit();
    setButtonsState();
}

void DlgOrder::on_btnDishDown_clicked()
{
    ui->scrollAreaDish->verticalScrollBar()->setValue(ui->scrollAreaDish->verticalScrollBar()->value() + 300);

    for(QObject *o : ui->scrollAreaWidgetContentsDish->children()) {
        QWidget *w = dynamic_cast<QWidget*>(o);

        if(w) {
            w->repaint();
        }
    }

    ui->scrollAreaDish->viewport()->update();
}

void DlgOrder::on_btnDishUp_clicked()
{
    ui->scrollAreaDish->verticalScrollBar()->setValue(ui->scrollAreaDish->verticalScrollBar()->value() - 300);

    for(QObject *o : ui->scrollAreaWidgetContentsDish->children()) {
        QWidget *w = dynamic_cast<QWidget*>(o);

        if(w) {
            w->repaint();
        }
    }

    ui->scrollAreaDish->viewport()->update();
}

void DlgOrder::on_btnPreorder_clicked()
{
    //TODO: CHECK
    // fHttp->createHttpQuery("/engine/waiter/reservation-open.php",
    // QJsonObject{{"id", wo->fOrderDriver->headerValue("f_id").toString()}}, SLOT(openReservationResponse(QJsonObject)));
}

void DlgOrder::on_btnPaymentIdram_clicked()
{
    //TODO: CHECK
    // auto *wo = worder();
    // if(!wo) {
    //     return;
    // }
    // QByteArray out;
    // double v;
    // if(Idram::check(__c5config.getValue(param_idram_server), __c5config.getValue(param_idram_session_id),
    //                 wo->fOrderDriver->headerValue("f_id").toString(), v, out)) {
    //     ui->leIDRAM->setDouble(v);
    //     //        ui->leCash->setText("0");
    //     //        ui->leCard->setText("0");
    //     //        ui->leBank->setText("0");
    //     //        ui->leOther->setText("0");
    //     worder()->fOrderDriver->setHeader("f_amountcash", 0);
    //     worder()->fOrderDriver->setHeader("f_amountcard", 0);
    //     worder()->fOrderDriver->setHeader("f_amountbank", 0);
    //     worder()->fOrderDriver->setHeader("f_amountprepaid", 0);
    //     worder()->fOrderDriver->setHeader("f_amountother", 0);
    //     worder()->fOrderDriver->setHeader("f_amountpayx", 0);
    //     worder()->fOrderDriver->setHeader("f_amountidram", v);
    //     worder()->fOrderDriver->setHeader("f_hotel", 0);
    //     worder()->fOrderDriver->setHeader("f_otherid", 0);
    //     ui->leRemain->setDouble(worder()->fOrderDriver->headerValue("f_amounttotal").toDouble() - v);
    //     clearOther();
    //     headerToLineEdit();
    //     setButtonsState();
    //     return ;
    //     worder()->fOrderDriver->setHeader("f_otherid", 0);
    //     lineEditToHeader();
    // } else {
    //     QString err = out;
    //     if(v < 0) {
    //         err = tr("Idram payment was not received");
    //     }
    //     C5Message::error(err);
    // }
    // C5LogToServerThread::remember(LOG_WAITER, fUser->fullName(), "",
    //                               wo->fOrderDriver->currentOrderId(), "",
    //                               "Idram request: " + __c5config.getValue(param_idram_server),
    //                               QString(out), "");
}

void DlgOrder::on_btnFillPayX_clicked()
{
    //TODO: CHECK
    // WOrder *wo = worder();
    // if(!wo) {
    //     return;
    // }
    // ui->lePayX->setDouble(
    //     wo->fOrderDriver->headerValue("f_amounttotal").toDouble()
    //     - wo->fOrderDriver->headerValue("f_amountcash").toDouble()
    //     - wo->fOrderDriver->headerValue("f_amountcard").toDouble()
    //     - wo->fOrderDriver->headerValue("f_amountbank").toDouble()
    //     - wo->fOrderDriver->headerValue("f_amountprepaid").toDouble()
    //     - wo->fOrderDriver->headerValue("f_amountidram").toDouble());
    // wo->fOrderDriver->setHeader("f_amountpayx", ui->lePayX->getDouble());
    // ui->leRemain->setDouble(0);
    // headerToLineEdit();
    // setButtonsState();
}

void DlgOrder::on_btnCalcPayX_clicked()
{
    //TODO: CHECK
    // auto *wo = worder();
    // if(!wo) {
    //     return;
    // }
    // double max = wo->fOrderDriver->headerValue("f_amounttotal").toDouble();
    // if(!DlgPassword::getAmount(tr("PayX"), max)) {
    //     return;
    // }
    // ui->lePayX->setDouble(max);
    // lineEditToHeader();
    // headerToLineEdit();
}

void DlgOrder::on_btnPayX_clicked()
{
    //TODO: CHECK
    // auto *wo = worder();
    // if(!wo) {
    //     return;
    // }
    // worder()->fOrderDriver->setHeader("f_otherid", 0);
    // ui->lePayX->setDouble(wo->fOrderDriver->headerValue("f_amounttotal").toDouble() - ui->leIDRAM->getDouble());
    // ui->leCash->setDouble(0);
    // ui->leCard->setDouble(0);
    // ui->leBank->setDouble(0);
    // ui->leOther->setDouble(0);
    // lineEditToHeader();
}

void DlgOrder::on_btnFillIdram_clicked()
{
    //TODO: CHECK
    // auto *wo = worder();
    // if(!wo) {
    //     return;
    // }
    // ui->leIDRAM->setDouble(wo->fOrderDriver->headerValue("f_amounttotal").toDouble()
    //                        - worder()->fOrderDriver->headerValue("f_amontprepaid").toDouble()
    //                        - worder()->fOrderDriver->headerValue("f_amountcash").toDouble()
    //                        - worder()->fOrderDriver->headerValue("f_amountcard").toDouble()
    //                        - worder()->fOrderDriver->headerValue("f_amountbank").toDouble()
    //                        - worder()->fOrderDriver->headerValue("f_amountpayx").toDouble());
    // worder()->fOrderDriver->setHeader("f_amountother", 0);
    // ui->lePayX->setDouble(0);
    // ui->leCash->setDouble(0);
    // ui->leCard->setDouble(0);
    // ui->leBank->setDouble(0);
    // ui->leOther->setDouble(0);
    // lineEditToHeader();
}

void DlgOrder::on_btnCloseCheckAll_clicked()
{
    disableForCheckall(false);
}

void DlgOrder::on_btnCheckAll_clicked()
{
    //  if(wo) {
    //      wo->checkAllItems(true);
    //  }
}

void DlgOrder::on_btnUncheckAll_clicked()
{
    //   if(wo) {
    //       wo->checkAllItems(false);
    //   }
}

void DlgOrder::on_btnReprintSelected_clicked()
{
    //   disableForCheckall(false);
    //   C5User *tmp = fUser;
    //   if(!tmp->check(cp_t5_repeat_service)) {
    //       if(!DlgPassword::getUserAndCheck(tr("Reprint service check"), tmp, cp_t5_repeat_service)) {
    //           return;
    //       }
    //   }
    //   ui->btnPrintService->setEnabled(true);
    //   if(tmp != fUser) {
    //       delete tmp;
    //   }
    //   QStringList reprintList;
    //   for(int i = 0; i < wo->fOrderDriver->dishesCount(); i++) {
    //       if(wo->dishWidget(i)->isChecked()) {
    //           wo->fOrderDriver->setDishesValue("f_reprint", i, wo->fOrderDriver->dishesValue("f_reprint", i).toInt() * -1);
    //           reprintList.append(QString("'%1'").arg(wo->fOrderDriver->dishesValue("f_id", i).toString()));
    //       }
    //   }
    //   for(int i = 0; i < wo->fOrderDriver->dishesCount(); i++) {
    //       if(wo->fOrderDriver->dishesValue("f_qty2", i).toDouble() < 0.001) {
    //           wo->fOrderDriver->setDishesValue("f_printtime", QDateTime::currentDateTime(), i);
    //       }
    //   }
    //   if(!wo->fOrderDriver->save()) {
    //       C5Message::error(worder()->fOrderDriver->error());
    //       return;
    //   }
    //   //TODO: CHECK L9
    //   //LOG TO AIRLOG
    //   fHttp->createHttpQueryLambda("/engine/waiter/printservice.php", {
    //       { "order", wo->fOrderDriver->currentOrderId()},
    //       {"reprint", reprintList.join(",")}
    //   }, [this](const QJsonObject & jo) {
    //       auto np = new NDataProvider(this);
    //       np->overwriteHost("http", "127.0.0.1", 8080);
    //       connect(np, &NDataProvider::done, this, [this](const QJsonObject & jjo) {
    //           fHttp->httpQueryFinished(this);
    //       });
    //       connect(np, &NDataProvider::error, this, [](const QString & err) {
    //       });
    //       np->getData("/printjson", jo);
    //   }, [](const QJsonObject & je) {
    //       Q_UNUSED(je);
    //   });
}

void DlgOrder::on_btnGroupSelect_clicked()
{
    disableForCheckall(true);
}

void DlgOrder::on_btnRemoveSelected_clicked()
{
    disableForCheckall(false);
    //  QList<int> indexes = wo->checkedItems();
    //  if(indexes.isEmpty()) {
    //      return;
    //  }
    //  QString reasonname;
    //  int reasonid = 0;
    //  bool needreason = false;
    //  for(int i : indexes) {
    //      if(wo->fOrderDriver->dishesValue("f_qty2", i).toDouble() > 0.001) {
    //          needreason = true;
    //          break;
    //      }
    //  }
    //  C5User *tmp = fUser;
    //  if(needreason) {
    //      if(!tmp->check(cp_t5_remove_printed_service)) {
    //          if(!DlgPassword::getUserAndCheck(tr("Void dish"), tmp, cp_t5_remove_printed_service)) {
    //              return;
    //          }
    //      }
    //      if(!DlgDishRemoveReason::getReason(reasonname, reasonid)) {
    //          if(tmp != fUser) {
    //              tmp->deleteLater();
    //          }
    //          return;
    //      }
    //  }
    //  if(C5Message::question(tr("Confirm to remove selected dishes")) != QDialog::Accepted) {
    //      if(tmp != fUser) {
    //          tmp->deleteLater();
    //      }
    //      return;
    //  }
    //  for(int in = 0; in < indexes.count(); in++) {
    //      int i = indexes.at(in);
    //      if(wo->fOrderDriver->dishesValue("f_qty2", i).toDouble() < 0.001) {
    //          wo->fOrderDriver->setDishesValue("f_state", DISH_STATE_NONE, i);
    //          wo->fOrderDriver->setDishesValue("f_emarks", QVariant(), i);
    //          wo->fOrderDriver->save();
    //      } else {
    //          wo->fOrderDriver->setDishesValue("f_state", reasonid, i);
    //          wo->fOrderDriver->setDishesValue("f_removetime", QDateTime::currentDateTime(), i);
    //          wo->fOrderDriver->setDishesValue("f_removeuser", tmp->id(), i);
    //          wo->fOrderDriver->setDishesValue("f_removereason", reasonname, i);
    //          wo->fOrderDriver->setDishesValue("f_removeprecheck", wo->fOrderDriver->headerValue("f_precheck"), i);
    //          wo->fOrderDriver->setDishesValue("f_emarks", QVariant(), i);
    //          wo->fOrderDriver->save();
    //          //TODO: CHECK L17
    //          auto np = new NDataProvider(this);
    //          np->overwriteHost("http", "127.0.0.1", 8080);
    //          connect(np, &NDataProvider::done, this, [](const QJsonObject & jo) {
    //          });
    //          connect(np, &NDataProvider::error, this, [](const QString & err) {
    //          });
    //          np->getData("/printremoved", {{"id", wo->fOrderDriver->dishesValue("f_id", i).toString()}});
    //      }
    //  }
    //  if(tmp != fUser) {
    //      tmp->deleteLater();
    //  }
    //  wo->fOrderDriver->save();
    //  wo->updateDishes();
}

void DlgOrder::on_btnSetPrecent_clicked()
{
    //  disableForCheckall(false);
    //  C5User *tmp = fUser;
    //  bool needproceed = false;
    //  for(int i = 0; i < wo->fOrderDriver->dishesCount(); i++) {
    //      if(wo->fOrderDriver->dishesValue("f_state", i).toInt() != DISH_STATE_OK) {
    //          continue;
    //      }
    //      if(wo->fOrderDriver->dishesValue("f_price", i).toDouble() < 0.01) {
    //          continue;
    //      }
    //      if(wo->dishWidget(i)->isChecked()) {
    //          needproceed = true;
    //          break;
    //      }
    //  }
    //  if(!needproceed) {
    //      return;
    //  }
    //  if(!tmp->check(cp_t5_present)) {
    //      if(!DlgPassword::getUserAndCheck(tr("Present dish"), tmp, cp_t5_present)) {
    //          return;
    //      }
    //  }
    //  if(C5Message::question(tr("Are you sure to present selected dish?")) != QDialog::Accepted) {
    //      return;
    //  }
    //  for(int i = 0; i < wo->fOrderDriver->dishesCount(); i++) {
    //      if(wo->fOrderDriver->dishesValue("f_state", i).toInt() != DISH_STATE_OK) {
    //          continue;
    //      }
    //      if(wo->fOrderDriver->dishesValue("f_price", i).toDouble() < 0.01) {
    //          continue;
    //      }
    //      if(wo->dishWidget(i)->isChecked()) {
    //          wo->fOrderDriver->setDishesValue("f_price", 0, i);
    //          logRecord(tmp->fullName(),
    //                    wo->fOrderDriver->headerValue("f_id").toString(),
    //                    wo->fOrderDriver->dishesValue("f_id", i).toString(),
    //                    "Present dish",
    //                    dbdish->name(wo->fOrderDriver->dishesValue("f_dish", i).toInt()),
    //                    "");
    //      }
    //  }
    //  updateData();
    //  if(tmp != fUser) {
    //      delete tmp;
    //  }
}

void DlgOrder::on_btnReceived_clicked()
{
    //  double v = wo->fOrderDriver->headerValue("f_amounttotal").toDouble();
    //  if(Change::getReceived(v)) {
    //      wo->fOrderDriver->setHeader("f_cash", v);
    //      headerToLineEdit();
    //}
}

void DlgOrder::on_btnSelfCost_clicked()
{
    // w->fOrderDriver->setHeader("f_otherid", PAYOTHER_PRIMECOST);
    // setSelfcost();
    // if(w->fOrderDriver->save() == false) {
    //     C5Message::error(w->fOrderDriver->error());
    // }
}

void DlgOrder::on_btnDelivery_clicked()
{
    // QString phone, address, name;
    // int id;
    // if(CustomerInfo::getCustomer(id, name, phone, address)) {
    //     w->fOrderDriver->setHeader("f_partner", id);
    //     ui->btnDelivery->setText(QString("%1, %2, %3").arg(phone, name, address));
    // }
}

void DlgOrder::on_btnCalcIdram_clicked()
{
    calcAmount(ui->leIDRAM);
}

void DlgOrder::on_btnFillBank_clicked()
{
    // worder()->fOrderDriver->setHeader("f_amountbank",
    //                                   worder()->fOrderDriver->headerValue("f_amounttotal").toDouble()
    //                                   - worder()->fOrderDriver->headerValue("f_amountbank").toDouble()
    //                                   - worder()->fOrderDriver->headerValue("f_amountcash").toDouble()
    //                                   - worder()->fOrderDriver->headerValue("f_amountprepaid").toDouble()
    //                                   - worder()->fOrderDriver->headerValue("f_amountidram").toDouble()
    //                                   - worder()->fOrderDriver->headerValue("f_amountpayx").toDouble());
    // worder()->fOrderDriver->setHeader("f_amountother", 0);
    // ui->leRemain->setDouble(0);
    // clearOther();
    // headerToLineEdit();
    // setButtonsState();
}

void DlgOrder::on_btnCashout_clicked()
{
    // double max = wo->fOrderDriver->headerValue("f_amounttotal").toDouble();
    // if(!DlgPassword::getAmount(tr("Cashout"), max)) {
    //     return;
    // }
    // QString comment;
    // if(!DlgListOfDishComments::getComment(0, comment)) {
    //     return;
    // }
    // QString purpose = tr("") + " #"
    //                   + wo->fOrderDriver->headerValue("f_prefix").toString()
    //                   + QString::number(wo->fOrderDriver->headerValue("f_hallid").toInt()) + " " + comment;
    // C5Database db;
    // db[":f_oheader"] = wo->fOrderDriver->headerValue("f_id");
    // db.exec("select e.f_id as f_idout, a.f_id "
    //         "from a_header_cash a "
    //         "left join e_cash e on e.f_header=a.f_id "
    //         "where f_oheader=:f_oheader");
    // QString cashUUID;
    // QString idout;
    // if(db.nextRow()) {
    //     cashUUID = db.getString("f_id");
    //     idout = db.getString("f_idout");
    // }
    // C5StoreDraftWriter dw(db);
    // dw.writeAHeader(cashUUID, tr("Delivery"), DOC_STATE_SAVED, DOC_TYPE_CASH,
    //                 wo->fOrderDriver->headerValue("f_currentstaff").toInt(),
    //                 QDate::currentDate(), QDate::currentDate(), QTime::currentTime(), 0, max, purpose, 1, 1);
    // dw.writeAHeaderCash(cashUUID, 0, 1, 0, "", wo->fOrderDriver->headerValue("f_id").toString());
    // dw.writeECash(idout, cashUUID, 1, -1, purpose, max, idout, 1);
    // C5Message::info(purpose);
}

void DlgOrder::on_btnMenuSet_clicked()
{
    int index = 0;
    // //TODO: FIX MIGRATION
    // if(!wo->currentRow(index)) {
    //     return;
    // }
    // if(wo->fOrderDriver->dishesValue("f_state", index).toInt() != DISH_STATE_OK) {
    //     return;
    // }
    // DlgMenuSet d(wo->fOrderDriver->dishesValue("f_id", index).toString());
    // d.exec();
    ////TODO FUCK
    //load(fTable);
}

void DlgOrder::on_btnQR_clicked()
{
    // int index = -1;
    // WOrder *wo = worder();
    // if(!wo) {
    //     return;
    // }
    // //TODO: FIX MIGRATION
    // // if(!wo->currentRow(index)) {
    // //     return;
    // // }
    // if(wo->fOrderDriver->dishesValue("f_state", index).toInt() != DISH_STATE_OK) {
    //     return;
    // }
    // bool ok;
    // QString qr = QInputDialog::getText(this, tr("Emarks"), tr("Emarks"), QLineEdit::Normal, "", &ok);
    // if(!ok) {
    //     return;
    // }
    // if(qr.length() < 29 && qr.length() > 0) {
    //     C5Message::error(tr("Invalid Emarks"));
    //     return;
    // }
    // fHttp->createHttpQuery("/engine/waiter/qr-list.php",
    // QJsonObject {
    //     {"action", "add"},
    //     {"bodyid", wo->fOrderDriver->dishesValue("f_id", index).toString()},
    //     {"emarks", qr}},
    // SLOT(qrListResponse(QJsonObject)));
}

void DlgOrder::on_btnTable_clicked()
{
//TODO: CHECK
    // int newTableId;
    // if(!DlgFace::getTable(newTableId, wo->fOrderDriver->headerValue("f_hall").toInt(), fUser)) {
    //     return;
    // }
    // if(newTableId == wo->fOrderDriver->headerValue("f_table").toInt()) {
    //     C5Message::error(tr("Same table"));
    //     return;
    // }
    // fHttp->createHttpQuery("/engine/waiter/move-table.php",
    // QJsonObject {{"action", "moveTable"}, {"order", wo->fOrderDriver->headerValue("f_id").toString()},
    //     {"oldtable", wo->fOrderDriver->headerValue("f_table").toInt()},
    //     {"newtable", newTableId}},
    // SLOT(moveTableResponse(QJsonObject)));
}

void DlgOrder::on_leCmd_returnPressed()
{
    if(ui->leCmd->text().trimmed().isEmpty()) {
        return;
    }

    QString code = ui->leCmd->text().trimmed();
    ui->leCmd->clear();
    //Check qr exists
    fHttp->createHttpQuery("/engine/waiter/check-qr.php", QJsonObject{{"action", "checkQr"},
        {"qr", code}}, SLOT(checkQrResponse(QJsonObject)));
}

void DlgOrder::on_btnSetQr_clicked()
{
    //  int index = 0;
    //  //TODO: FIX MIGRATION
    //  // if(!wo->currentRow(index)) {
    //  //     return;
    //  // }
    //  if(wo->fOrderDriver->dishesValue("f_state", index).toInt() != DISH_STATE_OK) {
    //      return;
    //  }
    //  if(dbdish->isHourlyPayment(wo->fOrderDriver->dishesValue("f_dish", index).toInt())) {
    //      C5Message::error(tr("Cannot add comment to hourly payment"));
    //      return;
    //  }
    //  if(wo->fOrderDriver->dishesValue("f_emarks", index).toString().isEmpty() == false) {
    //      C5Message::error(tr("Emarks already set"));
    //      return;
    //  }
    //  bool ok;
    //  QString emarks = QInputDialog::getText(this, tr("Emarks"), "", QLineEdit::Normal, "", &ok);
    //  if(!ok) {
    //      return;
    //  }
    //  if(emarks.length() < 29) {
    //      C5Message::error(tr("Invalid Emarks"));
    //      return;
    //  }
    //  C5Database db;
    //  db[":f_emarks"] = emarks;
    //  db.exec("select * from o_body where f_state=1 and f_emarks=:f_emarks");
    //  if(db.nextRow()) {
    //      C5Message::error(tr("Used Emarks"));
    //      return;
    //  }
    //  wo->fOrderDriver->setDishesValue("f_emarks", emarks, index);
    //  wo->updateItem(index);
    //  logRecord(fUser->fullName(), wo->fOrderDriver->headerValue("f_id").toString(),
    //            wo->fOrderDriver->dishesValue("f_id", index).toString(),
    //            "Emarks for dish",
    //            dbdish->name(wo->fOrderDriver->dishesValue("f_emarks", index).toInt()), emarks);
    //  wo->fOrderDriver->save();
}
