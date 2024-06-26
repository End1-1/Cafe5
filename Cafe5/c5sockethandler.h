#ifndef C5SOCKETHANDLER_H
#define C5SOCKETHANDLER_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>

#define SR_NO_ERROR 0
#define SR_ERROR 1

class C5SocketHandler : public QObject
{
    Q_OBJECT
public:
    explicit C5SocketHandler(QTcpSocket *socket, QObject *parent = nullptr);

    ~C5SocketHandler();

    static void setServerAddress(const QString &serverIP);

    QVariant &operator[] (const QString &key);

    void bind(const QString &key, const QVariant &value);

    void send();

    void send(int errorCode);

    void send(QJsonObject &obj);

    void close();

    QTcpSocket *fSocket;

private:
    bool fReadState;

    int fDataSize;

    QByteArray fData;

    int fErrorCode;

    QMap<QString, QVariant> fBindValues;

private slots:
    void connected();

    void readyRead();

    void error(QAbstractSocket::SocketError err);

signals:
    void handleCommand(const QJsonObject &data);

    void handleError(int cmd, const QString &message);
};

extern QString __socketServerHost;
extern quint16 __socketServerPort;

#endif // C5SOCKETHANDLER_H
