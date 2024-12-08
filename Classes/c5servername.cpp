#include "c5servername.h"
#include "c5message.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QThread>
#include <QWebSocket>

QJsonArray C5ServerName::mServers;

C5ServerName::C5ServerName(const QString &server, const QString &route, QObject *parent)
    : QObject{parent},
      mServer(server),
      mRoute(route)
{
}

bool C5ServerName::getServers(const QString &name)
{
    if (!name.isEmpty()) {
        mParams["name"] = name;
    }
    QWebSocket *s = new QWebSocket();
    QUrl url(QString("%1/ws").arg(mServer));
    QEventLoop l1;
    connect(s, &QWebSocket::connected, &l1, &QEventLoop::quit);
    connect(s, &QWebSocket::disconnected, &l1, &QEventLoop::quit);
    connect(this, &C5ServerName::messageReceived, &l1, &QEventLoop::quit);
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
    QJsonObject jo;
    jo["command"] = "ServerNamesAll";
    jo["handler"] = "office";
    jo["key"] = "asdf7fa8kk49888d!!jjdjmskkak98983mj???m";
    jo["params"] = mParams;
    s->sendTextMessage(QJsonDocument(jo).toJson(QJsonDocument::Compact));
    l2.exec();
    s->deleteLater();
    return true;
}
