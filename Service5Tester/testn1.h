#ifndef TESTN1_H
#define TESTN1_H

#include "testn.h"

class TestN1 : public TestN
{
    Q_OBJECT
public:
    explicit TestN1(const QString &serverIP, int port, const QSslCertificate &certificate, int number);
    ~TestN1();

public slots:
    virtual void run() override;

};

#endif // TESTN1_H
