#include "socketconnection.h"
#include "rawmessage.h"
#include "messagelist.h"
#include <QSslSocket>
#include <QThread>
#include <QTimer>
#include <QFile>
#include <QDataStream>
#include <QApplication>
#include <QMutex>
#include <QDebug>

QMutex fMutex;

SocketConnection::SocketConnection(const QString &phone, const QString &uuid, QObject *parent) :
    QObject(parent),
    fUuid(uuid),
    fPhone(phone)
{
    fRouteId = 0;
}

SocketConnection::~SocketConnection()
{
    fTimer->deleteLater();
    fSocket->deleteLater();
}

void SocketConnection::run()
{
    QString certFileName = qApp->applicationDirPath() + "/" + "cert.pem";
    QFile file(certFileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Certificate file missing";
        return;
    }
    QSslCertificate cert = QSslCertificate(file.readAll());

    fSocket = new SslSocket(this);
    fTimer = new QTimer(this);
    fRouteTimer = new QTimer(this);
    connect(fTimer, &QTimer::timeout, this, &SocketConnection::timeout);
    connect(fRouteTimer, &QTimer::timeout, this, &SocketConnection::routeTimeout);
    connect(fSocket, &QSslSocket::readyRead, this, &SocketConnection::readyRead);
    connect(fSocket, &QSslSocket::errorOccurred, this, &SocketConnection::errorOccurred);
    connect(fSocket, &QSslSocket::encrypted, this, &SocketConnection::encrypted);
    connect(fSocket, &QSslSocket::disconnected, this, &SocketConnection::disconnected);
    connect(this, &SocketConnection::dataReady, this, &SocketConnection::sendData);
    qDebug() << QSslSocket::supportsSsl() << QSslSocket::sslLibraryBuildVersionString() << QSslSocket::sslLibraryVersionString();
    fSocket->addCaCertificate(cert);
    fSocket->setPeerVerifyMode(QSslSocket::VerifyNone);
    fTimer->start(1000);
    fRouteTimer->start(1000);
}

void SocketConnection::sendData(QByteArray &d)
{
    RawMessage::setPacketNumber(d, getTcpPacketNumber());
    fSocket->write(d);
    fSocket->flush();
}

void SocketConnection::driverRoute(SocketConnection *s, const QString &filename)
{
    if (s == this) {
        fRoute.clear();
        QFile f(filename);
        if (f.open(QIODevice::ReadOnly)) {
            CoordinateData c;
            while (f.read(reinterpret_cast<char*>(&c), sizeof(c)) == sizeof(c)) {
                fRoute.append(c);
            }
            f.close();
        }
    }
}

quint32 SocketConnection::getTcpPacketNumber()
{
    QMutexLocker ml(&fMutex);
    return ++fTcpPacketNumber;
}

void SocketConnection::timeout()
{
    if (fSocket->state() == QSslSocket::ConnectedState) {
        fTimer->stop();
        return;
    } else {
        fSocket->connectToHostEncrypted("10.1.0.2", 10002);
    }
}

void SocketConnection::routeTimeout()
{
    if (fRouteId < fRoute.count() - 1) {
        CoordinateData c = fRoute.at(++fRouteId);
        RawMessage r(nullptr, QByteArray());
        r.setHeader(0, 0, MessageList::coordinate);
        r.putDouble(c.latitude);
        r.putDouble(c.longitude);
        r.putDouble(c.speed);
        r.putDouble(c.azimuth);
        r.putDouble(c.datetime);
        emit dataReady(r.data());
    } else {
        if (fRoute.constBegin() > 0) {
            fRouteId = 0;
        }
    }
}

void SocketConnection::encrypted()
{
    fTimer->stop();
    fTcpPacketNumber = 0;
    RawMessage r(fSocket, QByteArray());
    r.setHeader(0, 0, MessageList::hello);
    r.putString(fUuid);
    emit dataReady(r.data());
    r.clear();
    r.setHeader(0, 0, MessageList::silent_auth);
    r.putString(fPhone);
    r.putString(fUuid);
    emit dataReady(r.data());
//    r.clear();
//    r.setHeader(0, 0, MessageList::monitor);
//    emit dataReady(r.data());
    emit connected();
}

void SocketConnection::disconnected()
{
    fTcpPacketNumber = 0;
    emit connectionLost();
    fTimer->start(3000);
}

void SocketConnection::readyRead()
{
    QByteArray ba;
    ba.append(0x03);
    ba.append(0x04);
    ba.append(0x15);

    if (fData.isEmpty()) {
        fData = fSocket->read(3);
        if (fData.compare(ba) != 0) {
            fSocket->disconnectFromHost();
        }
    }
    fData.append(fSocket->readAll());
    quint32 headersize = 3 + sizeof(fMessageNumber) + sizeof(fMessageId) + sizeof(fMessageCommand) + sizeof(fMessageSize);
    while (fData.length() >= headersize) {
        int pos = 3;
        memcpy(&fMessageNumber, &fData.data()[pos], sizeof(fMessageNumber));
        pos += sizeof(fMessageNumber);
        memcpy(&fMessageId, &fData.data()[pos], sizeof(fMessageId));
        pos += sizeof(fMessageId);
        memcpy(&fMessageCommand, &fData.data()[pos], sizeof(fMessageCommand));
        pos += sizeof(fMessageCommand);
        memcpy(&fMessageSize, &fData.data()[pos], sizeof(fMessageSize));

        if (fData.length() - headersize >= fMessageSize) {
            emit externalDataReady(fMessageCommand, fData.mid(headersize, fMessageSize));
            fData.remove(0, headersize + fMessageSize);
        } else {
            break;
        }
    }
}

void SocketConnection::errorOccurred(QAbstractSocket::SocketError error)
{
    qDebug() << error;
    fSocket->disconnectFromHost();
}
