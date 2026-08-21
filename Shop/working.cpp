#include "working.h"
#include <QFile>
#include <QInputDialog>
#include <QJsonArray>
#include <QKeyEvent>
#include <QMessageBox>
#include <QMovie>
#include <QPageSize>
#include <QPrinter>
#include <QPrinterInfo>
#include <QProcess>
#include <QScreen>
#include <QSet>
#include <QSettings>
#include <QShortcut>
#include <QTimer>
#include <QtMath>
#include "appwebsocket.h"
#include "c5cleartablewidget.h"
#include "c5message.h"
#include "c5permissions.h"
#include "c5printing.h"
#include "c5user.h"
#include "c5utils.h"
#include "chatmessage.h"
#include "dlgcashout.h"
#include "dlggiftcardsale.h"
#include "dlggoodslist.h"
#include "dlgattendancelogin.h"
#include "dlgcookingprogress.h"
#include "dlgpin.h"
#include "dlgregistercard.h"
#include "dlgshowcolumns.h"
#include "dqty.h"
#include "ndataprovider.h"
#include "ninterface.h"
#include "printreceiptgroup.h"
#include "sales.h"
#include "searchitems.h"
#include "struct_workstationitem.h"
#include "httplite.h"
#include "ui_working.h"
#include "wcustomerdisplay.h"
#include "worder.h"
#include "wsession.h"

QHash<QString, int> Working::fGoodsRows;
QMap<QString, double> Working::fUnitDefaultQty;
QMap<int, Flag> Working::fFlags;
Working* Working::fInstance = nullptr;
static QSettings __s(QString("%1\\%2\\%3").arg(_ORGANIZATION_, _APPLICATION_, _MODULE_));

Working::Working(C5User *user, QWidget *parent) :
    C5ShopDialog(user),
    ui(new Ui::Working)
{
    ui->setupUi(this);
    fInstance = this;
    updateWsStatus();
    if (AppWebSocket::instance) {
        connect(AppWebSocket::instance, &AppWebSocket::socketConnecting, this, &Working::updateWsStatus);
        connect(AppWebSocket::instance, &AppWebSocket::socketConnected, this, &Working::updateWsStatus);
        connect(AppWebSocket::instance, &AppWebSocket::socketDisconnected, this, &Working::updateWsStatus);
    }
    QString ip;
    fCustomerDisplay = nullptr;
    QString username;
    QString password;
    mUser = user;
    QShortcut *sF1 = new QShortcut(QKeySequence(Qt::Key_F1), this);
    QShortcut *sF2 = new QShortcut(QKeySequence(Qt::Key_F2), this);
    //    QShortcut *sF4 = new QShortcut(QKeySequence(Qt::Key_F4), this);
    QShortcut *sF5 = new QShortcut(QKeySequence(Qt::Key_F5), this);
    QShortcut *sF6 = new QShortcut(QKeySequence(Qt::Key_F6), this);
    QShortcut *sF7 = new QShortcut(QKeySequence(Qt::Key_F7), this);
    QShortcut *sF8 = new QShortcut(QKeySequence(Qt::Key_F8), this);
    QShortcut *sF9 = new QShortcut(QKeySequence(Qt::Key_F9), this);

    QShortcut *sF11 = new QShortcut(QKeySequence(Qt::Key_F11), this);
    QShortcut *sF12 = new QShortcut(QKeySequence(Qt::Key_F12), this);
    QShortcut *sDown = new QShortcut(QKeySequence(Qt::Key_Down), this);
    QShortcut *sUp = new QShortcut(QKeySequence(Qt::Key_Up), this);
    QShortcut *sEsc = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    QShortcut *sMinus = new QShortcut(QKeySequence(Qt::Key_Delete), this);
    QShortcut *sPlus = new QShortcut(QKeySequence(Qt::Key_Plus), this);
    QShortcut *sAsterix = new QShortcut(QKeySequence(Qt::Key_Asterisk), this);
    QShortcut *keyNumpadDot = new QShortcut(QKeySequence(Qt::Key_Comma), this);
    QShortcut *sci = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_I), this);
    QShortcut *sco = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_O), this);
    QShortcut *scl = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_L), this);
    connect(sci, &QShortcut::activated, this, &Working::onCtrlI);
    connect(sco, &QShortcut::activated, this, &Working::onCtrlO);
    connect(scl, &QShortcut::activated, this, &Working::onCtrlL);
    connect(sF1, SIGNAL(activated()), this, SLOT(shortcutF1()));
    connect(sF2, SIGNAL(activated()), this, SLOT(shortcutF2()));
    connect(sF5, &QShortcut::activated, this, [this]() {
        auto user = new C5User(mUser);
        auto openGoodsList = [this, user]() {
            auto *dg = new DlgGoodsList(user);
            connect(dg, &DlgGoodsList::getGoods, this, &Working::getGoods);
            dg->showMaximized();
            connect(dg, &DlgGoodsList::destroyed, this, [user]() { user->deleteLater(); });
        };
        if (user->check(cp_t12_shop_enter_store)) {
            openGoodsList();
        } else {
            bool ok = false;
            const QString password = QInputDialog::getText(this,
                                                           tr("Password"),
                                                           tr("Password"),
                                                           QLineEdit::Password,
                                                           QString(),
                                                           &ok);

            if (!ok || password.isEmpty()) {
                user->deleteLater();
                return;
            }

            user->authorize(
                password,
                fHttp,
                [openGoodsList](const QJsonObject &) { openGoodsList(); },
                [user]() { user->deleteLater(); });
        }
    });
    //    connect(sF4, SIGNAL(activated()), this, SLOT(shortcutF4()));
    connect(sF6, SIGNAL(activated()), this, SLOT(shortcurF6()));
    connect(sF7, SIGNAL(activated()), this, SLOT(shortcutF7()));
    connect(sF8, SIGNAL(activated()), this, SLOT(shortcutF8()));
    connect(sF9, SIGNAL(activated()), this, SLOT(shortcutF9()));

    connect(sF12, SIGNAL(activated()), this, SLOT(shortcutF12()));
    connect(sDown, SIGNAL(activated()), this, SLOT(shortcutDown()));
    connect(sUp, SIGNAL(activated()), this, SLOT(shortcutUp()));
    connect(sEsc, SIGNAL(activated()), this, SLOT(shortcutEscape()));
    connect(sMinus, SIGNAL(activated()), this, SLOT(shortcutMinus()));
    connect(sPlus, SIGNAL(activated()), this, SLOT(shortcutPlus()));
    connect(sAsterix, SIGNAL(activated()), this, SLOT(shortcutAsterix()));
    connect(keyNumpadDot, SIGNAL(activated()), this, SLOT(shortcutComma()));
    ui->tab->installEventFilter(this);
    //todo
    // ui->btnNewRetail->setVisible(!__c5config.shopDenyF1());
    // ui->btnNewWhosale->setVisible(!__c5config.shopDenyF2());
    fTimerCounter = 0;
    QTimer *timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
    timer->start(1000);

    loadStaff();

    fHaveChanges = false;
    fUpFinished = true;
    fTab = ui->tab;

    QSettings s(_ORGANIZATION_, _APPLICATION_ + QString("\\") + _MODULE_);

    if(s.value("customerdisplay").toBool()) {
        ui->btnCostumerDisplay->click();
    }

    ui->lbConfig->setText(mWorkStation.name);
    ui->lbStore->setText(mWorkStation.defaultStoreName());
    ui->lbCashier->setText(mUser->fullName());
    ui->lbHost->setText(NDataProvider::mHost);
    // Status = configured names only (no substitutions).
    {
        QStringList parts;
        if (mWorkStation.usePrintServer()) {
            parts.append(tr("server: %1").arg(mWorkStation.printServer()));
        }
        if (mWorkStation.hasReceiptPrinter()) {
            const QString name = mWorkStation.receiptPrinter();
            if (QPrinterInfo::printerInfo(name).isNull()) {
                parts.append(tr("%1 — not found, will not print").arg(name));
            } else {
                parts.append(name);
            }
        }
        if (parts.isEmpty()) {
            ui->lbPrinter->setText(tr("Printer: not configured"));
        } else {
            ui->lbPrinter->setText(tr("Printer: %1").arg(parts.join(QStringLiteral(" | "))));
        }
    }
    fHttp = new NInterface(this);
    mMovie = new QMovie(":/progressbar.gif");
    setSaleControlsEnabled(false);
    checkCashboxSession();
}

