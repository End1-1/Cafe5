#ifndef FIREBASE_H
#define FIREBASE_H

#include <QObject>
#include <QNetworkReply>

class QNetworkAccessManager;
class QNetworkReply;

class Firebase : public QObject
{
    Q_OBJECT
public:
    explicit Firebase(QObject *parent = nullptr);
    ~Firebase();
    static Firebase *instance();

public slots:
    void sendMessage(const QString &token, const QString &msg);

private:
    QNetworkAccessManager *fNetworkAccessManager;
    static Firebase *fInstance;

private slots:
    void finished(QNetworkReply *reply);

signals:
    void sendFinished();
};

#endif // FIREBASE_H
