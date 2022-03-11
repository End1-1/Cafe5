#include "map.h"
#include "ui_map.h"
#include "structs.h"
#include "messagelist.h"
#include "rawmessage.h"
#include <QDateTime>
#include <QQuickItem>

QMap<quint32, CoordinateData> fUsersPos;

Map::Map(QWidget *parent) :
    SocketWidget(parent),
    ui(new Ui::Map)
{
    ui->setupUi(this);
    ui->qmap->setSource(QUrl(QStringLiteral("qrc:///qmap.qml")));
    fMapObject = ui->qmap->rootObject();
    QMetaObject::invokeMethod(fMapObject, "clearMarkers");
    for (auto [k, c]: fUsersPos.toStdMap()) {
        QMetaObject::invokeMethod(fMapObject, "createMarker",
                                  Q_ARG(QVariant, c.latitude),
                                  Q_ARG(QVariant, c.longitude),
                                  Q_ARG(QVariant, c.azimuth),
                                  Q_ARG(QVariant, k));
    }
//    QVariant out;
//    QMetaObject::invokeMethod(qml, "getPlugins", Q_RETURN_ARG(QVariant, out));
//    fProviders = out.toStringList();
//    qDebug() << fProviders;
//    QVariant provider = fProviders.at(4);
//    QMetaObject::invokeMethod(qml, "createMapWindow", Q_ARG(QVariant, provider));
}

Map::~Map()
{
    delete ui;
}

void Map::externalDataReceived(quint16 cmd, const QByteArray &data)
{
    RawMessage r(nullptr, data);
    switch (cmd) {
    case MessageList::srv_device_position:
        quint32 user = r.readUInt();
        CoordinateData cd;
        r.readBytes(reinterpret_cast<char*>(&cd));
        QDateTime dt;
        dt.setMSecsSinceEpoch(cd.datetime);
        if (fUsersPos.contains(user)) {
            QMetaObject::invokeMethod(fMapObject, "updateMarker", Q_ARG(QVariant, cd.latitude), Q_ARG(QVariant, cd.longitude), Q_ARG(QVariant, cd.azimuth), Q_ARG(QVariant, user));
        } else {
            QMetaObject::invokeMethod(fMapObject, "createMarker", Q_ARG(QVariant, cd.latitude), Q_ARG(QVariant, cd.longitude), Q_ARG(QVariant, cd.azimuth), Q_ARG(QVariant, user));
        }
        fUsersPos[user] = cd;
        break;
    }
}
