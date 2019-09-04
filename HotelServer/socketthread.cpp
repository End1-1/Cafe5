#include "socketthread.h"
#include "cmd.h"
#include "c5database.h"
#include "jsonutils.h"
#include <QUuid>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QHostAddress>

QString hostPassword;
QString dbHost;
int dbPort;
QString dbFile;
QString dbUser;
QString dbPass;

SocketThread::SocketThread(QTcpSocket *socket, QObject *parent) :
    QObject(parent)
{
    fDataSize = 0;
    fUUID = QUuid::createUuid().toByteArray();
    qRegisterMetaType <QAbstractSocket::SocketError> ("QAbstractSocket::SocketState");
    fSocket = socket;
    connect(fSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(fSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));
    connect(fSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

SocketThread::~SocketThread()
{
    qDebug() << "SocketThread destructor";
}

void SocketThread::sendData(const QString &uuid)
{
    if (fUUID != uuid) {
        return;
    }
    fDataSize = fData.length();
    fSocket->write(reinterpret_cast<const char *>(&fDataSize), sizeof(fDataSize));
    fSocket->write(fData);
    fSocket->flush();
    fData.clear();
    fDataSize = 0;
}

void SocketThread::processJson()
{
    QJsonDocument jdoc = QJsonDocument::fromJson(fData);
    QJsonObject jo = jdoc.object();
    switch(jo["cmd"].toString().toInt()) {
    case Cmd::cmd_auth_cts:
        jsonAuth(jo);
        break;
    case Cmd::cmd_auth_user:
        jsonAuthUser(jo);
        break;
    case Cmd::cmd_disconnect:
        jsonDisconnect(jo);
        break;
    }
}

void SocketThread::jsonAuth(QJsonObject &jo)
{
    if (jo["hostpassword"].toString() != hostPassword) {
        fSocket->close();
        return;
    } else {
        emit registerConnection(fUUID, "IN", QHostAddress(fSocket->peerAddress().toIPv4Address()).toString(), jo["hostname"].toString(), jo["username"].toString());
        C5Database db(dbHost, dbFile, dbUser, dbPass);
        db[":fhost"] = jo["hostname"].toString();
        db.exec("select * from dblist where fid in (select fdb from dbaccess where fallow=1 and fhost=:fhost)");
        QJsonArray ja = fetchFromDb(db);
        jo = QJsonObject();
        jo["uuid"] = fUUID;
        jo["dblist"] = ja;
        fData = QJsonDocument(jo).toJson();
        write();
    }
}

void SocketThread::jsonAuthUser(QJsonObject &jo)
{
    QJsonObject jor;
    jor["reply"] = 0;
    C5Database db(dbHost, dbFile, dbUser, dbPass);
    db[":fid"] = jo["db"].toString();
    db.exec("select fhost, fdb, fuser, fpassword from dblist where fid=:fid");
    if (db.nextRow()) {
        C5Database db2(db.getString(0), db.getString(1), db.getString(2), db.getString(3));
        db2[":fusername"] = jo["username"].toString();
        db2[":fpassword"] = jo["password"].toString();
        db2.exec("select * from users where fusername=:fusername and fpassword=md5(:fpassword)");
        QJsonArray ja = fetchFromDb(db2);
        if (ja.count() > 0) {
            jor["reply"] = 1;
            jor["user"] = ja;
            db2[":fgroup"] = ja.at(0).toObject()["fgroup"].toString();
            db2.exec("select frole from users_permissions where fgroup=:fgroup");
            ja = fetchFromDb(db2);
            jor["role"] = ja;
        }
    }
    fData = QJsonDocument(jor).toJson();
    write();
}

void SocketThread::jsonDisconnect(QJsonObject &jo)
{
    emit unregisterConnection(jo["uuid"].toString(), "By user");
    disconnect(fSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    fSocket->close();
    deleteLater();
}

void SocketThread::write()
{
    fDataSize = fData.length();
    fSocket->write(reinterpret_cast<const char *>(&fDataSize), sizeof(fDataSize));
    fSocket->write(fData);
    fSocket->waitForBytesWritten();
    fSocket->flush();
    emit connectionRXTX(fUUID, 0, fDataSize + sizeof(fDataSize));
    fData.clear();
    fDataSize = 0;
}

void SocketThread::disconnected()
{
    emit unregisterConnection(fUUID, "Lost");
}

void SocketThread::error(QAbstractSocket::SocketError err)
{
    qDebug() << err;
}

void SocketThread::readyRead()
{
    if (fDataSize == 0) {
        fSocket->read(reinterpret_cast<char *>(&fDataSize), sizeof(fDataSize));
    }
    while (fSocket->bytesAvailable() && fDataSize > fData.length()) {
        fData.append(fSocket->read(fDataSize - fData.length()));
    }
    if (fData.length() == fDataSize) {
        quint32 cmdtype = 0;
        memcpy(&cmdtype, fData.data(), sizeof(cmdtype));
        fData.remove(0, sizeof(cmdtype));
        switch (cmdtype) {
        case Cmd::cmdtype_json:
            processJson();
            break;
        }
        emit connectionRXTX(fUUID, fDataSize + sizeof(fDataSize), 0);
    } else if (fData.length() > fDataSize) {
        fSocket->close();
    }
}
