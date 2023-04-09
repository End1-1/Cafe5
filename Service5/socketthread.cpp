#include "socketthread.h"
#include "requesthandler.h"
#include "requestmanager.h"
#include "logwriter.h"
#include "rawhandler.h"
#include "pluginmanager.h"
#include "raw.h"
#include <QHostAddress>
#include <QApplication>
#include <QUuid>

SocketThread::SocketThread(int handle, const QSslCertificate &cert, const QSslKey &key, QSsl::SslProtocol proto) :
    QObject(),
    fSocketDescriptor(handle),
    fSslLocalCertificate(cert),
    fSslPrivateKey(key),
    fSslProtocol(proto)
{
    fMessageNumber = 0;
    fPreviouseMessageNumber = 0;
    fDoNotRemovePluginSocket = false;
    connect(this, &SocketThread::finished, this, &SocketThread::deleteLater);
}

SocketThread::~SocketThread()
{
    if (fDoNotRemovePluginSocket) {
        PluginManager::fInstance->removeSocket(fSslSocket);
    } else {
        Raw::removeSocket(fSslSocket);
        fSslSocket->disconnectFromHost();
        delete fSslSocket;
    }
    delete fTimeoutControl;
    delete fRawHandler;
}

void SocketThread::run()
{
    fSocketType = Invalid;
    setProperty("session", QUuid::createUuid().toString());
    fTimeoutControl = new QTimer();
    connect(fTimeoutControl, &QTimer::timeout, this, &SocketThread::timeoutControl);
    fTimeoutControl->start(5000);

    fSslSocket = new SslSocket();
    fSslSocket->setProperty("session", property("session"));
    qDebug() << "set socket description" << fSslSocket->setSocketDescriptor(fSocketDescriptor);
    fSslSocket->setLocalCertificate(fSslLocalCertificate);
    fSslSocket->setPeerVerifyMode(QSslSocket::VerifyNone);
    fSslSocket->setPrivateKey(fSslPrivateKey);
    fSslSocket->setProtocol(fSslProtocol);
    fSslSocket->startServerEncryption();
    qDebug() << fSslSocket->isEncrypted() << fSslSocket->errorString();
    if (!fSslSocket->waitForEncrypted()) {
        qDebug() <<fSslSocket->errorString();
    }
    fRawHandler = new RawHandler(fSslSocket, property("session").toString());
    connect(fRawHandler, &RawHandler::writeToSocket, this, &SocketThread::writeToSocket);
    fTimer.start();
    fContentLenght = 0;
    qRegisterMetaType <QAbstractSocket::SocketError> ("QAbstractSocket::SocketError");
    connect(fSslSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(fSslSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));
    connect(fSslSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    LogWriter::write(LogWriterLevel::verbose, property("session").toString(), QString("New connection from %1:%2").arg(QHostAddress(fSslSocket->peerAddress().toIPv4Address()).toString()).arg(fSslSocket->peerPort()));
}

void SocketThread::rawRequest()
{
    int headersize = 3 + sizeof(fMessageNumber) + sizeof(fMessageId) + sizeof(fMessageListData) + sizeof(fContentLenght);
    if (fData.length() >= headersize) {
        int pos = 3;
        memcpy(&fMessageNumber, &fData.data()[pos], sizeof(fMessageNumber));
        pos += sizeof(fMessageNumber);
        memcpy(&fMessageId, &fData.data()[pos], sizeof(fMessageId));
        pos += sizeof(fMessageId);
        memcpy(&fMessageListData, &fData.data()[pos], sizeof(fMessageListData));
        pos += sizeof(fMessageListData);
        memcpy(&fContentLenght, &fData.data()[pos], sizeof(fContentLenght));
        //pos += sizeof(fContentLenght);
    } else {
        fTimeoutControl->stop();
        fTimeoutControl->start(3000);
        return;
    }
    if (fPreviouseMessageNumber + 1 != fMessageNumber) {
        LogWriter::write(LogWriterLevel::errors, property("session").toString(), QString("Packet message number error, previous: %1, current: %2").arg(fPreviouseMessageNumber).arg(fMessageNumber));
//        fSslSocket->close();
//        return;
    }
    fPreviouseMessageNumber = fMessageNumber;
    if (fData.length() - headersize >= fContentLenght) {
        parseBody(fMessageListData, fData.mid(headersize, fContentLenght));
        fData.remove(0, headersize + fContentLenght);
        if (fData.length() > 0) {
            rawRequest();
        }
    }
}

void SocketThread::httpRequest()
{
    qDebug() << fData;
    if (fContentLenght > 0) {
        if (fData.length() < fContentLenght + fHeaderLength) {
            return;
        }
    } else {
        if (QString(fData.mid(0, 4).toUpper()) == "POST") {
            if (!fData.contains("Content-Length") && !fData.contains("content-length")) {
                return;
            }
        } else if (QString(fData.mid(0, 3).toUpper()) == "GET") {
            if ((QString(fData.mid(fData.length() - 4, 4)) != "\r\n\r\n") && (QString(fData.mid(fData.length() - 4, 4)) != "\n\n\n\n")){
                return;
            }
            if (QString(fData.mid(fData.length() - 4, 4)) == "\n\n\n\n") {
                fData.replace(fData.length() - 4, "\r\n\r\n");
            }
        }
    }
    fTimeoutControl->stop();
    HttpRequestMethod m;
    QString route;
    QString httpVersion;
    switch (parseRequest(m, httpVersion, route)) {
    case GET:
        break;
    case POST:
        break;
    case UNSUPPORTED:
        fSslSocket->write(QString("HTTP/1.1 405 Method not allowed\r\n\r\n<html><H1>HTTP %1 Method not allowed</H1></html>").arg(fMethodString).toUtf8());
        fSslSocket->close();
        break;
    case UNKNOWN_REQUEST_METHOD:
        fSslSocket->write(QString("HTTP/1.1 501 Not Implemented\r\n\r\n<html><H1>Not Implemented</H1></html>").toUtf8());
        fSslSocket->close();
        break;
    case INCOMPLETE:
        LogWriter::write(LogWriterLevel::errors, property("session").toString(), QString("Error in request. %1").arg(QString(fData)));
        return;
    }
    LogWriter::write(LogWriterLevel::verbose, property("session").toString(), route);
    QByteArray response;
    RequestManager::handle(property("session").toString(),
                           QHostAddress(fSslSocket->peerAddress().toIPv4Address()).toString(),
                           route.remove(0,1),
                           fData,
                           response,
                           fRequestBody,
                           fContentType);
    fSslSocket->write(response);
    LogWriter::write(LogWriterLevel::verbose, property("session").toString(), response);
    fSslSocket->close();
}

HttpRequestMethod SocketThread::parseRequest(HttpRequestMethod &requestMethod, QString &httpVersion, QString &route)
{
    requestMethod = UNKNOWN_REQUEST_METHOD;
    QStringList methodsPatters;
    methodsPatters.append("GET");
    methodsPatters.append("POST");
    for (const QString &m: methodsPatters) {
        QString cmp = fData.mid(0, m.length()).toUpper();
        if (cmp == m) {
            fMethodString = cmp;
            if (cmp == "GET") {
                requestMethod = GET;
            } else if (cmp == "POST") {
                requestMethod = POST;
            } else {
                requestMethod = UNSUPPORTED;
            }
        }
    }
    if (requestMethod != UNSUPPORTED && requestMethod != UNKNOWN_REQUEST_METHOD) {
        int s1 = fData.indexOf(' ', 0);
        int s2 = 0;
        int s3 = 0;
        if (s1 == -1) {
            requestMethod = UNKNOWN_REQUEST_METHOD;
        } else {
            s2 = fData.indexOf(' ', s1 + 1);
            if (s2 == -1) {
                requestMethod = UNKNOWN_REQUEST_METHOD;
            } else {
                route = fData.mid(s1 + 1, s2 - s1 - 1);
            }
        }
        s1 = s2 + 1;
        s2 = fData.indexOf("\r\n", s1);
        if (s2 == -1) {
            return UNKNOWN_REQUEST_METHOD;
        } else {
            httpVersion = fData.mid(s1, s2 - s1);
        }
        bool headerEnd = false;
        while (!headerEnd) {
            s1 = s2 + 1;
            if (fData[s1] == '\r') {
                break;
            }
            s2 = fData.indexOf("\r\n", s1);
            if (s2 == -1) {
                break;
            }
            s3 = fData.indexOf(':', s1);
            if (s3 == -1) {
                break;
            }
            fHttpHeader.insert(fData.mid(s1 + 1, s3 - s1 - 1).toLower(), DataAddress(s3 + 2, s2 - 1, s2 - s3));
            if (fData.at(s2 + 2) == '\r') {
                headerEnd = true;
                fHeaderLength = s2 + 4;
            }
        }
        if (fContentLenght == 0 && requestMethod == POST) {
            fContentLenght = header("content-length").toInt();
            if (fData.length() < fContentLenght + fHeaderLength) {
                return INCOMPLETE;
            }
        }
        bool bodyEnd = false;
        s1 = 0;
        s2 = 0;
        s3 = 0;
        if (requestMethod == GET) {
            s1 = route.indexOf("?", 0);
            if (s1 > -1) {
                parseBody(route.mid(s1 + 1, route.length() - s1), s1 + 5);
                route = route.mid(0, s1);
            }
            fContentType = ContentType::TextHtml;
        } else if (requestMethod == POST) {
            if (!fHttpHeader.contains("content-type")) {
                return UNKNOWN_REQUEST_METHOD;
            }
            QString contentType = header("content-type");
            if (contentType.contains("multipart/form-data", Qt::CaseInsensitive)) {
                fContentType = ContentType::MultipartFormData;
                s1 = fData.indexOf("boundary=");
                if (s1 == -1) {
                    return UNKNOWN_REQUEST_METHOD;
                }
                s1 += QString("boundary=").length();
                s2 = fData.indexOf('\r', s1 + 1);
                if (s2 == -1) {
                    return UNKNOWN_REQUEST_METHOD;
                }
                fBoundary = "--" + fData.mid(s1, s2 - s1);
                s1 = 0;
                s3 = 0;
                int s4 = s2;
                int boundaryLength = fBoundary.length();
                do {
                    s1 = fData.indexOf(fBoundary, s4) ;
                    if (s1 == -1) {
                        return UNKNOWN_REQUEST_METHOD;
                    }
                    s1 += boundaryLength;
                    s2 = fData.indexOf("\"", s1);
                    if (s2 == -1) {
                        break;
                    }
                    s3 = fData.indexOf("\"", s2 + 1);
                    int s5 = fData.indexOf("\r\n\r\n", s3);
                    s4 = fData.indexOf(fBoundary, s5);
                    fRequestBody.insert(fData.mid(s2 + 1, s3 - s2 - 1), DataAddress(s5 + 4, s4 - 1, s4 - s5 - 6));
                    s4 = s3;
                    if (fData.indexOf(fBoundary, s5) == -1) {
                        bodyEnd = true;
                    }
                } while (!bodyEnd);
            } else if (contentType.contains("application/x-www-form-urlencoded", Qt::CaseInsensitive)) {
                fContentType = ContentType::UrlEncoded;
                s1 = fData.indexOf("\r\n\r\n");
                if (s1 == -1) {
                    return UNKNOWN_REQUEST_METHOD;
                }
                s1 += 4;
                parseBody(fData.mid(s1, fData.length() - s1), s1);
            } else if (contentType.contains("application/json", Qt::CaseInsensitive)) {
                fContentType = ContentType::ApplilcationJson;
                s1 = fData.indexOf("\r\n\r\n");
                if (s1 == -1) {
                    return UNKNOWN_REQUEST_METHOD;
                }
                fRequestBody.insert("json", DataAddress(s1, fData.length() - 1, fData.length() - s1));
            } else {
                fContentType = ContentType::TextHtml;
            }
        }
    }
    return requestMethod;
}

void SocketThread::parseBody(quint16 msgType, const QByteArray &data)
{
    switch (fRawHandler->run(fPreviouseMessageNumber, fMessageId, msgType, data)) {
    case 1:
        fTimeoutControl->stop();
        break;
    case 2:
        fTimeoutControl->stop();
        fTimeoutControl->start(60000);
        break;
    case 3:
        fDoNotRemovePluginSocket = true;
        break;
    default:
        break;
    }
}

void SocketThread::parseBody(const QString &request, int offset)
{
    int s1 = 0;
    int s2 = 0;
    int s3 = 0;
    int s4 = 0;
    bool bodyEnd = false;
    do {
        s1 = s3;
        s2 = request.indexOf("=", s1 + 1);
        s3 = request.indexOf("&", s2 + 1);
        if (s3 == -1) {
            s3 = request.length();
            bodyEnd = true;
        }
        if (s1 != -1 && s2 != -1) {
            fRequestBody.insert(request.mid(s1 + s4, s2 - s1 - s4).toLower(), DataAddress(s2 + 1 + offset, s3 + offset - 1, s3 - s2 - 1));
        } else {
            bodyEnd = true;
        }
        s4 = 1;
    } while (!bodyEnd);
}

QString SocketThread::header(const QString &name) const
{
    const DataAddress &da = fHttpHeader[name];
    return data(da);
}

QString SocketThread::request(const QString &name) const
{
    const DataAddress &da = fRequestBody[name];
    return data(da);
}

QString SocketThread::data(const DataAddress &da) const
{
    QString result = fData.mid(da.start, da.length);
    return result;
}

void SocketThread::timeoutControl()
{
    fSslSocket->close();
    LogWriter::write(LogWriterLevel::errors, property("session").toString(), tr("Connection timeout"));
}

void SocketThread::readyRead()
{
    /* Raw data in socket pattern, otherwise means http request */
    QByteArray ba;
    ba.append(0x03);
    ba.append(0x04);
    ba.append(0x15);

    if (fData.isEmpty()) {
        fData = fSslSocket->read(3);
        if (fData.compare(ba) == 0) {
            fSocketType = RawData;
        } else {
            fSocketType = HttpRequest;
        }
    }
    fData.append(fSslSocket->readAll());
    switch (fSocketType) {
    case RawData:
        rawRequest();
        break;
    case HttpRequest:
        httpRequest();
        break;
    }
}

void SocketThread::writeToSocket(const QByteArray &d)
{
    quint64 bw = fSslSocket->write(d);
    if (bw != d.length()) {
        LogWriter::write(LogWriterLevel::warning,
                         property("session").toString(),
                         QString("Socket write incomplete %1 of %2").arg(bw).arg(d.length()));
    }
}

void SocketThread::disconnected()
{
    emit finished();
}

void SocketThread::error(QAbstractSocket::SocketError err)
{
    Q_UNUSED(err);
    fSslSocket->disconnect();
    emit finished();
}
