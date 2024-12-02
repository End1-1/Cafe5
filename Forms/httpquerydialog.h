#ifndef HTTPQUERYDIALOG_H
#define HTTPQUERYDIALOG_H

#include "c5dialog.h"
#include <QJsonObject>

namespace Ui
{
class HttpQueryDialog;
}

class HttpQueryDialog : public C5Dialog
{
    Q_OBJECT

public:
    explicit HttpQueryDialog(const QStringList &dbParams, const QString &url, const QJsonObject &jo,
                             QWidget *parent = nullptr);
    ~HttpQueryDialog();
    virtual int exec() override;

private:
    Ui::HttpQueryDialog *ui;

    void sendRequest();
    QString fUrl;
    QJsonObject jRequest;
    void connectError(QAbstractSocket::SocketError error);

signals:
    void messageReceived();
    void exitLoop();
};

#endif // HTTPQUERYDIALOG_H
