#include "working.h"
#include "ui_working.h"
#include "worder.h"
#include "c5database.h"
#include "datadriver.h"
#include "printtaxn.h"
#include "sales.h"
#include "c5config.h"
#include "ndataprovider.h"
#include "c5user.h"
#include "loghistory.h"
#include "dlggoodslist.h"
#include "dlgpin.h"
#include "dlgsplashscreen.h"
#include "c5cleartablewidget.h"
#include "searchitems.h"
#include "wcustomerdisplay.h"
#include "goodsreserve.h"
#include "c5logsystem.h"
#include "storeinput.h"
#include "taxprint.h"
#include "messagelist.h"
#include "threadcheckmessage.h"
#include "chatmessage.h"
#include "threadreadmessage.h"
#include "selectprinters.h"
#include "c5printing.h"
#include "dlggiftcardsale.h"
#include "dlgregistercard.h"
#include "printreceiptgroup.h"
#include "dlgshowcolumns.h"
#include "c5tempsale.h"
#include <QShortcut>
#include <QInputDialog>
#include <QKeyEvent>
#include <QTimer>
#include <QMessageBox>
#include <QProcess>
#include <QSettings>
#include <QDesktopWidget>
#include <QMovie>

QHash<QString, int> Working::fGoodsRows;
QHash<QString, QString> Working::fMultiscancode;
QMap<QString, double> Working::fUnitDefaultQty;
QMap<int, Flag> Working::fFlags;
Working *Working::fInstance = nullptr;
static QSettings __s(QString("%1\\%2\\%3").arg(_ORGANIZATION_, _APPLICATION_, _MODULE_));

