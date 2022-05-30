#include "idram.h"
#include <QSslSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

Idram::Idram(QObject *parent) : QObject(parent)
{

}

bool Idram::check(const QString &server, const QString &sessionid, const QString &orderid, double &amount, QByteArray &out)
{
    QString request = QString("{\"Detail\":\"%1\"}").arg(orderid);
    QString str;
    QString host, route, f = server;
    int hoststart = f.indexOf("//") + 2;
    int routestart = f.indexOf("/", hoststart + 1);
    host = f.midRef(hoststart, routestart - hoststart).toString();
    route = f.midRef(routestart, f.length() - routestart).toString();
    str.append(QString("POST %1 HTTP/1.1\r\n").arg(route));
    str.append(QString("Host: %1\r\n").arg(host));
    str.append(QString("Content-Type: application/json;charset=UTF-8\r\n"));
    str.append(QString("Content-Length: %1\r\n").arg(request.toLatin1().length()));
    str.append(QString("_SessionId_: %1\r\n").arg(sessionid));
    str.append(QString("_EncMethod_: NONE\r\n"));
    str.append(QString("User-Agent: Godzilla\r\n"));
    str.append(QString("Accept:*/*\r\n"));
    str.append("\r\n");
    str.append(request);
    QSslSocket s;
    s.connectToHostEncrypted(host, 443);
    if (!s.waitForEncrypted(10000)) {
        out = s.errorString().toLatin1();
        return false;
    }
    s.waitForEncrypted();
    s.write(str.toLatin1());
    s.waitForBytesWritten();
    if (!s.waitForReadyRead(10000)) {
        out = s.errorString().toLatin1();
        return false;
    }
    out = s.readAll();
    s.close();
    int datastart = out.indexOf("\r\n\r\n", 0) + 4;
    int datasize = out.length() - datastart;
    if (out.contains("chunked")) {
        int chunkend = out.indexOf("\r\n", datastart);
        int dataend = out.indexOf("0\r\n", chunkend + 2) - 2;
        datastart = chunkend + 2;
        datasize = dataend - datastart;//QString(replyData.mid(datastart, chunkend - datastart)).toInt(nullptr, 16);
    }
    qDebug() << out.mid(datastart, datasize);
    QJsonDocument jdoc = QJsonDocument::fromJson(out.mid(datastart, datasize));
    QJsonObject jo = jdoc.object();
    QJsonArray ja = jo["Result"].toArray();
    if (ja.count() > 0) {
        QString DEBIT = ja.at(0).toObject()["DEBIT"].toString();
        DEBIT.replace(",", "");
        amount = DEBIT.toDouble();
        return true;
    }
    amount = -1;
    return false;
}