Working::~Working()
{
    delete ui;
}

bool Working::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::KeyRelease) {
        auto *ke = static_cast<QKeyEvent*>(event);

        switch(ke->key()) {
        case Qt::Key_Delete:
            shortcutMinus();
            event->accept();
            return true;

        case Qt::Key_Plus:
            shortcutPlus();
            event->accept();
            return true;

        case Qt::Key_Asterisk:
            shortcutAsterix();
            event->accept();
            return true;

        case Qt::Key_S:
            if(ke->modifiers() &Qt::ControlModifier) {
                openSearch();
                event->accept();
                return true;
            }

            break;
        }
    }

    return QWidget::eventFilter(watched, event);
}

void Working::setActiveWidget(WOrder *w)
{
    for(int i = 0; i < ui->tab->count(); i++) {
        if(w == ui->tab->widget(i)) {
            ui->tab->setCurrentIndex(i);
            refreshCustomerDisplay();
            return;
        }
    }
}

void Working::refreshCustomerDisplay()
{
    if (!fCustomerDisplay) {
        return;
    }
    if (auto *wo = worder()) {
        wo->updateCustomerDisplay();
    } else {
        fCustomerDisplay->clear();
    }
}

Working* Working::working()
{
    return fInstance;
}

int Working::cashSessionId() const
{
    return mCashSessionId;
}

bool Working::hasActiveSession() const
{
    return mCashSessionId > 0;
}

Flag Working::flag(int id)
{
    if(fFlags.contains(id)) {
        return fFlags[id];
    } else {
        return Flag();
    }
}

void Working::getGoods(int id, const QString &scancode, double stockQty)
{
    if(QObject *s = sender()) {
        s->deleteLater();
    }
    WOrder *w = static_cast<WOrder*>(ui->tab->currentWidget());
    if(!w) {
        return;
    }
    if(id <= 0 && scancode.isEmpty()) {
        C5Message::error(tr("Empty barcode"));
        return;
    }
    w->checkGoodsId(id, scancode, nullptr, stockQty);
}

WOrder* Working::worder()
{
    return qobject_cast<WOrder *>(ui->tab->currentWidget());
}

void Working::loadStaff(std::function<void()> next)
{
    NInterface::query1(QStringLiteral("/engine/v2/common/attendance/checked-in"),
                       mUser->mSessionKey,
                       this,
                       {},
                       [this, next](const QJsonObject &jo) {
                           fCurrentUsers.clear();
                           const QJsonArray users = jo.value(QStringLiteral("users")).toArray();
                           for(const QJsonValue &v : users) {
                               const QJsonObject o = v.toObject();
                               IUser u;
                               u.id = o.value(QStringLiteral("f_id")).toInt();
                               u.group = o.value(QStringLiteral("f_group")).toInt();
                               u.name = o.value(QStringLiteral("f_name")).toString();
                               QPixmap p;
                               const QByteArray photoData =
                                   QByteArray::fromBase64(o.value(QStringLiteral("f_photo")).toString().toLatin1());
                               if(photoData.isEmpty() || !p.loadFromData(photoData)) {
                                   p = QPixmap(QStringLiteral(":/staff.png"));
                               }
                               u.photo = p;
                               fCurrentUsers.append(u);
                           }
                           if(next) {
                               next();
                           }
                       });
}

