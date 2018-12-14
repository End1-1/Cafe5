#include "c5sockethandler.h"

QString __socketServerHost = "10.1.0.2";
quint16 __socketServerPort = 1000;

C5SocketHandler::C5SocketHandler(QTcpSocket *socket, QObject *parent) :
    QObject(parent)
{
    fReadState = false;
    fSocket = socket;
    fErrorCode = 0;
    if (fSocket == 0) {
        fSocket = new QTcpSocket(parent);
        connect(fSocket, SIGNAL(connected()), this, SLOT(connected()));
        fSocket->connectToHost(__socketServerHost, __socketServerPort);
        fSocket->waitForConnected(5000);
    }
    connect(fSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

C5SocketHandler::~C5SocketHandler()
{

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
    QJsonObject jObj;
    for (QMap<QString, QVariant>::const_iterator it = fBindValues.begin(); it != fBindValues.end(); it++) {
        switch (it.value().type()) {
        case QVariant::Int:
            jObj[it.key()] = it.value().toInt();
            break;
        default:
            jObj[it.key()] = it.value().toString();
            break;
        }
    }
    QJsonDocument jDoc(jObj);
    QByteArray data = jDoc.toJson();
    int docSize = data.size();
    fSocket->write(reinterpret_cast<const char*>(&docSize), sizeof(quint32));
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
        switch (it.value().type()) {
        case QVariant::Int:
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
    fSocket->write(reinterpret_cast<const char*>(&docSize), sizeof(quint32));
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
        fSocket->read(reinterpret_cast<char*>(&fDataSize), sizeof(quint32));
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
        }
        fData.clear();
        fDataSize = 0;
        fReadState = false;
    }
    if (fDataSize < fData.size()) {
        qDebug() << "PIZDA";
    }
}
