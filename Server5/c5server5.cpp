#include "c5server5.h"
#include "ui_c5server5.h"
#include "c5printjson.h"
#include "server5settings.h"
#include "c5utils.h"
#include "notificationwidget.h"
#include "appwebsocket.h"
#include "c5printing.h"
#include "c5scheduler.h"
#include "ndataprovider.h"
#include "logwriter.h"
#include "jsons.h"
#include "c5serverprinter.h"
#include <QCloseEvent>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QSettings>
#include <QInputDialog>
#include <QDebug>

static QSettings ___s(_ORGANIZATION_, _APPLICATION_);

C5Server5::C5Server5(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::C5Server5)
{
    ui->setupUi(this);
    wc = ui->wc;
    wc->getWatchList(this);
    fCanClose = false;
    fTrayMenu.addAction(tr("Quit"), this, SLOT(appTerminate()));
    fTrayMenu.addAction(tr("Clear basket"), this, SLOT(clearBasket()));
    fTrayIcon.setIcon(QIcon(":/server.png"));
    fTrayIcon.show();
    fTrayIcon.setContextMenu( &fTrayMenu);
    connect( &fTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this,
             SLOT(iconClicked(QSystemTrayIcon::ActivationReason)));
    connect(AppWebSocket::instance, &AppWebSocket::socketConnecting, this, &C5Server5::socketConnecting);
    connect(AppWebSocket::instance, &AppWebSocket::socketConnected, this, &C5Server5::socketConnected);
    connect(AppWebSocket::instance, &AppWebSocket::socketDisconnected, this, &C5Server5::socketDisconnected);
    switch (AppWebSocket::instance->mConnectionState) {
        case AppWebSocket::connecting:
            socketConnecting();
            break;
        case AppWebSocket::connected:
            socketConnected();
            break;
        case AppWebSocket::disconnected:
            socketDisconnected();
            break;
    }
    scheduler = new c5scheduler(this);
    fLastRunTime = QDateTime::currentDateTime();
    NDataProvider::mHost = ___s.value("server").toString();
    NDataProvider::sessionKey = __s.value("sessionkey").toString();
    fHttp = new NInterface(this);
    connect(AppWebSocket::instance, &AppWebSocket::messageReceived, this, &C5Server5::messageReceived);
    started();
}

C5Server5::~C5Server5()
{
    delete scheduler;
    delete ui;
}

void C5Server5::closeEvent(QCloseEvent *event)
{
    if (!fCanClose) {
        event->ignore();
        hide();
        return;
    }
    QWidget::closeEvent(event);
}

void C5Server5::printOrder(const QJsonObject &jo, QJsonArray &jcomplete)
{
    auto *thread = new QThread();
    auto *worker = new C5ServerPrinter(jo);
    worker->moveToThread(thread);
    connect(thread, &QThread::started, worker, &C5ServerPrinter::run);
    connect(worker, &C5ServerPrinter::finished, thread, &QThread::quit);
    connect(worker, &C5ServerPrinter::showNotificationWidget, this, &C5Server5::showNotificationWidget);
    connect(thread, &QThread::finished, worker, &QObject::deleteLater);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    thread->start();
}

void C5Server5::started()
{
    int fontSize = 22;
    QFont font = qApp->font();
    font.setPointSize(20);
    C5Printing p;
    p.setSceneParams(600, 2800, QPrinter::Portrait);
    p.setFont(font);
    p.setFontBold(false);
    p.setFontSize(fontSize);
    p.ctext("Սպասում ենք KINOPARK-ի պատվերները");
    p.br();
    p.setFontSize(20);
    p.ctext(QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss"));
    p.br();
    p.br();
    p.br();
    p.br();
    p.br();
    p.ctext(".");
    p.print(ui->lePrinter->text(), QPrinter::Custom);
    auto *timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &C5Server5::timeout);
#ifdef QT_DEBUG
    timer->start(10000);
#else
    timer->start(60000);
#endif
    timeout();
}

void C5Server5::appTerminate()
{
    if (QMessageBox::warning(this, tr("Confirmation"), tr("Are you sure to close application?"),
                             QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        fCanClose = true;
        close();
    }
}

void C5Server5::clearBasket()
{
    AppWebSocket::instance->sendMessage(QJsonObject{{"command", "chat"},
        {"sessionkey", ___s.value("sessionkey").toString()},
        {"database", "kinopark"},
        {"handler", "clearbasket"}});
}

void C5Server5::iconClicked(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
        case QSystemTrayIcon::DoubleClick:
            setVisible(true);
            break;
        default:
            break;
    }
}

