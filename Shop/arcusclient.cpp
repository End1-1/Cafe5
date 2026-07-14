#include "arcusclient.h"
#include <QCoreApplication>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QThread>
#include <QUrl>

bool ArcusClient::chargeCard(const QString &address,
                             int port,
                             const QString &apiKey,
                             qint64 amountMinor,
                             QJsonObject &response,
                             QString &error)
{
    response = QJsonObject();
    const QString host = address.trimmed();
    if (host.isEmpty()) {
        error = QCoreApplication::translate("ArcusClient", "Arcus address is not configured");
        return false;
    }
    if (port <= 0) {
        error = QCoreApplication::translate("ArcusClient", "Arcus port is not configured");
        return false;
    }
    if (apiKey.trimmed().isEmpty()) {
        error = QCoreApplication::translate("ArcusClient", "Arcus API key is not configured");
        return false;
    }
    if (amountMinor <= 0) {
        error = QCoreApplication::translate("ArcusClient", "Invalid payment amount");
        return false;
    }

    const QUrl url(QStringLiteral("http://%1:%2/api/pay").arg(host).arg(port));
    if (!url.isValid()) {
        error = QCoreApplication::translate("ArcusClient", "Invalid Arcus URL");
        return false;
    }

    QNetworkAccessManager manager;
    manager.setTransferTimeout(120000);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
    request.setRawHeader("x-api-key", apiKey.trimmed().toUtf8());

    const QJsonObject payload{{QStringLiteral("amountMinor"), amountMinor}};
    QNetworkReply *reply = manager.post(request, QJsonDocument(payload).toJson(QJsonDocument::Compact));

    while (!reply->isFinished()) {
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        QThread::msleep(10);
    }

    const QByteArray body = reply->readAll();
    if (reply->error() != QNetworkReply::NoError) {
        error = reply->errorString();
        if (!body.isEmpty()) {
            error += QStringLiteral(": ") + QString::fromUtf8(body);
        }
        reply->deleteLater();
        return false;
    }
    reply->deleteLater();

    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(body, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        error = QCoreApplication::translate("ArcusClient", "Invalid Arcus response");
        return false;
    }

    response = doc.object();
    if (!response.value(QStringLiteral("approved")).toBool()) {
        error = response.value(QStringLiteral("message")).toString();
        if (error.isEmpty()) {
            error = response.value(QStringLiteral("errorMessage")).toString();
        }
        if (error.isEmpty()) {
            error = QCoreApplication::translate("ArcusClient", "Card payment was not approved");
        }
        return false;
    }

    return true;
}
