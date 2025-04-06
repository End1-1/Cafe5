#include "c5servername.h"
#include "c5config.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QThread>
#include <QWebSocket>

QJsonArray C5ServerName::mServers;

C5ServerName::C5ServerName(const QString &server, QObject *parent)
    : QObject{parent},
      mServer(server)
{
}

bool C5ServerName::getServers()
{
    connect( &mTimer, &QTimer::timeout, this, []() {
        qDebug() << "Websocket timeout";
    });
    QWebSocket *s = new QWebSocket();
    QUrl url(QString("%1/ws").arg(mServer));
    QEventLoop l1;
    connect(s, &QWebSocket::connected, &l1, &QEventLoop::quit);
    connect(s, &QWebSocket::disconnected, &l1, &QEventLoop::quit);
    connect(this, &C5ServerName::messageReceived, &l1, &QEventLoop::quit);
    connect( &mTimer, &QTimer::timeout, &l1, &QEventLoop::quit);
    mTimer.start(10000);
    s->open(url);
    l1.exec();
    if (s->state() != QAbstractSocket::ConnectedState) {
        qDebug() << s->error() << s->errorString();
        mErrorString = s->errorString();
        s->deleteLater();
        return false;
    }
    QEventLoop l2;
    connect(s, &QWebSocket::textMessageReceived, this, [this](const QString &s) {
        fLastTextMessage = s;
        QJsonObject jrep = QJsonDocument::fromJson(s.toUtf8()).object();
        mServers = jrep["result"].toArray();
        emit messageReceived();
    });
    connect(this, &C5ServerName::messageReceived, &l2, &QEventLoop::quit);
    connect(s, &QWebSocket::disconnected, &l2, &QEventLoop::quit);
    connect( &mTimer, &QTimer::timeout, &l2, &QEventLoop::quit);
    QJsonObject jo;
    jo["command"] = "get_db_list";
    jo["server_key"] = __c5config.getRegValue("ss_server_key").toString();
    jo["handler"] = "office";
    s->sendTextMessage(QJsonDocument(jo).toJson(QJsonDocument::Compact));
    l2.exec();
    s->deleteLater();
    return true;
}

bool C5ServerName::getConnection(const QString &connectionName)
{
    connect( &mTimer, &QTimer::timeout, this, []() {
        qDebug() << "Websocket timeout";
    });
    QWebSocket *s = new QWebSocket();
    QUrl url(QString("%1/ws").arg(mServer));
    QEventLoop l1;
    connect(s, &QWebSocket::connected, &l1, &QEventLoop::quit);
    connect(s, &QWebSocket::disconnected, &l1, &QEventLoop::quit);
    connect(this, &C5ServerName::messageReceived, &l1, &QEventLoop::quit);
    connect( &mTimer, &QTimer::timeout, &l1, &QEventLoop::quit);
    mTimer.start(10000);
    s->open(url);
    l1.exec();
    if (s->state() != QAbstractSocket::ConnectedState) {
        qDebug() << s->error() << s->errorString();
        mErrorString = s->errorString();
        s->deleteLater();
        return false;
    }
    QEventLoop l2;
    connect(s, &QWebSocket::textMessageReceived, this, [this](const QString &s) {
        fLastTextMessage = s;
        QJsonObject jrep = QJsonDocument::fromJson(s.toUtf8()).object();
        mReply = jrep["result"].toObject();
        emit messageReceived();
    });
    connect(this, &C5ServerName::messageReceived, &l2, &QEventLoop::quit);
    connect(s, &QWebSocket::disconnected, &l2, &QEventLoop::quit);
    connect( &mTimer, &QTimer::timeout, &l2, &QEventLoop::quit);
    QJsonObject jo;
    jo["command"] = "get_connection";
    jo["server_key"] = __c5config.getRegValue("ss_server_key").toString();
    jo["handler"] = "office";
    jo["connection_name"] = connectionName;
    s->sendTextMessage(QJsonDocument(jo).toJson(QJsonDocument::Compact));
    l2.exec();
    s->deleteLater();
    return true;
}
