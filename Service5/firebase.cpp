#include "firebase.h"
#include <QUrl>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

Firebase *Firebase::fInstance = nullptr;

Firebase::Firebase(QObject *parent) :
    QObject(parent)
{
    fNetworkAccessManager = new QNetworkAccessManager(this);
    connect(fNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished(QNetworkReply*)), Qt::DirectConnection);
}

Firebase::~Firebase()
{
    qDebug() << "~Firebase";
}

Firebase *Firebase::instance()
{
    if (fInstance == nullptr) {
        fInstance = new Firebase();
    }
    return fInstance;
}

void Firebase::sendMessage(const QString &token, const QString &msg)
{
    QJsonObject jo;
    jo["to"] = token;
    QJsonObject joNotification;
    joNotification["sound"] = "default";
    joNotification["body"] = msg;
    joNotification["title"] = "Your code";
    joNotification["content_available"] = true;
    joNotification["priority"] = "high";
    joNotification["click_action"] = "FLUTTER_NOTIFICATION_CLICK";
    QJsonObject joData;
    joData["sound"] = "default";
    joData["body"] = "THE DATA BODY";
    joData["title"] = "THE DATA TITLE";
    joData["content_available"] = true;
    joData["priority"] = "high";
    jo["notification"] = joNotification;
    jo["data"] = joData;
    QByteArray requestData = QJsonDocument(jo).toJson(QJsonDocument::Compact);
    QUrl url("https://fcm.googleapis.com/v1/projects/myproject-b5ae1/messages:send");
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", "Bearer AIzaSyBhPq9j-KVryNw-T3evesRW35MG43qKdbo");
    request.setRawHeader("Content-Type", "application/json");
    fNetworkAccessManager->post(request, requestData);
}

void Firebase::finished(QNetworkReply *reply)
{
    qDebug() << reply->readAll();
    reply->deleteLater();
    emit sendFinished();
}
