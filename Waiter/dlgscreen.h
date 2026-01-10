#ifndef DLGSCREEN_H
#define DLGSCREEN_H

#include "c5waiterdialog.h"

namespace Ui
{
class DlgScreen;
}

class C5WaiterServer;

class DlgScreen : public C5WaiterDialog
{
    Q_OBJECT

public:
    explicit DlgScreen(C5User *user);

    ~DlgScreen();

protected:
    virtual bool eventFilter(QObject *o, QEvent *e) override;

    virtual void paintEvent(QPaintEvent *e) override;

private slots:
    void on_btnCancel_clicked();

    void on_btn1_clicked();

    void on_btn2_clicked();

    void on_btn3_clicked();

    void on_btn4_clicked();

    void on_btn5_clicked();

    void on_btn6_clicked();

    void on_btn7_clicked();

    void on_btn8_clicked();

    void on_btn9_clicked();

    void on_btn0_clicked();

    void on_btnAccept_clicked();

    void on_lePassword_returnPressed();

    void on_btnSettings_clicked();

    void on_btnClose_clicked();

private:
    Ui::DlgScreen* ui;

    C5WaiterServer* mWaiterServer;

    void tryExit();

    void updatePin();

    QString mPin;
};

#endif // DLGSCREEN_H