void Working::checkCashboxSession()
{
    const int cashboxId = mWorkStation.cashboxId();

    if (cashboxId <= 0) {
        showSessionWidget();
        return;
    }

    NInterface::query1(QStringLiteral("/engine/v2/waiter/cashbox/check-status"),
                       mUser->mSessionKey,
                       this,
                       {{QStringLiteral("cashbox_id"), cashboxId}},
                       [this](const QJsonObject &jdoc) {
                           const QJsonObject session = jdoc.value(QStringLiteral("cashbox_session")).toObject();

                           if (session.value(QStringLiteral("f_id")).toInt() > 0) {
                               applyCashboxSession(session);
                               loadShopTables([this]() { newSale(1); });
                           } else {
                               showSessionWidget();
                           }
                       });
}

void Working::applyCashboxSession(const QJsonObject &session)
{
    mCashboxSessionData = session;
    mCashSessionId = session.value(QStringLiteral("f_id")).toInt();
    updateSessionUi();
    setSaleControlsEnabled(true);
}

void Working::clearCashboxSession()
{
    mCashboxSessionData = QJsonObject();
    mCashSessionId = 0;
    updateSessionUi();
    setSaleControlsEnabled(false);
}

void Working::refreshCashboxSession()
{
    const int cashboxId = mWorkStation.cashboxId();

    if (cashboxId <= 0 || !hasActiveSession()) {
        return;
    }

    NInterface::query1(QStringLiteral("/engine/v2/waiter/cashbox/check-status"),
                       mUser->mSessionKey,
                       this,
                       {{QStringLiteral("cashbox_id"), cashboxId}},
                       [this](const QJsonObject &jdoc) {
                           const QJsonObject session = jdoc.value(QStringLiteral("cashbox_session")).toObject();

                           if (session.value(QStringLiteral("f_id")).toInt() > 0) {
                               applyCashboxSession(session);
                           } else {
                               clearCashboxSession();
                               showSessionWidget();
                           }
                       });
}

void Working::updateSessionUi()
{
    if (!hasActiveSession()) {
        ui->lbShift->setVisible(false);
        ui->lbShiftInfo->setText(tr("closed"));
        return;
    }

    ui->lbShift->setVisible(true);
    ui->lbShiftInfo->setText(QStringLiteral("#%1 | %2 | %3 %4")
                                 .arg(mCashSessionId)
                                 .arg(mCashboxSessionData.value(QStringLiteral("f_amount_expected")).toString())
                                 .arg(mCashboxSessionData.value(QStringLiteral("f_orders_count")).toInt())
                                 .arg(tr("ops")));
}

void Working::updateWsStatus()
{
    if (!ui->lbWS) {
        return;
    }
    AppWebSocket::ConnectionState state = AppWebSocket::disconnected;
    if (AppWebSocket::instance) {
        state = AppWebSocket::instance->mConnectionState;
    }
    switch (state) {
    case AppWebSocket::connected:
        ui->lbWS->setText(tr("WebSocket: connected"));
        ui->lbWS->setStyleSheet(QStringLiteral("color: #1b7a1b;"));
        break;
    case AppWebSocket::connecting:
        ui->lbWS->setText(tr("WebSocket: connecting"));
        ui->lbWS->setStyleSheet(QStringLiteral("color: #b36b00;"));
        break;
    case AppWebSocket::disconnected:
    default:
        ui->lbWS->setText(tr("WebSocket: not connected"));
        ui->lbWS->setStyleSheet(QStringLiteral("color: #b00020;"));
        break;
    }
}

void Working::showSessionWidget()
{
    clearCashboxSession();

    while (ui->tab->count() > 0) {
        QWidget *w = ui->tab->widget(0);
        ui->tab->removeTab(0);
        w->deleteLater();
    }

    auto *ws = new WSession(mUser, this);
    ui->tab->addTab(ws, tr("Session"));
    ui->tab->setTabsClosable(false);
    connect(ws, &WSession::sessionOpened, this, &Working::onSessionOpened);
}

void Working::onSessionOpened(const QJsonObject &session)
{
    if (hasActiveSession()) {
        return;
    }

    auto *ws = qobject_cast<WSession *>(ui->tab->currentWidget());

    if (ws) {
        ui->tab->removeTab(ui->tab->currentIndex());
        ws->deleteLater();
    }

    ui->tab->setTabsClosable(true);
    applyCashboxSession(session);
    loadShopTables([this]() { newSale(1); });
}

void Working::setSaleControlsEnabled(bool enabled)
{
    ui->btnNewRetail->setEnabled(enabled);
    ui->btnNewWhosale->setEnabled(enabled);
    ui->btnWriteOrder->setEnabled(enabled);
    ui->btnCloseSession->setEnabled(enabled && mUser->check(cp_t5_waiter_open_close_shift));
}

