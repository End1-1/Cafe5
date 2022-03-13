#ifndef RAW_H
#define RAW_H

#include "rawmessage.h"
#include "structs.h"

class SslSocket;
class QMutex;

class Raw : public RawMessage
{
    Q_OBJECT
public:
    explicit Raw(SslSocket *s);
    ~Raw();
    static void init();
    static void removeSocket(SslSocket *s);

public slots:
    virtual void run(const QByteArray &d) = 0;

protected:
    static QMutex *fMutexTokenUser;
    static QMutex *fMutexInformMonitors;
    static QHash<QString, int> fMapTokenUser;
    static QHash<QString, ConnectionStatus> fMapTokenConnectionStatus;
    static QHash<SslSocket*, QString> fMapSocketToken;
    static QHash<QString, SslSocket*> fMapTokenSocket;
    static QList<SslSocket*> fMonitors;
    static void informMonitors(const QByteArray &d);
    static const QString &tokenOfSocket(SslSocket *s);
};

#endif // RAW_H
