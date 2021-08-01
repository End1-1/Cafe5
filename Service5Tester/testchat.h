#ifndef TESTCHAT_H
#define TESTCHAT_H

#include "testn.h"

class TestChat : public TestN
{
public:
    TestChat(const QString &serverIP, int port, const QSslCertificate &certificate, int number, int timeout, const QVariant &data = QVariant());

protected:
    virtual void run() override;

signals:
    void finished();
};

#endif // TESTCHAT_H