void Working::printCloseSessionReport(const QJsonObject &cashbox, bool cashCounted)
{
    if (!mWorkStation.isReceiptPrintingConfigured()) {
        return;
    }
    C5Printing p;
    QPrinterInfo pi;
    if (mWorkStation.hasReceiptPrinter()) {
        pi = QPrinterInfo::printerInfo(mWorkStation.receiptPrinter());
    }
    QPrinter printer(pi.isNull() ? QPrinterInfo() : pi);
    if (!pi.isNull()) {
        printer.setPageSize(QPageSize::Custom);
        printer.setFullPage(false);
        QRectF pr = printer.pageRect(QPrinter::DevicePixel);
        constexpr qreal SAFE_RIGHT_MM = 4.0;
        qreal safePx = SAFE_RIGHT_MM * printer.logicalDpiX() / 25.4;
        p.setSceneParams(pr.width() - safePx, pr.height(), printer.logicalDpiX());
    } else {
        p.setSceneParams(650, 2800, 96);
    }
    p.setFont(qApp->font());
    p.setFontSize(22);
    const QString logoFile = qApp->applicationDirPath() + "/logo_receipt.png";

    if (QFile::exists(logoFile)) {
        p.image(logoFile, Qt::AlignHCenter);
        p.br();
    }

    p.ctext(tr("Closing session") + " " + QString::number(cashbox.value(QStringLiteral("f_id")).toInt()));
    p.br();
    p.br();
    p.lrtext(tr("Open"), cashbox.value(QStringLiteral("f_date_open")).toString());
    p.br();
    p.rtext(cashbox.value(QStringLiteral("f_user_open_name")).toString());
    p.br();
    p.br();
    p.lrtext(tr("Close"), cashbox.value(QStringLiteral("f_date_close")).toString());
    p.br();
    p.rtext(cashbox.value(QStringLiteral("f_user_close_name")).toString());
    p.br();
    p.br();
    p.lrtext(tr("Operations"), QString::number(cashbox.value(QStringLiteral("f_orders_count")).toInt()));
    p.br();
    p.lrtext(tr("Shift total"), cashbox.value(QStringLiteral("f_amount_expected")).toString());
    p.br();

    if (cashCounted) {
        const QString expectedCash = cashbox.value(QStringLiteral("f_amount_expected_cash")).toString();

        if (!expectedCash.isEmpty()) {
            p.lrtext(tr("Expected cash"), expectedCash);
            p.br();
        }

        p.lrtext(tr("Counted cash"), cashbox.value(QStringLiteral("f_amount_fact")).toString());
        p.br();
    }

    const double diffRaw = cashbox.value(QStringLiteral("f_amount_difference_raw")).toVariant().toDouble();

    if (qAbs(diffRaw) > 0.009) {
        const QString diffLabel = diffRaw > 0 ? tr("Cash Overage") : tr("Cash Shortage");
        p.lrtext(diffLabel, cashbox.value(QStringLiteral("f_amount_difference")).toString());
        p.br();
    }

    p.line();
    p.br();
    if (mWorkStation.usePrintServer()) {
        auto *http = new HttpLite(this);
        QJsonObject json;
        json.insert(QStringLiteral("print_data"), p.jsonData());
        json.insert(QStringLiteral("printer_name"),
                    mWorkStation.hasReceiptPrinter() ? mWorkStation.receiptPrinter() : QString());
        http->post(mWorkStation.printServer(), json);
    }
    if (mWorkStation.hasReceiptPrinter() && !pi.isNull()) {
        p.print(printer);
    }
}

void Working::printDifferenceAct(const QJsonObject &cashbox)
{
    if (!mWorkStation.isReceiptPrintingConfigured()) {
        return;
    }
    C5Printing p;
    QPrinterInfo pi;
    if (mWorkStation.hasReceiptPrinter()) {
        pi = QPrinterInfo::printerInfo(mWorkStation.receiptPrinter());
    }
    QPrinter printer(pi.isNull() ? QPrinterInfo() : pi);
    if (!pi.isNull()) {
        printer.setPageSize(QPageSize::Custom);
        printer.setFullPage(false);
        QRectF pr = printer.pageRect(QPrinter::DevicePixel);
        constexpr qreal SAFE_RIGHT_MM = 4.0;
        qreal safePx = SAFE_RIGHT_MM * printer.logicalDpiX() / 25.4;
        p.setSceneParams(pr.width() - safePx, pr.height(), printer.logicalDpiX());
    } else {
        p.setSceneParams(650, 2800, 96);
    }
    p.setFont(qApp->font());
    p.setFontSize(22);

    const double diffRaw = cashbox.value(QStringLiteral("f_amount_difference_raw")).toVariant().toDouble();
    const bool overage = diffRaw > 0;

    p.ctext(overage ? tr("Cash Overage Act") : tr("Cash Shortage Act"));
    p.br();
    p.ctext(tr("Session") + " " + QString::number(cashbox.value(QStringLiteral("f_id")).toInt()));
    p.br();
    p.br();
    p.lrtext(tr("Close"), cashbox.value(QStringLiteral("f_date_close")).toString());
    p.br();
    p.rtext(cashbox.value(QStringLiteral("f_user_close_name")).toString());
    p.br();
    p.br();
    const QString expectedCash = cashbox.value(QStringLiteral("f_amount_expected_cash")).toString();
    p.lrtext(expectedCash.isEmpty() ? tr("Expected amount") : tr("Expected cash"),
             expectedCash.isEmpty() ? cashbox.value(QStringLiteral("f_amount_expected")).toString() : expectedCash);
    p.br();
    p.lrtext(tr("Counted cash"), cashbox.value(QStringLiteral("f_amount_fact")).toString());
    p.br();
    p.lrtext(overage ? tr("Cash Overage") : tr("Cash Shortage"),
             cashbox.value(QStringLiteral("f_amount_difference")).toString());
    p.br();
    p.line();
    p.br();
    p.br();
    p.lrtext(tr("Signature"), QStringLiteral("______________"));
    p.br();
    if (mWorkStation.usePrintServer()) {
        auto *http = new HttpLite(this);
        QJsonObject json;
        json.insert(QStringLiteral("print_data"), p.jsonData());
        json.insert(QStringLiteral("printer_name"),
                    mWorkStation.hasReceiptPrinter() ? mWorkStation.receiptPrinter() : QString());
        http->post(mWorkStation.printServer(), json);
    }
    if (mWorkStation.hasReceiptPrinter() && !pi.isNull()) {
        p.print(printer);
    }
}

