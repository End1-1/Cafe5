#include "c5servername.h"
#include "c5message.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QThread>

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
    QNetworkAccessManager m;
    QString host = QString("https://%1:10002/%2").arg(mServer, mRoute);
    QNetworkRequest rq(host);
    m.setTransferTimeout(60000);
    rq.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QSslConfiguration sslConf = rq.sslConfiguration();
    sslConf.setPeerVerifyMode(QSslSocket::VerifyNone);
    sslConf.setProtocol(QSsl::AnyProtocol);
    rq.setSslConfiguration(sslConf);
    QJsonObject jo;
    jo["key"] = "asdf7fa8kk49888d!!jjdjmskkak98983mj???m";
    jo["params"] = mParams;
    auto *r = m.post(rq, QJsonDocument(jo).toJson(QJsonDocument::Compact));
    while (!r->isFinished()) {
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        QThread::msleep(10);
    }
    if (r->error() != QNetworkReply::NoError) {
        C5Message::error(r->errorString() + "<br>" + r->readAll());
        return false;
    }
    QByteArray ba = r->readAll();
    mServers = QJsonDocument::fromJson(ba).array();
    qDebug() << mServers;
    return true;
}