Working::Working(C5User *user, QWidget *parent) :
    QWidget(parent, Qt::FramelessWindowHint),
    ui(new Ui::Working)
{
    ui->setupUi(this);
    fInstance = this;
    QString ip;
    fCustomerDisplay = nullptr;
    QString username;
    QString password;
    fUser = user;
    QShortcut *sF1 = new QShortcut(QKeySequence(Qt::Key_F1), this);
    QShortcut *sF2 = new QShortcut(QKeySequence(Qt::Key_F2), this);
    //    QShortcut *sF3 = new QShortcut(QKeySequence(Qt::Key_F3), this);
    //    QShortcut *sF4 = new QShortcut(QKeySequence(Qt::Key_F4), this);
    QShortcut *sF5 = new QShortcut(QKeySequence(Qt::Key_F5), this);
    QShortcut *sF6 = new QShortcut(QKeySequence(Qt::Key_F6), this);
    QShortcut *sF7 = new QShortcut(QKeySequence(Qt::Key_F7), this);
    QShortcut *sF8 = new QShortcut(QKeySequence(Qt::Key_F8), this);
    QShortcut *sF9 = new QShortcut(QKeySequence(Qt::Key_F9), this);
    QShortcut *sF10 = new QShortcut(QKeySequence(Qt::Key_F10), this);
    QShortcut *sF11 = new QShortcut(QKeySequence(Qt::Key_F11), this);
    QShortcut *sF12 = new QShortcut(QKeySequence(Qt::Key_F12), this);
    QShortcut *sDown = new QShortcut(QKeySequence(Qt::Key_Down), this);
    QShortcut *sUp = new QShortcut(QKeySequence(Qt::Key_Up), this);
    QShortcut *sEsc = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    QShortcut *sMinus = new QShortcut(QKeySequence(Qt::Key_Delete), this);
    QShortcut *sPlus = new QShortcut(QKeySequence(Qt::Key_Plus), this);
    QShortcut *sAsterix = new QShortcut(QKeySequence(Qt::Key_Asterisk), this);
    QShortcut *keyNumpadDot = new QShortcut(QKeySequence(Qt::Key_Comma), this);
    connect(sF1, SIGNAL(activated()), this, SLOT(shortcutF1()));
    connect(sF2, SIGNAL(activated()), this, SLOT(shortcutF2()));
    //    connect(sF3, SIGNAL(activated()), this, SLOT(shortcutF3()));
    //    connect(sF4, SIGNAL(activated()), this, SLOT(shortcutF4()));
    connect(sF5, SIGNAL(activated()), this, SLOT(shortcutF5()));
    connect(sF6, SIGNAL(activated()), this, SLOT(shortcurF6()));
    connect(sF7, SIGNAL(activated()), this, SLOT(shortcutF7()));
    connect(sF8, SIGNAL(activated()), this, SLOT(shortcutF8()));
    connect(sF9, SIGNAL(activated()), this, SLOT(shortcutF9()));
    connect(sF10, SIGNAL(activated()), this, SLOT(shortcutF10()));
    connect(sF11, SIGNAL(activated()), this, SLOT(shortcutF11()));
    connect(sF12, SIGNAL(activated()), this, SLOT(shortcutF12()));
    connect(sDown, SIGNAL(activated()), this, SLOT(shortcutDown()));
    connect(sUp, SIGNAL(activated()), this, SLOT(shortcutUp()));
    connect(sEsc, SIGNAL(activated()), this, SLOT(shortcutEscape()));
    connect(sMinus, SIGNAL(activated()), this, SLOT(shortcutMinus()));
    connect(sPlus, SIGNAL(activated()), this, SLOT(shortcutPlus()));
    connect(sAsterix, SIGNAL(activated()), this, SLOT(shortcutAsterix()));
    connect(keyNumpadDot, SIGNAL(activated()), this, SLOT(shortcutComma()));
    ui->tab->installEventFilter(this);
    ui->btnNewRetail->setVisible(!__c5config.shopDenyF1());
    ui->btnNewWhosale->setVisible(!__c5config.shopDenyF2());
    fTimerCounter = 0;
    QTimer *timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
    timer->start(1000);
    if (__c5config.shopDifferentStaff()) {
        loadStaff();
    }
    fHaveChanges = false;
    fUpFinished = true;
    fTab = ui->tab;
    C5Database db(__c5config.dbParams());
    db.exec("select m.f_id, g.f_scancode from c_goods_multiscancode m "
            "inner join c_goods g on g.f_id=m.f_goods "
            "where length(g.f_scancode)>0");
    fMultiscancode.clear();
    while (db.nextRow()) {
        fMultiscancode[db.getString(0)] = db.getString(1);
    }
    QSettings s(_ORGANIZATION_, _APPLICATION_ + QString("\\") + _MODULE_);
    if (s.value("customerdisplay").toBool()) {
        ui->btnCostumerDisplay->click();
    }
    ui->lbConfig->setText(__c5config.fSettingsName);
    ui->lbStore->setText(dbstore->name(__c5config.defaultStore()));
    ui->lbCashier->setText(fUser->fullName());
    http = new NInterface(this);
    http->createHttpQuery("/engine/shop/create-a-store-sale.php", QJsonObject{{"store", __c5config.defaultStore()}, {"forceupdate", true}},
    SLOT(astoresaleResponse(QJsonObject)));
    mMovie = new QMovie(":/progressbar.gif");
}

Working::~Working()
{
    delete ui;
}