void C5Server5::on_btnApply_clicked()
{
    if (ui->wc->hasChanges()) {
        ui->wc->saveChanges();
    }
    NDataProvider::mProtocol = ui->cbProtocol->currentText() + "://";
    NDataProvider::mHost = ui->leServer->text();
    fHttp->createHttpQuery("/engine/login.php", QJsonObject{{"method", 2}, {"pin", ui->leSecret->text()}}, SLOT(
        applyResponse(QJsonObject)));
}

void C5Server5::socketConnecting()
{
    ui->lbStatus->setText(tr("Connecting"));
}

void C5Server5::socketConnected()
{
    ui->lbStatus->setText(tr("Connected"));
    QJsonObject jo;
    jo["command"] = "register_socket";
    jo["socket_type"] = 8;
    jo["userid"] = 1;
    jo["database"] = "kinopark";
    AppWebSocket::instance->sendMessage(__jsonstr(jo));
}

void C5Server5::socketDisconnected()
{
    ui->lbStatus->setText(tr("Disconnected"));
}

void C5Server5::timeout()
{
    qDebug() << fLastRunTime.secsTo(QDateTime::currentDateTime());
    if (fLastRunTime.secsTo(QDateTime::currentDateTime()) < 20) {
        return;
    }
    if (ui->leServer->text().isEmpty()) {
        return;
    }
    fLastRunTime = QDateTime::currentDateTime();
    fHttp->fErrorObject = this;
    fHttp->fErrorSlot = (char *) SLOT(ordersErrorResponse(QString));
    fHttp->createHttpQuery("/engine/kinopark/orders.php", QJsonObject{
        {"action", "get"},
        {"printer", ui->lePrinter->text()}
    }, SLOT(ordersResponse(QJsonObject)), "", false);
    AppWebSocket::instance->sendMessage(QJsonObject{{"command", "chat"},
        {"handler", "getunread"},
        {"database", "kinopark"},
        {"sessionkey", ___s.value("sessionkey").toString()},
        {"myid", ___s.value("userid").toInt()}});
}

void C5Server5::ordersResponse(const QJsonObject &jdoc)
{
    QJsonArray ja = jdoc["orders"].toArray();
    QJsonArray jcomplete;
    for (int i = 0; i < ja.size(); i++) {
        QJsonObject jo = ja.at(i).toObject();
        QString strTemp = jo["ord"].toString();
        jo = __strjson(strTemp);
        jo = jo["id"].toObject();
        printOrder(jo, jcomplete);
    }
    fHttp->httpQueryFinished(sender());
}

void C5Server5::ordersErrorResponse(const QString &err)
{
    qDebug() << err;
    LogWriter::write(LogWriterLevel::errors, "", err);
}

void C5Server5::applyResponse(const QJsonObject &jdoc)
{
    fHttp->httpQueryFinished(sender());
    qDebug() << jdoc;
    QJsonObject jo = jdoc["data"].toObject();
    NDataProvider::sessionKey = jo["sessionkey"].toString();
    ___s.setValue("sessionkey", jo["sessionkey"].toString());
    jo = jo["config"].toObject();
    jo = jo["f_config"].toObject();
    ui->lePrinter->setText(jo["receiptprinter"].toString());
    jo = jdoc["data"].toObject()["user"].toObject();
    ___s.setValue("userid", jo["f_id"].toInt());
    ui->wc->saveChanges();
}

void C5Server5::showNotificationWidget(const QJsonObject &jdoc)
{
    NotificationWidget::showMessage(jdoc["message"].toString(),
                                    jdoc["id"].toString(),
                                    (char *)SLOT(notificationClosed(QVariant)),
                                    this);
}

void C5Server5::notificationClosed(const QVariant &v)
{
    qDebug() << v;
}

void C5Server5::messageReceived(const QString &message)
{
    QJsonObject jo = __strjson(message);
    if (jo.contains("chat")) {
        QJsonArray ja = jo["chat"].toArray();
        for (int i = 0; i < ja.count(); i++) {
            QJsonObject jc = ja.at(i).toObject();
            QJsonObject jm = jc["f_body"].toObject();
            NotificationWidget::showMessage(jc["f_last"].toString()
                                            + jc["f_first"].toString()
                                            + "<br>"
                                            + jm["text"].toString(), 0, nullptr, nullptr);
            auto *thread = new QThread();
            jc["chat"] = true;
            jc["text"] = jm["text"];
            auto *worker = new C5ServerPrinter(jc);
            worker->moveToThread(thread);
            connect(thread, &QThread::started, worker, &C5ServerPrinter::run);
            connect(worker, &C5ServerPrinter::finished, thread, &QThread::quit);
            connect(worker, &C5ServerPrinter::showNotificationWidget, this, &C5Server5::showNotificationWidget);
            connect(thread, &QThread::finished, worker, &QObject::deleteLater);
            connect(thread, &QThread::finished, thread, &QObject::deleteLater);
            thread->start();
        }
    }
}
