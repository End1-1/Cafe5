#ifndef C5SERVERPRINTER_H
#define C5SERVERPRINTER_H

#include <QObject>
#include <QJsonObject>
#include "ninterface.h"

class C5ServerPrinter : public QObject
{
    Q_OBJECT
public:
    explicit C5ServerPrinter(const QJsonObject &j, QObject *parent = nullptr);

    ~C5ServerPrinter();

public slots:
    void run();

private:
    NInterface *fHttp;
    QJsonObject jo;
    void handleChat();

private slots:
    void completeResponse(const QJsonObject &jdoc);
    void httpError(const QString &err);

signals:
    void finished();

    void showNotificationWidget(const QJsonObject &);
};

#endif // C5SERVERPRINTER_H