bool Working::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::KeyRelease) {
        auto *ke = static_cast<QKeyEvent *>(event);
        switch (ke->key()) {
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
                if (ke->modifiers() &Qt::ControlModifier) {
                    SearchItems *si = new SearchItems();
                    si->exec();
                    si->deleteLater();
                    event->accept();
                    return true;
                }
                break;
            case Qt::Key_I:
                if (ke->modifiers() &Qt::ControlModifier) {
                    QString pin, pass;
                    if (DlgPin::getPin(pin, pass)) {
                        C5Database db(__c5config.dbParams());
                        C5User ua(0);
                        QString name;
                        if (ua.authByPinPass(pin, pass)) {
                            db[":f_user"] = ua.id();
                            db.exec("select * from s_salary_inout where f_user=:f_user and f_dateout is null");
                            if (db.nextRow()) {
                                C5Message::error(tr("Cannot input without output"));
                            } else {
                                db[":f_id"] = db.uuid();
                                db[":f_user"] = ua.id();
                                db[":f_hall"] = __c5config.getValue(param_default_hall).toInt();
                                db[":f_datein"] = QDate::currentDate();
                                db[":f_timein"] = QTime::currentTime();
                                db.insert("s_salary_inout", false);
                                loadStaff();
                                C5Message::info(QString("%1,<br>%2").arg(tr("Welcome"), name));
                            }
                        } else {
                            C5Message::error(ua.error());
                        }
                    }
                }
                break;
            case Qt::Key_L:
                if (ke->modifiers() &Qt::ControlModifier) {
                    C5Database db(__c5config.dbParams());
                    db.exec("select concat(u.f_last, ' ', u.f_first) as f_name, u.f_login from s_salary_inout io "
                            "left join s_user u on u.f_id=io.f_user "
                            "where io.f_dateout is null ");
                    QString users;
                    while (db.nextRow()) {
                        users += QString("%1, %2<br>").arg(db.getString("f_login"), db.getString("f_name"));
                    }
                    C5Message::info(users);
                }
                break;
            case Qt::Key_O:
                if (ke->modifiers() &Qt::ControlModifier) {
                    QString pin, pass;
                    if (DlgPin::getPin(pin, pass)) {
                        C5Database db(__c5config.dbParams());
                        C5User ua(0);
                        QString name;
                        if (ua.authByPinPass(pin, pass)) {
                            db[":f_user"] = ua.id();
                            db.exec("select * from s_salary_inout where f_user=:f_user and f_dateout is null");
                            if (db.nextRow()) {
                                db[":f_user"] = ua.id();
                                db[":f_dateout"] = QDate::currentDate();
                                db[":f_timeout"] = QTime::currentTime();
                                db.update("s_salary_inout", where_id(db.getString("f_id")));
                                loadStaff();
                                C5Message::info(QString("%1,<br>%2").arg(tr("Good bye"), name));
                            } else {
                                C5Message::error(tr("Cannot output without input"));
                            }
                        } else {
                            C5Message::error(ua.error());
                        }
                    }
                }
                break;
            case Qt::Key_T:
                if (ke->modifiers() &Qt::ControlModifier) {
                    C5Database db(__c5config.dbParams());
                    db[":f_hall"] = __c5config.defaultHall();
                    db[":f_datecash"] = QDate::currentDate();
                    db[":f_state"] = ORDER_STATE_CLOSE;
                    db.exec("select concat(w.f_last, ' ', w.f_first) as f_staff, if(f_amounttotal<0,-1,1) as f_sign,"
                            "sum(oh.f_amounttotal) as f_amounttotal "
                            "from o_header oh "
                            "left join s_user w on w.f_id=oh.f_staff  "
                            "where oh.f_hall=:f_hall and oh.f_datecash=:f_datecash  and oh.f_state=:f_state and oh.f_amounttotal>0 "
                            "group by 1, 2 "
                            "union "
                            "select '-' as f_staff, if(f_amounttotal<0,-1,1) as f_sign,"
                            "sum(oh.f_amounttotal) as f_amounttotal "
                            "from o_header oh "
                            "left join s_user w on w.f_id=oh.f_staff  "
                            "where oh.f_hall=:f_hall and oh.f_datecash=:f_datecash  and oh.f_state=:f_state and oh.f_amounttotal<0 "
                            "group by 1, 2 ");
                    QString info = tr("Total today") + "<br>";
                    while (db.nextRow()) {
                        info += QString("%1: %2<br>").arg(db.getString("f_staff"), float_str(db.getDouble("f_amounttotal"), 2));
                    }
                    C5Message::info(info);
                    event->accept();
                }
                break;
            case Qt::Key_H:
                if (ke->modifiers() &Qt::ControlModifier) {
                    auto *l = new LogHistory(this);
                    l->exec();
                    delete l;
                }
                break;
        }
    }
    return QWidget::eventFilter(watched, event);
}

void Working::decQty(int id, double qty)
{
    C5Database db(__c5config.dbParams());
    db[":f_goods"] = id;
    db[":f_store"] = __c5config.defaultStore();
    db[":f_qty"] = qty;
    db.exec("update a_store_sale set f_qty=f_qty+:f_qty where f_goods=:f_goods and f_store=:f_store");
}

void Working::setActiveWidget(WOrder *w)
{
    for (int i = 0; i < ui->tab->count(); i++) {
        if (w == ui->tab->widget(i)) {
            ui->tab->setCurrentIndex(i);
            return;
        }
    }
}

WOrder *Working::findDraft(const QString &draftid)
{
    for (int i = 0; i < ui->tab->count(); i++) {
        WOrder *wo = static_cast<WOrder *>(ui->tab->widget(i));
        if (wo->fDraftSale.id == draftid) {
            ui->tab->setCurrentIndex(i);
            return wo;
        }
    }
    return nullptr;
}

