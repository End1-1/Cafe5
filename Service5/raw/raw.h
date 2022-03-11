#ifndef RAW_H
#define RAW_H

#include "rawmessage.h"

class SslSocket;

class Raw : public RawMessage
{
    Q_OBJECT
public:
    explicit Raw(SslSocket *s, const QByteArray &d);

public slots:
    virtual void run() = 0;    

protected slots:
    virtual void receiveReply(SslSocket *s, const QByteArray &d);

signals:
    void finish(QByteArray);
};

#endif // RAW_H
