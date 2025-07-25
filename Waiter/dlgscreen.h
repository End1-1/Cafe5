#ifndef DLGSCREEN_H
#define DLGSCREEN_H

#include "c5dialog.h"

namespace Ui
{
class DlgScreen;
}

class C5WaiterServer;

class DlgScreen : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgScreen();

    ~DlgScreen();

private slots:
    void initResponse(const QJsonObject &jdoc);

    void serviceTimeout();

    void loginResponse(const QJsonObject &jdoc);

    void timerTimeout();

    void on_btnClear_clicked();

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

    void on_btnP_clicked();

    void on_btnAccept_clicked();

    void on_lePassword_returnPressed();

    void on_btnSettings_clicked();

private:
    Ui::DlgScreen* ui;

    C5WaiterServer* mWaiterServer;

    void tryExit();
};

#endif // DLGSCREEN_H
