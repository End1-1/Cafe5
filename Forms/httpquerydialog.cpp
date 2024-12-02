#include "httpquerydialog.h"
#include "ui_httpquerydialog.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QWebSocket>
#include <QJsonDocument>
#include <QTimer>

HttpQueryDialog::HttpQueryDialog(const QStringList &dbParams, const QString &url, const QJsonObject &jo,
                                 QWidget *parent) :
    C5Dialog(dbParams, parent),
    ui(new Ui::HttpQueryDialog),
    fUrl(url),
    jRequest(jo)
{
    ui->setupUi(this);
}

HttpQueryDialog::~HttpQueryDialog()
{
    delete ui;
}

int HttpQueryDialog::exec()
{
    QTimer *t = new QTimer(this);
    connect(t, &QTimer::timeout, this, [ = ] {
        sendRequest();
    });
    t->start(1000);
    return C5Dialog::exec();
}

void HttpQueryDialog::sendRequest()
{
    static_cast<QTimer *>(sender())->stop();
    sender()->deleteLater();
    QWebSocket *s = new QWebSocket();
    QUrl url(QString("%1").arg(fUrl));
    QEventLoop l1;
    connect(s, &QWebSocket::connected, &l1, &QEventLoop::quit);
    connect(s, &QWebSocket::disconnected, &l1, &QEventLoop::quit);
    connect(this, &HttpQueryDialog::exitLoop, &l1, &QEventLoop::quit);
    connect(this, &HttpQueryDialog::messageReceived, &l1, &QEventLoop::quit);
    s->open(url);
    l1.exec();
    QEventLoop l2;
    connect(s, &QWebSocket::textMessageReceived, this, [this](const QString &s) {
        qDebug() << s;
        QJsonObject jrep = QJsonDocument::fromJson(s.toUtf8()).object();
        emit messageReceived();
        accept();
    });
    connect(this, &HttpQueryDialog::messageReceived, &l2, &QEventLoop::quit);
    connect(s, &QWebSocket::disconnected, &l2, &QEventLoop::quit);
    connect(this, &HttpQueryDialog::messageReceived, &l2, &QEventLoop::quit);
    connect(s, &QWebSocket::disconnected, &l2, &QEventLoop::quit);
    s->sendTextMessage(QJsonDocument(jRequest).toJson(QJsonDocument::Compact));
    l2.exec();
}

void HttpQueryDialog::connectError(QAbstractSocket::SocketError error)
{
    emit exitLoop();
}
