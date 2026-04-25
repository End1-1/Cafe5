#include "httplite.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
HttpLite::HttpLite(QObject *parent)
    : QObject{parent}
{}

void HttpLite::post(const QString &url, const QJsonObject &obj)
{
    static QNetworkAccessManager *manager = new QNetworkAccessManager();

    QNetworkRequest request((QUrl(url)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Превращаем JSON объект в массив байт для отправки
    QByteArray data = QJsonDocument(obj).toJson();

    // Отправляем запрос
    QNetworkReply *reply = manager->post(request, data);

    // Самый важный момент — освобождение ресурсов ответа
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            qDebug() << "Success:" << reply->readAll();
        } else {
            qDebug() << "Error:" << reply->errorString();
        }

        // Удаляем объект ответа только ПОСЛЕ того, как обработали сигнал finished
        reply->deleteLater();
        this->deleteLater();
    });
}
