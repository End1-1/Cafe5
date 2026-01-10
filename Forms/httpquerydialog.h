#ifndef HTTPQUERYDIALOG_H
#define HTTPQUERYDIALOG_H

#include "c5dialog.h"
#include <QJsonObject>
#include <QTcpSocket>
#include <QTimer>

namespace Ui
{
class HttpQueryDialog;
}

class HttpQueryDialog : public C5Dialog
{
    Q_OBJECT

public:
    explicit HttpQueryDialog(C5User *user, const QString &url, const QJsonObject &jo,
                             QWidget *parent = nullptr);
    ~HttpQueryDialog();
    virtual int exec() override;

private:
    Ui::HttpQueryDialog* ui;

    void sendRequest();
    QString fUrl;
    QJsonObject jRequest;
    void connectError(QAbstractSocket::SocketError error);
    QTimer fTimer;

signals:
    void messageReceived();
    void exitLoop();
};

#endif // HTTPQUERYDIALOG_H