WOrder* Working::newSale(int type)
{
    if (!hasActiveSession()) {
        return nullptr;
    }

    const int tableId = allocateFreeTableId();
    if (tableId <= 0) {
        C5Message::error(tr("No free tables in hall %1. Create more tables in h_tables or close unused sale tabs.")
                             .arg(mWorkStation.defaultHallId()));
        return nullptr;
    }

    WOrder *w = new WOrder(mUser, type, fCustomerDisplay, this);
    w->setTableId(tableId);
    QObjectList ol = w->children();

    for(QObject *o : ol) {
        auto wd = dynamic_cast<QWidget*>(o);

        if(wd) {
            wd->installEventFilter(this);
        }
    }

    QString title;

    switch(type) {
    case 1:
        title = tr("Retail");
        break;

    case 2:
        title = tr("Whosale");
        break;
    }

    ui->tab->addTab(w, QString("%1 #%2").arg(title).arg(ordersCount()));
    ui->tab->setCurrentIndex(ui->tab->count() - 1);
    connect(w, &WOrder::orderSaved, this, &Working::orderSaved);
    refreshCustomerDisplay();
    return w;
}

WOrder* Working::openExistingSale(const QJsonObject &orderResponse)
{
    if (!hasActiveSession()) {
        return nullptr;
    }

    const QJsonObject order = orderResponse.value(QStringLiteral("order")).toObject();
    int tableId = orderResponse.value(QStringLiteral("table_id")).toInt();
    if (tableId <= 0) {
        tableId = order.value(QStringLiteral("f_table")).toInt();
    }
    if (tableId <= 0) {
        tableId = allocateFreeTableId();
    }
    if (tableId <= 0) {
        C5Message::error(tr("No free tables in hall %1. Create more tables in h_tables or close unused sale tabs.")
                             .arg(mWorkStation.defaultHallId()));
        return nullptr;
    }

    const int saleType = order.value(QStringLiteral("f_saletype")).toInt(1);
    WOrder *w = new WOrder(mUser, saleType > 0 ? saleType : 1, fCustomerDisplay, this);
    w->setStaffId(order.value(QStringLiteral("f_staff")).toInt());
    w->loadExistingOrder(orderResponse, tableId);

    QObjectList ol = w->children();
    for (QObject *o : ol) {
        auto wd = dynamic_cast<QWidget *>(o);
        if (wd) {
            wd->installEventFilter(this);
        }
    }

    QString title = (saleType == 2) ? tr("Whosale") : tr("Retail");
    ui->tab->addTab(w, QString("%1 #%2").arg(title).arg(ordersCount()));
    ui->tab->setCurrentIndex(ui->tab->count() - 1);
    connect(w, &WOrder::orderSaved, this, &Working::orderSaved);
    refreshCustomerDisplay();
    return w;
}

int Working::allocateFreeTableId() const
{
    QSet<int> used;
    for (int i = 0; i < ui->tab->count(); ++i) {
        auto *wo = qobject_cast<WOrder *>(ui->tab->widget(i));
        if (wo && wo->tableId() > 0) {
            used.insert(wo->tableId());
        }
    }

    for (int id : mShopTableIds) {
        if (!used.contains(id)) {
            return id;
        }
    }
    return 0;
}

void Working::loadShopTables(std::function<void()> next)
{
    if (!mShopTableIds.isEmpty()) {
        if (next) {
            next();
        }
        return;
    }

    const int hallId = mWorkStation.defaultHallId();
    if (hallId <= 0) {
        C5Message::error(tr("Default hall is not set for this workstation (f_default_hall_id)"));
        return;
    }

    NInterface::query1(QStringLiteral("/engine/v2/shop/tables/get"),
                       mUser->mSessionKey,
                       this,
                       {{QStringLiteral("hall"), hallId}},
                       [this, next, hallId](const QJsonObject &jo) {
                           mShopTableIds.clear();
                           const QJsonArray arr = jo.value(QStringLiteral("tables")).toArray();
                           for (const QJsonValue &v : arr) {
                               const int id = v.toInt();
                               if (id > 0) {
                                   mShopTableIds.append(id);
                               }
                           }
                           if (mShopTableIds.isEmpty()) {
                               C5Message::error(tr("No tables found in h_tables for hall %1").arg(hallId));
                               return;
                           }
                           if (next) {
                               next();
                           }
                       });
}

int Working::ordersCount()
{
    //TODO
    // TODO db;
    // db[":f_state"] = ORDER_STATE_CLOSE;
    // db[":f_datecash"] = QDate::currentDate();
    // db[":f_hall"] = mWorkStation.defaultHallId();
    // db.exec("select count(f_id) + 1 from o_header where f_datecash=:f_datecash and f_state=:f_state and f_hall=:f_hall");

    // if(db.nextRow()) {
    //     return db.getInt(0) + ui->tab->count();
    // } else {
    //     return ui->tab->count();
    // }
    return 1;
}

void Working::openSearch()
{
    SearchItems *si = new SearchItems(mUser);
    si->exec();
    si->deleteLater();
}

void Working::orderSaved(QWidget *w)
{
    for (int i = 0; i < ui->tab->count(); i++) {
        auto *wt = ui->tab->widget(i);
        if (wt == w) {
            ui->tab->removeTab(i);
            w->deleteLater();
            refreshCashboxSession();
            if (ui->tab->count() == 0) {
                loadShopTables([this]() { newSale(1); });
            }
        }
    }
}

void Working::timeout()
{
#ifdef QT_DEBUG
    int div = 10;
#else
    int div = 30;
#endif
    fTimerCounter++;
    //TODO
    // if(fTimerCounter % div == 0) {
    //     QJsonObject jo;
    //     jo["action"] = MSG_GET_UNREAD;
    //     jo["userfrom"] = mWorkStation.defaultStoreId();
    //     fHttp->createHttpQuery("/engine/shop/create-reserve.php", jo, SLOT(checkMessageResponse(QJsonObject)), QVariant(),
    //                            false);
    // }
}