void Working::openDraft(const QString &draftid)
{
    auto *wo = newSale(SALE_RETAIL);
    wo->openDraft(draftid);
}

Working *Working::working()
{
    return fInstance;
}

Flag Working::flag(int id)
{
    if (fFlags.contains(id)) {
        return fFlags[id];
    } else {
        return Flag();
    }
}

void Working::startStoreUpdate()
{
    ui->lbStatus->setMovie(mMovie);
    mMovie->start();
}

void Working::getGoods(int id)
{
    sender()->deleteLater();
    WOrder *w = static_cast<WOrder *>(ui->tab->currentWidget());
    if (!w) {
        return;
    }
    DbGoods g(id);
    w->addGoods(g.scancode());
}

WOrder *Working::worder()
{
    return static_cast<WOrder *>(ui->tab->currentWidget());
}

void Working::loadStaff()
{
    fCurrentUsers.clear();
    C5Database db(__c5config.dbParams());
    db[":f_hall"] = __c5config.defaultHall();
    db.exec("select u.f_id, u.f_group, concat(u.f_last, ' ' , u.f_first) as f_name, to_base64(p.f_data) as f_data "
            "from s_salary_inout s "
            "inner join s_user u on u.f_id=s.f_user "
            "left join s_user_photo p on p.f_id=u.f_id "
            "where s.f_dateout is null and s.f_hall=:f_hall");
    while (db.nextRow()) {
        IUser u;
        u.id = db.getInt("f_id");
        u.group = db.getInt("f_group");
        u.name = db.getString("f_name");
        QPixmap p;
        if (!p.loadFromData(QByteArray::fromBase64(db.getValue("f_data").toString().toLatin1()))) {
            p = QPixmap(":/staff.png");
        }
        u.photo = p;
        fCurrentUsers.append(u);
    }
}

WOrder *Working::newSale(int type)
{
    WOrder *w = new WOrder(fUser, type, fCustomerDisplay, this);
    QObjectList ol = w->children();
    for (QObject *o : ol) {
        auto wd = dynamic_cast<QWidget *>(o);
        if (wd) {
            wd->installEventFilter(this);
        }
    }
    QString title;
    switch (type) {
        case SALE_RETAIL:
            title = tr("Retail");
            break;
        case SALE_WHOSALE:
            title = tr("Whosale");
            break;
    }
    ui->tab->addTab(w, QString("%1 #%2").arg(title).arg(ordersCount()));
    ui->tab->setCurrentIndex(ui->tab->count() - 1);
    C5LogSystem::writeEvent(QString("%1 %2, #%3").arg(tr("New"), title).arg(ui->tab->count()));
    return w;
}

int Working::ordersCount()
{
    C5Database db(C5Config::dbParams());
    db[":f_state"] = ORDER_STATE_CLOSE;
    db[":f_datecash"] = QDate::currentDate();
    db[":f_hall"] = C5Config::defaultHall();
    db.exec("select count(f_id) + 1 from o_header where f_datecash=:f_datecash and f_state=:f_state and f_hall=:f_hall");
    if (db.nextRow()) {
        return db.getInt(0) + ui->tab->count();
    } else {
        return ui->tab->count();
    }
}

void Working::timeout()
{
    fTimerCounter++;
    if (fTimerCounter % 10 == 0) {
        auto *tcm = new ThreadCheckMessage();
        connect(tcm, &ThreadCheckMessage::threadError, this, &Working::threadMessageError);
        connect(tcm, &ThreadCheckMessage::data, this, &Working::threadMessageData);
        tcm->start();
    }
    if (__c5config.rdbReplica()) {
        if (fTimerCounter % 20 == 0 && fUpFinished && __c5config.rdbReplica()) {
            //            fUpFinished = false;
            //            auto *r = new C5Replication();
            //            connect(r, SIGNAL(finished()), this, SLOT(uploadDataFinished()));
            //            r->start(SLOT(uploadToServer()));
        }
        //        if (fTimerCounter % 120 == 0) {
        //            auto *r = new C5Replication();
        //            connect(r, SIGNAL(haveChanges(bool)), this, SLOT(haveChanges(bool)));
        //            r->start(SLOT(downloadFromServer()));
        //        }
    }
}

void Working::astoresaleResponse(const QJsonObject &jdoc)
{
    Q_UNUSED(jdoc);
    newSale(SALE_RETAIL);
    http->httpQueryFinished(sender());
}

