#include "ndataprovider.h"
#include "logwriter.h"
#include <QHostInfo>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSslConfiguration>
#include <QJsonDocument>
#include <QJsonObject>
#include <QElapsedTimer>

bool  NDataProvider::mDebug = false;
QString NDataProvider::sessionKey;
QString NDataProvider::mProtocol;
QString NDataProvider::mHost;

NDataProvider::NDataProvider(QObject *parent)
    : QObject(parent)
{
    mNetworkAccessManager = new QNetworkAccessManager(this);
    mNetworkAccessManager->setTransferTimeout(30000);
    connect(mNetworkAccessManager, &QNetworkAccessManager::finished, this, &NDataProvider::queryFinished);
    mTimer = new QElapsedTimer();
}

NDataProvider::~NDataProvider()
{
    delete mTimer;
}

void NDataProvider::getData(const QString &route, const QJsonObject &data)
{
    mTimer->restart();
    emit started();
    // if (mHost.startsWith("http")) {
    //     mProtocol.clear();
    // }
    // if (mProtocol.isEmpty() && !mHost.contains("https")) {
    //     mProtocol = "https://";
    // }
    QNetworkRequest rq(mHost + route);
    rq.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject jo;
    jo["sessionkey"] = sessionKey;
    jo["hostinfo"] = QHostInfo::localHostName().toLower();
    QStringList keys = data.keys();
    for (const auto &s : qAsConst(keys)) {
        jo[s] = data[s];
    }
    if (mDebug) {
        LogWriter::write(LogWriterLevel::verbose, "", route);
        LogWriter::write(LogWriterLevel::verbose, "", QJsonDocument(data).toJson(QJsonDocument::Compact));
    } else {
#ifdef QT_DEBUG
        LogWriter::write(LogWriterLevel::verbose, "", mHost + route);
        LogWriter::write(LogWriterLevel::verbose, "", QJsonDocument(jo).toJson(QJsonDocument::Compact));
#endif
    }
    mNetworkAccessManager->post(rq, QJsonDocument(jo).toJson(QJsonDocument::Compact));
}

void NDataProvider::changeTimeout(int value)
{
    mNetworkAccessManager->setTransferTimeout(value);
}

void NDataProvider::queryFinished(QNetworkReply *r)
{
    if (r->error() != QNetworkReply::NoError) {
        QString err = r->errorString() + r->readAll();
        if (err.contains("<html>")) {
            err = err.mid(err.indexOf("<html>"), err.length());
        } else {
            err = err.mid(err.indexOf("Server error") + 12, err.length());
        }
        LogWriter::write(LogWriterLevel::errors, "Error", err);
        emit error(err);
        r->deleteLater();
        return;
    }
    QByteArray ba = r->readAll();
    r->deleteLater();
    if (mDebug) {
        LogWriter::write(LogWriterLevel::verbose, "Data size", (ba.size() < 1000 ? QString("%1 bytes").arg(ba.size())
                         : (ba.size() < 1000000 ? QString("%1 kb").arg(ba.size() / 1000) : QString("%1 mb").arg(
                                ba.size() / 1000000))));
        LogWriter::write(LogWriterLevel::verbose, "Elapsed", QString::number(mTimer->elapsed()));
        LogWriter::write(LogWriterLevel::verbose, "", ba);
    } else {
#ifdef QT_DEBUG
        LogWriter::write(LogWriterLevel::verbose, "Data size", (ba.size() < 1000 ? QString("%1 bytes").arg(ba.size())
                         : (ba.size() < 1000000 ? QString("%1 kb").arg(ba.size() / 1000) : QString("%1 mb").arg(
                                ba.size() / 1000000))));
        LogWriter::write(LogWriterLevel::verbose, "Elapsed", QString::number(mTimer->elapsed()));
        LogWriter::write(LogWriterLevel::verbose, "", ba);
#endif
    }
    QJsonParseError err;
    QJsonObject jdoc = QJsonDocument::fromJson(ba, &err).object();
    if (err.error == QJsonParseError::NoError) {
        emit done(jdoc);
    } else {
        LogWriter::write(LogWriterLevel::errors, err.errorString(), ba);
        emit error(ba);
    }
}
