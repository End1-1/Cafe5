#ifndef SOCKETHANDLER_H
#define SOCKETHANDLER_H

#include <QByteArray>

class SocketHandler
{
public:
    explicit SocketHandler(QByteArray &data);
    virtual void processData() = 0;
    virtual bool closeConnection();
    qint32 fResponseCode;

    template<typename T>
    static T *create(QByteArray &data) {
        T *t = new T(data);
        return t;
    }

protected:
    QByteArray &fData;
};

#endif // SOCKETHANDLER_H