void Working::onCtrlI()
{
    //TODO
    // QString pin, pass;

    // if(DlgPin::getPin(pin, pass, true)) {
    //     C5User *ua = new C5User();
    //     //TODO: REMOVE UA IF ERROR, MEMORY LEAK OTHERWISSE
    //     ua->authByPinPass(pin, pass, fHttp, [this, ua](const QJsonObject & jo) {
    //         todo db;
    //         db[":f_user"] = ua->id();
    //         db.exec("select * from s_salary_inout where f_user=:f_user and f_dateout is null");

    //         if(db.nextRow()) {
    //             C5Message::error(tr("Cannot input without output"));
    //         } else {
    //             db[":f_id"] = db.uuid();
    //             db[":f_user"] = ua->id();
    //             db[":f_hall"] = __c5config.getValue(param_default_hall).toInt();
    //             db[":f_datein"] = QDate::currentDate();
    //             db[":f_timein"] = QTime::currentTime();
    //             db.insert("s_salary_inout", false);
    //             loadStaff();
    //             ua->deleteLater();
    //             C5Message::info(QString("%1,<br>%2").arg(tr("Welcome"), ua->fullName()));
    //         }
    //     });
    // }
}

void Working::onCtrlO()
{
    // QString pin, pass;

    // if(DlgPin::getPin(pin, pass, true)) {
    //     C5User *ua = new C5User();
    //     ua->authByPinPass(pin, pass, fHttp, [this, ua](const QJsonObject & jo) {
    //         todo db;
    //         db[":f_user"] = ua->id();
    //         db.exec("select * from s_salary_inout where f_user=:f_user and f_dateout is null");

    //         if(db.nextRow()) {
    //             db[":f_user"] = ua->id();
    //             db[":f_dateout"] = QDate::currentDate();
    //             db[":f_timeout"] = QTime::currentTime();
    //             db.update("s_salary_inout", where_id(db.getString("f_id")));
    //             loadStaff();
    //             C5Message::info(QString("%1,<br>%2").arg(tr("Good bye"), ua->fullName()));
    //         } else {
    //             C5Message::error(tr("Cannot output without input"));
    //         }
    //     });
    // }
}

void Working::onCtrlL()
{
    // todo db;
    // db.exec("select concat(u.f_last, ' ', u.f_first) as f_name, u.f_login from s_salary_inout io "
    //         "left join s_user u on u.f_id=io.f_user "
    //         "where io.f_dateout is null ");
    // QString users;

    // while(db.nextRow()) {
    //     users += QString("%1, %2<br>").arg(db.getString("f_login"), db.getString("f_name"));
    // }

    // C5Message::info(users);
}

void Working::checkMessageResponse(const QJsonObject & jdoc)
{
    if(jdoc["messages"].toArray().isEmpty()) {
        return;
    }

    QFont font(qApp->font());
    font.setPointSize(12);
    C5Printing p;
    const bool canPrint = mWorkStation.hasReceiptPrinter();
    QPrinterInfo pi;
    if (canPrint) {
        pi = QPrinterInfo::printerInfo(mWorkStation.receiptPrinter());
    }
    QPrinter printer(pi);
    printer.setPageSize(QPageSize::Custom);
    printer.setFullPage(false);
    QRectF pr = printer.pageRect(QPrinter::DevicePixel);
    constexpr qreal SAFE_RIGHT_MM = 2.0;
    qreal safePx = SAFE_RIGHT_MM * printer.logicalDpiX() / 25.4;
    p.setSceneParams(pr.width() - safePx, pr.height(), printer.logicalDpiX());
    p.setFont(font);
    p.br(2);
    QPixmap img(":/atention.png");
    img = img.scaled(100, 100);
    p.image(img, Qt::AlignCenter);
    p.br(img.height() / 2);
    p.br(img.height() / 2);

    for(int i = 0; i < jdoc["messages"].toArray().count(); i++) {
        QJsonObject jom = jdoc["messages"].toArray().at(i).toObject();
        qDebug() << jom;
        QJsonParseError jerr;
        QJsonDocument jdocmsg = QJsonDocument::fromJson(jom["f_body"].toString().toUtf8(), &jerr);

        if(jerr.error == QJsonParseError::NoError) {
            QJsonObject jjm = jdocmsg.object();

            switch(jjm["action"].toInt()) {
            case MSG_GOODS_RESERVE:
                p.ltext(tr("Goods reserved"), 0);
                p.br();
                p.ltext(jjm["goodsname"].toString(), 0);
                p.br();
                p.ltext(jjm["scancode"].toString(), 0);
                p.br();
                p.ltext(QString("%1 %2").arg(jjm["qty"].toDouble()).arg(jjm["unit"].toString()), 0);
                p.br();
                p.ltext(jjm["usermessage"].toString(), 0);
                p.br();
                p.ltext(QString("%1 %2").arg(tr("End date"), jjm["enddate"].toString()), 0);
                p.br();
                p.br();
                p.line();
                p.br();
                C5Message::info(QString("%1<br>%2<br>%3<br>%4<br>%5<br>%6")
                                .arg(tr("Goods reserved"))
                                .arg(jjm["goodsname"].toString())
                                .arg(jjm["scancode"].toString())
                                .arg(QString("%1 %2").arg(jjm["qty"].toDouble()).arg(jjm["unit"].toString()))
                                .arg(jjm["usermessage"].toString())
                                .arg(QString("%1 %2").arg(tr("End date")).arg(jjm["enddate"].toString())));
                break;

            case MSG_PRINT_RECEIPT: {
                const QString orderid = jjm["usermessage"].toString();
                PrintReceiptGroup::print2(orderid, mUser, this);
                break;
            }

            default:
                break;
            }
        } else {
            p.ltext(jom["message"].toString(), 0);
            p.br();
            p.br();
            p.line();
            p.br();
            C5Message::info(jom["message"].toString());
        }
    }

    if (canPrint && !pi.isNull()) {
        p.br();
        p.br();
        p.ltext(tr("Printed"), 0);
        p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR2));
        p.print(printer);
    }
}

