#ifndef C5PRINTJSON_H
#define C5PRINTJSON_H

#include <QThread>
#include <QJsonArray>

class C5PrintJson : public QThread
{
    Q_OBJECT

public:
    C5PrintJson(const QJsonArray &obj, QObject *parent = 0);

    ~C5PrintJson();

protected:
    virtual void run();

private:
    QJsonArray fJson;
};

#endif // C5PRINTJSON_H
