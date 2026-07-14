#ifndef WSESSION_H
#define WSESSION_H

#include <QJsonObject>
#include <QWidget>

namespace Ui
{
class WSession;
}

class C5User;

class WSession : public QWidget
{
    Q_OBJECT

public:
    explicit WSession(C5User *user, QWidget *parent = nullptr);

    ~WSession();

signals:
    void sessionOpened(const QJsonObject &session);

protected:
    void showEvent(QShowEvent *e) override;

private slots:
    void on_btnOpenCashbox_clicked();

private:
    Ui::WSession *ui;

    C5User *mUser;

    QJsonObject mCashboxData;

    bool mFinished = false;

    bool mOpening = false;

    void checkStatus();

    void setup();

    void finishSessionOpen();
};

#endif // WSESSION_H
