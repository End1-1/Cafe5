#ifndef TESTN2_H
#define TESTN2_H

#include "testn.h"
class TestN2 : public TestN
{
    Q_OBJECT
public:
    explicit TestN2(const QString &serverIP, int port, const QSslCertificate &certificate, int number, const QVariant &data = QVariant());
    ~TestN2();

public slots:
    virtual void run() override;

private:
    SslSocket *fSslSocket;

private slots:
    void timeout();
};

#endif // TESTN2_H
