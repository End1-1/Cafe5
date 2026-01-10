#ifndef DLGEXITWITHMESSAGE_H
#define DLGEXITWITHMESSAGE_H

#include "c5dialog.h"
#include <QTimer>

namespace Ui {
class DlgExitWithMessage;
}

class DlgExitWithMessage : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgExitWithMessage(C5User *user);

    ~DlgExitWithMessage();

    static void openDialog(C5User *user, const QString &msg, int closeTimeout = 10);

private slots:
    void timeout();

    void on_btnClose_clicked();

private:
    Ui::DlgExitWithMessage *ui;

    int fCloseTimeout;

    QTimer fTimer;

    void setCloseTimeout(int closeTimeout);
};

#endif // DLGEXITWITHMESSAGE_H