void Working::uploadDataFinished()
{
    fUpFinished = true;
}
void Working::shortcutEscape()
{
    WOrder *w = worder();

    if (!w) {
        return;
    }

    w->clearCode();
}
void Working::shortcutMinus()
{
    WOrder *w = worder();

    if (!w) {
        return;
    }

    w->keyMinus();
}
void Working::shortcutPlus()
{
    WOrder *w = worder();

    if (!w) {
        return;
    }

    w->keyPlus();
}
void Working::shortcutAsterix()
{
    WOrder *w = worder();

    if (!w) {
        return;
    }

    w->keyAsterix();
}
void Working::shortcutF1()
{
    loadShopTables([this]() { newSale(1); });
}
void Working::shortcutF2()
{
    loadShopTables([this]() { newSale(2); });
}

void Working::shortcutF7()
{
}
void Working::shortcutF8()
{
}
void Working::shortcutF9()
{
}

void Working::shortcutF12()
{
    on_btnWriteOrder_clicked();
}
void Working::shortcutDown()
{
    WOrder *w = static_cast<WOrder*>(ui->tab->currentWidget());

    if(!w) {
        return;
    }

    w->nextRow();
}
void Working::shortcutUp()
{
    WOrder *w = static_cast<WOrder*>(ui->tab->currentWidget());

    if(!w) {
        return;
    }

    w->prevRow();
}
void Working::shortcutComma()
{
    WOrder *w = static_cast<WOrder*>(ui->tab->currentWidget());

    if(w) {
        w->comma();
    }
}
void Working::qtyRemains(const QJsonObject & jdoc)
{
    //TODO:: BY OPTIONS
    QJsonObject jo = jdoc["data"].toObject();
    QJsonArray ja = jo["qty"].toArray();
    bool print = false;
    C5Printing p;
    const bool canPrint = mWorkStation.hasReceiptPrinter();
    QPrinterInfo pi;
    if (canPrint) {
        pi = QPrinterInfo::printerInfo(mWorkStation.receiptPrinter());
    }
    QPrinter printer(pi);
    printer.setPageSize(QPageSize::Custom);
    printer.setFullPage(false);
    QRectF pr = printer.pageRect(QPrinter::DevicePixel);
    constexpr qreal SAFE_RIGHT_MM = 2.0;
    qreal safePx = SAFE_RIGHT_MM * printer.logicalDpiX() / 25.4;
    p.setSceneParams(pr.width() - safePx, pr.height(), printer.logicalDpiX());
    QFont font = qApp->font();
    font.setPointSize(10);
    p.setFont(font);
    p.br(2);
    QPixmap img(":/atention.png");
    img = img.scaled(100, 100);
    p.image(img, Qt::AlignCenter);
    p.br(img.height() / 2);
    p.br(img.height() / 2);
    p.ctext(tr("The product is out of stock"));
    p.br();

    for(int i = 0; i < ja.size(); i++) {
        QJsonObject jn = ja.at(i).toObject();

        if(jn["f_qty"].toDouble() < 0.01) {
            print = true;
            p.ltext(jn["f_taxname"].toString(), 0);
            p.br();
            p.ltext(jn["f_name"].toString(), 0);
            p.br();
            p.ltext(jn["f_scancode"].toString(), 0);
            p.br();
        }
    }

    if(print && canPrint && !pi.isNull()) {
        p.ltext(tr("Printed"), 0);
        p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR2));
        p.print(printer);
    }

    sender()->deleteLater();
}
void Working::haveChanges(bool v)
{
    if(!fHaveChanges) {
        fHaveChanges = v;
    }
}
void Working::on_tab_tabCloseRequested(int index)
{
    QString err;
    WOrder *w = static_cast<WOrder*>(ui->tab->widget(index));

    ui->tab->removeTab(index);
    w->deleteLater();
    refreshCustomerDisplay();

    if(ui->tab->count() == 0) {
        loadShopTables([this]() { newSale(1); });
    }
}


void Working::on_tab_currentChanged(int index)
{
    Q_UNUSED(index);
    // Always show the active sale window on the buyer display.
    refreshCustomerDisplay();
}
void Working::on_btnCloseApplication_clicked()
{
    if(C5Message::question(tr("Confirm to close application")) == QDialog::Accepted) {
        qApp->quit();
    }
}
void Working::on_btnWriteOrder_clicked()
{
    WOrder *w = static_cast<WOrder*>(ui->tab->currentWidget());

    if(!w) {
        return;
    }

    w->writeOrder([this, w]() {
        w->table()->setRowCount(0);
        ui->tab->removeTab(ui->tab->currentIndex());

        if (ui->tab->count() == 0) {
            loadShopTables([this]() { newSale(1); });
        }

        w->deleteLater();

        //TODO
        // if (C5Config::fMainJson["remind_out_of_stock"].toBool()) {
        //     auto *dp = new NDataProvider(this);
        //     connect(dp, &NDataProvider::done, this, &Working::qtyRemains);
        //     dp->getData("/engine/shop/check-qty-remain.php", {});
        // }
    });
}

void Working::on_btnNewRetail_clicked()
{
    loadShopTables([this]() { newSale(1); });
}
void Working::on_btnNewWhosale_clicked()
{
    loadShopTables([this]() { newSale(2); });
}

