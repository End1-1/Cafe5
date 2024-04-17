#include "c5servername.h"
#include "c5message.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QThread>

QJsonArray C5ServerName::mServers;

C5ServerName::C5ServerName(const QString &server, QObject *parent)
    : QObject{parent},
    mServer(server)
{

}

void C5ServerName::getServers()
{
    QNetworkAccessManager m;
    QString host = QString("https://%1:10002/%2").arg(mServer, "servernames");
    QNetworkRequest rq(host);
    m.setTransferTimeout(60000);
    rq.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QSslConfiguration sslConf = rq.sslConfiguration();
    sslConf.setPeerVerifyMode(QSslSocket::VerifyNone);
    sslConf.setProtocol(QSsl::AnyProtocol);
    rq.setSslConfiguration(sslConf);
    QJsonObject jo;
    jo["key"] = "asdf7fa8kk49888d!!jjdjmskkak98983mj???m";

    auto *r = m.post(rq, QJsonDocument(jo).toJson(QJsonDocument::Compact));
    while (!r->isFinished()) {
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        QThread::msleep(10);
    }
    if (r->error() != QNetworkReply::NoError) {
        C5Message::error(r->errorString());
        return;
    }
    QByteArray ba = r->readAll();
    mServers = QJsonDocument::fromJson(ba).array();
    qDebug() << mServers;

}