void Working::checkStoreResponse(const QJsonObject &jdoc)
{
    Q_UNUSED(jdoc);
    http->httpQueryFinished(sender());
    auto *dg = new DlgGoodsList(C5Config::getValue(param_default_currency).toInt());
    connect(dg, &DlgGoodsList::getGoods, this, &Working::getGoods);
    dg->showMaximized();
}

void Working::uploadDataFinished()
{
    fUpFinished = true;
}

void Working::threadMessageError(int code, const QString &message)
{
    qDebug() << code << message;
    C5LogSystem::writeEvent(message);
}

void Working::threadMessageData(int code, const QVariant &data)
{
    qDebug() << code << data;
    int start = data.toString().indexOf("\r\n\r\n") + 4;
    int len = data.toString().length() - start;
    QString msg = data.toString().mid(start, len);
    QJsonObject jo = QJsonDocument::fromJson(msg.toUtf8()).object();
    if (jo["messages"].toArray().isEmpty()) {
        return;
    }
    QFont font(qApp->font());
    font.setPointSize(30);
    C5Printing p;
    p.setSceneParams(650, 2800, QPrinter::Portrait);
    p.setFont(font);
    p.br(2);
    QPixmap img(":/atention.png");
    img = img.scaled(400, 400);
    p.image(img, Qt::AlignCenter);
    p.br(img.height() / 2);
    p.br(img.height() / 2);
    for (int i = 0; i < jo["messages"].toArray().count(); i++) {
        QJsonObject jom = jo["messages"].toArray().at(i).toObject();
        qDebug() << jom;
        QJsonParseError jerr;
        QJsonDocument jdocmsg = QJsonDocument::fromJson(jom["message"].toString().toUtf8(), &jerr);
        p.ctext(tr("Message date and time"));
        p.br();
        p.ctext(jom["msgdate"].toString());
        p.br();
        if (jerr.error == QJsonParseError::NoError) {
            QJsonObject jjm = jdocmsg.object();
            switch (jjm["action"].toInt()) {
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
                case MSG_PRINT_TAX: {
                    C5Database db(__c5config.dbParams());
                    QJsonObject jord = jjm["usermessage"].toObject();
                    QString id = jord["id"].toString();
                    QString rseq;
                    p.br();
                    p.br();
                    if (Sales::printCheckWithTax(db, id)) {
                        p.ctext(QString("%1: %2").arg(tr("Fiscal printed"), jord["ordernum"].toString()));
                        p.br();
                        p.ctext(jord["orderamount"].toString());
                    } else {
                        p.ctext(QString("%1: %2").arg(tr("Fiscal not printed"), jord["ordernum"].toString()));
                    }
                    break;
                }
                case MSG_PRINT_RECEIPT: {
                    QString orderid = jjm["usermessage"].toString();
                    PrintReceiptGroup p;
                    C5Database db(__c5config.dbParams());
                    switch (C5Config::shopPrintVersion()) {
                        case 1: {
                            bool p1, p2;
                            if (SelectPrinters::selectPrinters(p1, p2)) {
                                if (p1) {
                                    p.print(orderid, db, 1);
                                }
                                if (p2) {
                                    p.print(orderid, db, 2);
                                }
                            }
                            break;
                        }
                        case 2:
                            p.print2(orderid, db);
                            break;
                        default:
                            break;
                    }
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
    p.br();
    p.br();
    p.ltext(tr("Printed"), 0);
    p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR2));
    p.print(C5Config::localReceiptPrinter(), QPrinter::Custom);
    auto *trm = new ThreadReadMessage(jo["idlist"].toString());
    trm->start();
}

void Working::shortcutEscape()
{
    worder()->clearCode();
}

void Working::shortcutMinus()
{
    worder()->keyMinus();
}

void Working::shortcutPlus()
{
    worder()->keyPlus();
}

void Working::shortcutAsterix()
{
    worder()->keyAsterix();
}

void Working::shortcutF1()
{
    if (__c5config.shopDenyF1()) {
        return;
    }
    newSale(SALE_RETAIL);
}

void Working::shortcutF2()
{
    if (__c5config.shopDenyF2()) {
        return;
    }
    newSale(SALE_WHOSALE);
}

void Working::shortcutF5()
{
    on_btnGoodsList_clicked();
}

void Working::shortcurF6()
{
    WOrder *w = static_cast<WOrder *>(ui->tab->currentWidget());
    if (!w) {
        return;
    }
    w->focusTaxpayerId();
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

void Working::shortcutF10()
{
    on_btnItemBack_clicked();
}

void Working::shortcutF11()
{
    on_btnGoodsMovement_clicked();
}

void Working::shortcutF12()
{
    on_btnWriteOrder_clicked();
}

void Working::shortcutDown()
{
    WOrder *w = static_cast<WOrder *>(ui->tab->currentWidget());
    if (!w) {
        return;
    }
    w->nextRow();
}

void Working::shortcutUp()
{
    WOrder *w = static_cast<WOrder *>(ui->tab->currentWidget());
    if (!w) {
        return;
    }
    w->prevRow();
}

void Working::shortcutComma()
{
    WOrder *w = static_cast<WOrder *>(ui->tab->currentWidget());
    if (w) {
        w->comma();
    }
}

void Working::qtyRemains(const QJsonObject &jdoc)
{
    QJsonObject jo = jdoc["data"].toObject();
    QJsonArray ja = jo["qty"].toArray();
    bool print = false;
    C5Printing p;
    QFont font = qApp->font();
    font.setPointSize(28);
    p.setSceneParams(650, 2800, QPrinter::Portrait);
    p.setFont(font);
    p.br(2);
    QPixmap img(":/atention.png");
    img = img.scaled(400, 400);
    p.image(img, Qt::AlignCenter);
    p.br(img.height() / 2);
    p.br(img.height() / 2);
    p.ctext(tr("The product is out of stock"));
    p.br();
    for (int i = 0; i < ja.size(); i++) {
        QJsonObject jn = ja.at(i).toObject();
        if (jn["f_qty"].toDouble() < 0.01) {
            print = true;
            p.ltext(jn["f_taxname"].toString(), 0);
            p.br();
            p.ltext(jn["f_name"].toString(), 0);
            p.br();
            p.ltext(jn["f_scancode"].toString(), 0);
            p.br();
        }
    }
    if (print) {
        p.ltext(tr("Printed"), 0);
        p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR2));
        p.print(C5Config::localReceiptPrinter(), QPrinter::Custom);
    }
    sender()->deleteLater();
}

void Working::haveChanges(bool v)
{
    if (!fHaveChanges) {
        fHaveChanges = v;
    }
}

void Working::on_tab_tabCloseRequested(int index)
{
    QString err;
    C5Database db(__c5config.dbParams());
    WOrder *w = static_cast<WOrder *>(ui->tab->widget(index));
    if (!w->fDraftSale.id.toString().isEmpty()) {
        if (w->rowCount() == 0) {
            w->fDraftSale.state = 3;
            w->fDraftSale.write(db, err);
        }
    }
    ui->tab->removeTab(index);
    w->deleteLater();
    if (ui->tab->count() == 0) {
        newSale(SALE_RETAIL);
    }
}

void Working::on_btnItemBack_clicked()
{
    if (!fUser->check(cp_t5_refund_goods)) {
        return;
    }
    Sales::showSales(this, fUser);
}

void Working::on_tab_currentChanged(int index)
{
    auto *wo = worder();
    if (wo) {
        wo->updateCustomerDisplay(fCustomerDisplay);
    }
    C5LogSystem::writeEvent(QString("%1 #%2").arg(tr("Current window")).arg(index + 1));
}

void Working::on_btnCloseApplication_clicked()
{
    if (C5Message::question(tr("Confirm to close application")) == QDialog::Accepted) {
        qApp->quit();
    }
}

void Working::on_btnWriteOrder_clicked()
{
    WOrder *w = static_cast<WOrder *>(ui->tab->currentWidget());
    if (!w) {
        return;
    }
    C5LogSystem::writeEvent(QString("F12"));
    if (!w->writeOrder()) {
        return;
    }
    QString id = w->fOHeader._id();
    w->table()->setRowCount(0);
    ui->tab->removeTab(ui->tab->currentIndex());
    if (ui->tab->count() == 0) {
        newSale(SALE_RETAIL);
    }
    w->deleteLater();
    startStoreUpdate();
    auto *dp = new NDataProvider(this);
    connect(dp, &NDataProvider::done, this, &Working::qtyRemains);
    dp->getData("/engine/shop/check-qty-remain.php", QJsonObject{{"header", id}});
}

void Working::on_btnGoodsMovement_clicked()
{
    C5User *u = fUser;
    if (!u->check(cp_t5_refund_goods)) {
        QString password = QInputDialog::getText(this, tr("Password"), tr("Password"), QLineEdit::Password);
        C5User *tmp = new C5User(password);
        if (tmp->error().isEmpty()) {
            u = tmp;
        } else {
            C5Message::error(tmp->error());
            delete tmp;
            return;
        }
    }
    StoreInput *si = new StoreInput(u);
    si->showMaximized();
    if (u != fUser) {
        delete u;
    }
}

void Working::on_btnNewRetail_clicked()
{
    newSale(SALE_RETAIL);
}

void Working::on_btnNewWhosale_clicked()
{
    newSale(SALE_WHOSALE);
}

void Working::on_btnGoodsList_clicked()
{
    http->createHttpQuery("/engine/shop/create-a-store-sale.php", QJsonObject{{"store", __c5config.defaultStore()}}, SLOT(
        checkStoreResponse(QJsonObject)));
}

void Working::on_btnSalesReport_clicked()
{
    C5User *u = fUser;
    if (!u->check(cp_t5_refund_goods)) {
        QString password = QInputDialog::getText(this, tr("Password"), tr("Password"), QLineEdit::Password);
        C5User *tmp = new C5User(password);
        if (tmp->error().isEmpty()) {
            u = tmp;
        } else {
            C5Message::error(tmp->error());
            delete tmp;
            return;
        }
    }
    Sales::showSales(this, u);
    if (u != fUser) {
        delete u;
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

void Working::on_btnManualTax_clicked()
{
    TaxPrint *tp = new TaxPrint();
    tp->exec();
    tp->deleteLater();
}

void Working::on_btnMinimize_clicked()
{
    showMinimized();
}

void Working::on_btnClientConfigQR_clicked()
{
    WOrder *wo = static_cast<WOrder *>(ui->tab->currentWidget());
    if (wo) {
        wo->imageConfig();
    }
}

void Working::on_btnGiftCard_clicked()
{
    WOrder *wo = static_cast<WOrder *>(ui->tab->currentWidget());
    if (wo->rowCount() > 0) {
        C5Message::error(tr("The gift card must saled separately"));
        return;
    }
    DlgGiftCardSale d(__c5config.dbParams());
    if (d.exec() == QDialog::Accepted) {
        QString err;
        if (!wo->checkQty(d.fGiftGoodsId, 1, err, 0)) {
            C5Message::error(err);
            return;
        }
        DbGoods dd(d.fGiftGoodsId);
        wo->addGoods(dd.scancode());
        wo->changePrice(d.fGiftPrice);
        wo->fOHeader.saleType = -1;
    }
}

void Working::on_btnCostumerDisplay_clicked(bool checked)
{
    Q_UNUSED(checked);
    QSettings s(_ORGANIZATION_, _APPLICATION_ + QString("\\") + _MODULE_);
    if (fCustomerDisplay) {
        s.setValue("customerdisplay", false);
        fCustomerDisplay->deleteLater();
        fCustomerDisplay = nullptr;
    } else {
        s.setValue("customerdisplay", true);
        fCustomerDisplay = new WCustomerDisplay();
        if (qApp->desktop()->screenCount() > 1) {
            fCustomerDisplay->move(qApp->desktop()->screenGeometry(1).x(), qApp->desktop()->screenGeometry(1).y());
            fCustomerDisplay->showMaximized();
            fCustomerDisplay->showFullScreen();
        } else {
            fCustomerDisplay->showFullScreen();
        }
    }
    auto *wo = worder();
    if (wo) {
        wo->updateCustomerDisplay(fCustomerDisplay);
    }
}

void Working::on_btnOpenDraft_clicked()
{
    C5TempSale t;
    if (t.exec() == QDialog::Accepted) {
        QString id = t.openDraft();
        if (id.isEmpty()) {
            return;
        }
        auto *wo = findDraft(id);
        if (wo) {
            wo->openDraft(id);
        } else {
            wo = newSale(SALE_RETAIL);
            wo->openDraft(id);
        }
    }
}

void Working::on_btnColumns_clicked()
{
    DlgShowColumns().exec();
}

void Working::on_chRegisterCard_clicked()
{
    DlgRegisterCard().exec();
}