void Working::on_btnSalesReport_clicked()
{
    C5User *tmp = new C5User(mUser);

    if(!tmp->check(cp_t12_shop_enter_sale)) {
        QString password = QInputDialog::getText(this, tr("Password"), tr("Password"), QLineEdit::Password);
        tmp->authorize(password, fHttp, [this, tmp](const QJsonObject & jo) {
            Q_UNUSED(jo)

            if(tmp->check(cp_t12_shop_enter_sale)) {
                if(tmp->fConfig["copyfrom"].toInt() != 0) {
                    tmp->copySettings(mUser);
                }

                Sales::showSales(this, tmp);
            } else {
                tmp->deleteLater();
            }
        }, [tmp]() {
            tmp->deleteLater();
        });
    } else {
        Sales::showSales(this, tmp);
    }
}
void Working::on_btnHelp_clicked()
{
    QString info = QString("Ctrl+S: %1<br>"
                           "Ctrl+I: %2<br>"
                           "Ctrl+O: %3<br>"
                           "Ctrl+T: %4<br>"
                           "Ctrl+A: %5<br>"
                           "Ctrl+H: %6<br>"
                           "Ctrl+Z: %7<br>"
                           "Ctrl+L: %8<br>")
                   .arg(tr("Search goods in the storages"),
                        tr("Input staff at the work"),
                        tr("Output staff from the work"),
                        tr("Total today"),
                        tr("Preorder"),
                        tr("Show log"),
                        tr("Open new store input document"),
                        tr("List of workers at work"));
    C5Message::info(info);
}
void Working::on_btnMinimize_clicked()
{
    showMinimized();
}

void Working::on_btnGiftCard_clicked()
{
    WOrder *wo = static_cast<WOrder*>(ui->tab->currentWidget());
    // if (wo->rowCount() > 0) {
    //     wo->removeRowForce();
    // }
    // if(wo->rowCount() > 0) {
    //     C5Message::error(tr("The gift card must saled separately"));
    //     return;
    // }

    DlgGiftCardSale d(mUser);
    //TODO
    // if(d.exec() == QDialog::Accepted) {
    //     QString err;

    //     if (!wo->checkQty(d.fGiftGoodsId, 1, err, 0)) {
    //         C5Message::error(err);
    //         return;
    //     }

    //     DbGoods dd(d.fGiftGoodsId);
    //     double price = d.fGiftPrice;
    //     wo->checkGoodsCode(d.fGiftScancode, [wo, price]() {
    //         wo->fOHeader.saleType = -1;
    //         wo->setPriceOfRow(0, price);
    //     });
    // } else {
    //     ui->tab->removeTab(ui->tab->currentIndex());
    //     wo->deleteLater();
    //     newSale(SALE_RETAIL);
    // }
}
void Working::on_btnCostumerDisplay_clicked(bool checked)
{
    Q_UNUSED(checked);
    QSettings s(_ORGANIZATION_, _APPLICATION_ + QString("\\") + _MODULE_);

    if (fCustomerDisplay) {
        s.setValue("customerdisplay", false);
        auto *d = fCustomerDisplay;
        fCustomerDisplay = nullptr;
        disconnect(d, nullptr, this, nullptr);
        d->close(); // WA_DeleteOnClose
        ui->btnCostumerDisplay->setChecked(false);
        return;
    }

    s.setValue("customerdisplay", true);
    fCustomerDisplay = new WCustomerDisplay();
    connect(fCustomerDisplay, &WCustomerDisplay::displayClosed, this, [this]() {
        fCustomerDisplay = nullptr;
        ui->btnCostumerDisplay->setChecked(false);
        QSettings st(_ORGANIZATION_, _APPLICATION_ + QString("\\") + _MODULE_);
        st.setValue("customerdisplay", false);
    });
    fCustomerDisplay->placeOnSecondaryScreen();
    ui->btnCostumerDisplay->setChecked(true);
    // Defer paint until the native window is mapped on the target screen.
    QTimer::singleShot(0, this, [this]() { refreshCustomerDisplay(); });
}

void Working::on_btnColumns_clicked()
{
    DlgShowColumns(mUser).exec();
}

void Working::on_chRegisterCard_clicked()
{
    DlgRegisterCard().exec();
}

void Working::on_btnCashout_clicked()
{
    DlgCashout(mUser).exec();
}

void Working::on_btnBooking_clicked()
{
    openSearch();
}

void Working::on_btnCloseSession_clicked()
{
    if (!hasActiveSession()) {
        return;
    }

    if (!mUser->check(cp_t5_waiter_open_close_shift)) {
        C5Message::error(mUser->error());
        return;
    }

    if (C5Message::question(tr("Do you want to close active session?")) != QDialog::Accepted) {
        return;
    }

    double amount_cash = 0;
    const bool cashCounted = mWorkStation.data.value(QStringLiteral("input_cashbox_amount_before_close")).toBool();

    if (cashCounted) {
        amount_cash = DQty::getQty(tr("Cash in drawer"), 0, this);

        if (amount_cash < 0) {
            return;
        }
    }

    QJsonObject closeParams;
    closeParams[QStringLiteral("cashbox_id")] = mWorkStation.cashboxId();
    closeParams[QStringLiteral("amount_cash")] = amount_cash;

    if (cashCounted) {
        closeParams[QStringLiteral("cash_counted")] = true;
    }

    NInterface::query1(QStringLiteral("/engine/v2/waiter/cashbox/close"),
                       mUser->mSessionKey,
                       this,
                       closeParams,
                       [this, cashCounted](const QJsonObject &jdoc) {
                           const QJsonObject cashbox = jdoc.value(QStringLiteral("cashbox")).toObject();
                           printCloseSessionReport(cashbox, cashCounted);

                           if (cashCounted) {
                               const double diffRaw = cashbox.value(QStringLiteral("f_amount_difference_raw")).toVariant().toDouble();

                               if (qAbs(diffRaw) > 0.009) {
                                   const QString diffLabel = diffRaw > 0
                                       ? tr("Cash Overage")
                                       : tr("Cash Shortage");
                                   C5Message::info(diffLabel + ": " + cashbox.value(QStringLiteral("f_amount_difference")).toString());
                                   printDifferenceAct(cashbox);
                               }
                           }

                           showSessionWidget();
                       });
}

void Working::on_btnAttendance_clicked()
{
    if(DlgAttendanceLogin::run(mUser, this)) {
        loadStaff();
    }
}

void Working::on_btnProgressWindow_clicked()
{
    DlgCookingProgress(mUser, this).exec();
}
