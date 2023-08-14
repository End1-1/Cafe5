#include "rawyandexkey.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>

RawYandexKey::RawYandexKey(SslSocket *s) :
    Raw(s)
{

}

int RawYandexKey::run(const QByteArray &d)
{
    quint8 cmd;
    readUByte(cmd, d);
    switch (cmd) {
    case 1: {
        QString key = "06495363-2976-4cbb-a0b7-f09387554b9d";
        putUByte(1);
        putString(key);
        break;
    }
    case 2: {
        QString address;
        readString(address, d);
        QUrl url(QString("https://maps.googleapis.com/maps/api/geocode/json?language=hy&address=%1&key=%2")
                 .arg(address, "AIzaSyCqSpwxZC1076ws3X2dFnTgTwNcyEaWNTY"));
        QNetworkAccessManager *manager = new QNetworkAccessManager();
        QNetworkReply *http = manager->get(QNetworkRequest(url));
        QEventLoop eventLoop;
        QObject::connect(http, SIGNAL(finished()), &eventLoop, SLOT(quit()));
        eventLoop.exec();
        QJsonParseError err;
        QJsonDocument jdoc = QJsonDocument::fromJson(http->readAll(), &err);
        if (err.error != QJsonParseError::NoError) {
            putUByte(0);
            return 0;
        }
        QJsonObject jobj = jdoc.object();
        qDebug() << jobj;
        break;
    }
    case 3: {
        double lat, lon;
        readDouble(lat, d);
        readDouble(lon, d);
        QUrl url(QString("https://maps.googleapis.com/maps/api/geocode/json?language=hy&latlng=%1,%2&key=%3")
                 .arg(lat).arg(lon).arg("AIzaSyCqSpwxZC1076ws3X2dFnTgTwNcyEaWNTY"));
        QNetworkAccessManager *manager = new QNetworkAccessManager();
        QNetworkReply *http = manager->get(QNetworkRequest(url));
        QEventLoop eventLoop;
        QObject::connect(http, SIGNAL(finished()), &eventLoop, SLOT(quit()));
        eventLoop.exec();
        QJsonParseError err;
        QJsonDocument jdoc = QJsonDocument::fromJson(http->readAll(), &err);
        if (err.error != QJsonParseError::NoError) {
            putUByte(0);
            return 0;
        }
        QJsonObject jobj = jdoc.object();
        qDebug() << jobj;
        break;
    }
    }
    return 0;
}
