#include "c5sockethandler.h"

QString __socketServerHost = "";
quint16 __socketServerPort = 1000;

C5SocketHandler::C5SocketHandler(QTcpSocket *socket, QObject *parent) :
    QObject(parent)
{
    fSocket = socket;
    if (fSocket) {
        fSocket->setParent(this);
    }
    if (__socketServerHost.length() == 0) {
        return;
    }
    fReadState = false;
    fErrorCode = 0;
    fDataSize = 0;
    if (fSocket == nullptr) {
        fSocket = new QTcpSocket(parent);
        connect(fSocket, SIGNAL(connected()), this, SLOT(connected()));
        qRegisterMetaType <QAbstractSocket::SocketError> ("QAbstractSocket::SocketError");
        connect(fSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));
        fSocket->connectToHost(__socketServerHost, __socketServerPort);
        fSocket->waitForConnected(5000);
    }
    connect(fSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

C5SocketHandler::~C5SocketHandler()
{
    fSocket->deleteLater();
}

void C5SocketHandler::setServerAddress(const QString &serverIP)
{
    __socketServerHost = serverIP;
}

QVariant &C5SocketHandler::operator[](const QString &key)
{
    return fBindValues[key];
}

void C5SocketHandler::bind(const QString &key, const QVariant &value)
{
    fBindValues[key] = value;
}

void C5SocketHandler::send()
{
    if (!fSocket) {
        return;
    }
    QJsonObject jObj;
    for (QMap<QString, QVariant>::const_iterator it = fBindValues.constBegin(); it != fBindValues.constEnd(); it++) {
        switch (it.value().typeId()) {
            case QMetaType::Int:
                jObj[it.key()] = it.value().toInt();
                break;
            case QMetaType::Double:
                jObj[it.key()] = it.value().toDouble();
                break;
            default:
                jObj[it.key()] = it.value().toString();
                break;
        }
    }
    QJsonDocument jDoc(jObj);
    QByteArray data = jDoc.toJson();
    int docSize = data.size();
    if (fSocket->write(reinterpret_cast<const char * >( &docSize), sizeof(quint32)) < 0) {
        QJsonObject obj;
        obj["reply"] = 0;
        obj["msg"] = QString("Socket error.\r\n%1\r\n%2:%3")
                     .arg(fSocket->errorString())
                     .arg(__socketServerHost)
                     .arg(__socketServerPort);
        emit handleCommand(obj);
        return;
    }
    fSocket->write(data);
    fSocket->flush();
}

void C5SocketHandler::send(int errorCode)
{
    fErrorCode = errorCode;
    send();
}

void C5SocketHandler::send(QJsonObject &obj)
{
    for (QMap<QString, QVariant>::const_iterator it = fBindValues.begin(); it != fBindValues.end(); it++) {
        switch (it.value().typeId()) {
            case QMetaType::Int:
                obj[it.key()] = it.value().toInt();
                break;
            default:
                obj[it.key()] = it.value().toString();
                break;
        }
    }
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();
    int docSize = data.length();
    fSocket->write(reinterpret_cast<const char *>( &docSize), sizeof(quint32));
    fSocket->write(data);
    fSocket->flush();
}

void C5SocketHandler::close()
{
    fSocket->close();
}

void C5SocketHandler::connected()
{
}

void C5SocketHandler::readyRead()
{
    if (!fReadState) {
        char *c = new char[sizeof(quint32)];
        fSocket->read(c, sizeof(quint32));
        memcpy( &fDataSize, c, sizeof(quint32));
        fReadState = true;
    }
    fData.append(fSocket->readAll());
    if (fDataSize == fData.size()) {
        QJsonParseError je;
        QJsonDocument jDoc = QJsonDocument::fromJson(fData, &je);
        if (je.error == QJsonParseError::NoError) {
            emit handleCommand(jDoc.object());
        } else {
            emit handleError(fErrorCode, je.errorString());
            fSocket->close();
        }
        fData.clear();
        fDataSize = 0;
        fReadState = false;
    }
    if (fDataSize < fData.size()) {
        close();
    }
}

void C5SocketHandler::error(QAbstractSocket::SocketError err)
{
    Q_UNUSED(err);
    QTcpSocket *s = static_cast<QTcpSocket *>(sender());
    qDebug() << s->errorString();
}
