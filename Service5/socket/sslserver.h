#ifndef QSSLSERVER_H
#define QSSLSERVER_H

#include <QTcpServer>
#include <QSslCertificate>
#include <QSslKey>

class SslServer : public QTcpServer
{
    Q_OBJECT

public:
    SslServer(QObject *parent = nullptr);
    void setSslLocalCertificate(const QSslCertificate &s);
    bool setSslLocalCertificate(const QString &path, QSsl::EncodingFormat format = QSsl::Pem);
    void setSslPrivateKey(const QSslKey &k);
    bool setSslPrivateKey(const QString &path, QSsl::KeyAlgorithm algorithm = QSsl::Rsa, QSsl::EncodingFormat format = QSsl::Pem, const QByteArray &passPhrase = QByteArray());
    void setSslProtocol(QSsl::SslProtocol p);

    QSslCertificate fSslLocalCertificate;
    QSslKey fSslPrivateKey;
    QSsl::SslProtocol fSslProtocol;

public slots:
    void startListen();

protected:
    virtual void incomingConnection(qintptr handle) override final;

private:


private slots:
    //void acceptError(QAbstractSocket::SocketError err);

signals:
    void connectionRequest(int);
};

#endif // QSSLSERVER_H
