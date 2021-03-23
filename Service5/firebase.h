#ifndef FIREBASE_H
#define FIREBASE_H

#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;

class Firebase : public QObject
{
    Q_OBJECT
public:
    explicit Firebase(QObject *parent = nullptr);
    void sendMessage();

private:
    QNetworkAccessManager *fNetworkAccessManager;

private slots:
    void finished(QNetworkReply *reply);

signals:

};

#endif // FIREBASE_H
