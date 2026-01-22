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
QString NDataProvider::mAppName;
QString NDataProvider::mFileVersion;

NDataProvider::NDataProvider(QObject *parent)
    : QObject(parent)
{
    mNetworkAccessManager = new QNetworkAccessManager(this);
    mNetworkAccessManager->setTransferTimeout(60000);
    connect(mNetworkAccessManager, &QNetworkAccessManager::finished, this, &NDataProvider::queryFinished);
    mTimer = new QElapsedTimer();
    mConnectionProtocol = mProtocol;
    mConnectionHost = mHost;
    mConnectionPort = mConnectionProtocol == "https" ? 443 : 80;
}

NDataProvider::~NDataProvider()
{
    delete mTimer;
}

void NDataProvider::getData(const QString &route, const QJsonObject &data)
{
    Q_ASSERT(mAppName.isEmpty() == false);
    Q_ASSERT(mFileVersion.isEmpty() == false);
    mStartDate = QDateTime::currentDateTime();
    mTimer->restart();
    emit started();
    QString url = QString("%1://%2/%3").arg(mConnectionProtocol, mConnectionHost, route);
    QNetworkRequest rq(url);
    rq.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    rq.setRawHeader("Authorization", "Bearer " + (mBearer.isEmpty() ? sessionKey.toUtf8() : mBearer.toUtf8()));
    rq.setRawHeader("X-Application-Name", mAppName.toUtf8());
    rq.setRawHeader("X-Application-Version", mFileVersion.toUtf8());
    QJsonObject jo;
    jo["sessionkey"] = mBearer.isEmpty() ? sessionKey : mBearer;
    jo["hostinfo"] = QHostInfo::localHostName().toLower();
    jo["app"] = mAppName;
    jo["appversion"] = mFileVersion;
#ifdef QT_DEBUG
    jo["debug"] = true;
#endif
    QStringList keys = data.keys();

    for(const auto &s : std::as_const(keys)) {
        jo[s] = data[s];
    }

    if(mDebug) {
        LogWriter::write(LogWriterLevel::verbose, "", url);
        LogWriter::write(LogWriterLevel::verbose, "", QJsonDocument(jo).toJson(QJsonDocument::Compact));
    } else {
#ifdef QT_DEBUG
        LogWriter::write(LogWriterLevel::verbose, "", url);
        LogWriter::write(LogWriterLevel::verbose, "", QJsonDocument(jo).toJson(QJsonDocument::Compact));
#endif
    }

    mNetworkAccessManager->post(rq, QJsonDocument(jo).toJson(QJsonDocument::Compact));
}

void NDataProvider::changeTimeout(int value)
{
    mNetworkAccessManager->setTransferTimeout(value);
}

void NDataProvider::changeBearer(const QString &bearer)
{
    mBearer = bearer;
}

void NDataProvider::overwriteHost(const QString &protocol, const QString &host, int port)
{
    mConnectionProtocol = protocol;
    mConnectionHost = host;
    mConnectionPort = port;
}

void NDataProvider::queryFinished(QNetworkReply *r)
{
    int httpCode = r->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QByteArray ba = r->readAll();

    if(r->error() != QNetworkReply::NoError) {
        QString err = r->errorString() + r->readAll();

        if(httpCode == 426) {
            QJsonObject jmsg = QJsonDocument::fromJson(ba).object();
            QString msg = QString("%1<br>%2 > %3<br><p><a href=\"launch-updater?version=%3\">%4</a></p>")
                          .arg(tr("Application update required"),
                               jmsg.value("old_version").toString(),
                               jmsg.value("new_version").toString(),
                               tr("Click to launch updater"));
            emit updateRequired(msg, mAppName, jmsg.value("new_version").toString());
            return;
        }

        if(err.contains("access denied", Qt::CaseInsensitive)) {
            err = tr("Access denied");
        } else if(err.contains("Application version must be", Qt::CaseInsensitive) || err.contains("Application update required", Qt::CaseInsensitive)) {
            LogWriter::write(LogWriterLevel::errors, "", err);
            QJsonObject  je = QJsonDocument::fromJson(err.toUtf8()).object();
            err = QString("<p>%1</p><p><a href=\"launch-updater?version=%3\">%2</a></p>")
                  .arg(tr("You must upgrade the application to continue using it"),
                       tr("Click here to launch updater"),
                       je["expected_version"].toString());
        } else if(err.contains("server replied:")) {
            int index = err.indexOf("server replied:") + 15;
            err = err.mid(index, err.length());
            index = err.indexOf("Server error");

            if(index > -1) {
                index += 12;
            } else {
                index = 0;
            }

            err = err.mid(index, err.length());
        } else if(err.contains("<html>")) {
            err = err.mid(err.indexOf("<html>"), err.length());
        } else  if(err.contains("Unauthorized", Qt::CaseInsensitive)) {
            err = tr("Access denied");
        } else {
            int index = err.indexOf("Server error");

            if(index > -1) {
                index += 12;
            } else {
                index = 0;
            }

            err = err.mid(index, err.length());
        }

        LogWriter::write(LogWriterLevel::errors, "Error", err);
        emit error(err);
        r->deleteLater();
        return;
    }

    r->deleteLater();

    if(mDebug) {
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

    if(err.error == QJsonParseError::NoError) {
        jdoc["query_start"] = mStartDate.toString("yyyy-MM-dd HH:mm:ss.zzz");
        emit done(jdoc);
    } else {
        LogWriter::write(LogWriterLevel::errors, err.errorString(), ba);
        emit error(ba);
    }
}
