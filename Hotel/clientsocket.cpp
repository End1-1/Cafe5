#include "clientsocket.h"
#include "cmd.h"
#include "settings.h"
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>

ClientSocket __socket;

ClientSocket::ClientSocket()
{
    fSocket = new QTcpSocket();
}

bool ClientSocket::connectToSocket(const QString &ip, int port, QJsonObject &jo)
{
    fSocket->connectToHost(ip, port);
    if (!fSocket->waitForConnected(5000)) {
        qDebug() << fSocket->errorString();
        return false;
    }
    fBindValues["cmd"] = Cmd::cmd_auth_cts;
    fBindValues["hostpassword"] = __s.s("hostpassword");
    fBindValues["username"] = __s.s("username");
    fBindValues["hostname"] = __s.s("hostname");
    if (!sendJson()) {
        return false;
    }
    QJsonDocument jdoc = QJsonDocument::fromJson(fBuff);
    jo = jdoc.object();
    fUUID = jo["uuid"].toString();
    return true;
}

void ClientSocket::disconnectFromHost()
{
    if (!fSocket->isOpen()) {
        return;
    }
    fBindValues["cmd"] = Cmd::cmd_disconnect;
    fBindValues["uuid"] = fUUID;
    sendJson();
    fSocket->close();
}

QVariant &ClientSocket::operator[](const QString &name)
{
    return fBindValues[name];
}

bool ClientSocket::sendJson()
{
    fBuff.clear();
    QJsonObject jo;
    for (auto v: fBindValues.keys()) {
        jo[v] = fBindValues[v].toString();
    }
    QJsonDocument jdoc(jo);
    fBuff.append(jdoc.toJson());
    fBuff.insert(0, reinterpret_cast<const char *>(&Cmd::cmdtype_json), sizeof(Cmd::cmdtype_json));
    quint32 dataSize = fBuff.length();
    fBuff.insert(0, reinterpret_cast<const char *>(&dataSize), sizeof(dataSize));
    fSocket->write(fBuff);
    if (!fSocket->waitForBytesWritten()) {
        return false;
    }
    fSocket->flush();
    dataSize = 0;
    fBuff.clear();
    while (fSocket->waitForReadyRead()) {
        if (dataSize == 0){
            fSocket->read(reinterpret_cast<char *>(&dataSize), sizeof(dataSize));
        }
        fBuff.append(fSocket->readAll());
        if (fBuff.length() >= dataSize) {
            break;
        }
    }
    if (fBuff.length() < dataSize) {
        return false;
    }
    return true;
}

QJsonObject ClientSocket::responseJson() const
{
    return QJsonDocument::fromJson(fBuff).object();
}
