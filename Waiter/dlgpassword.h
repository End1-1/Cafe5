#ifndef DLGPASSWORD_H
#define DLGPASSWORD_H

#include "c5dialog.h"

namespace Ui {
class DlgPassword;
}

class C5User;

class DlgPassword : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgPassword(C5User *u);

    explicit DlgPassword(int i);

    ~DlgPassword();

    static bool getUser(const QString &title, C5User *&user);

    static bool getUserAndCheck(const QString &title, C5User *&user, int permission);

    static bool getQty(const QString &title, int &qty);

    static bool getAmount(const QString &title, double &amount, bool defaultAmount = false);

    static bool getString(const QString &title, QString &str);

    static bool getPassword(const QString &title, QString &str);

    static bool getPasswordString(const QString &title, QString &pass);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_10_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_12_clicked();

    void on_lePassword_returnPressed();

    void on_lePassword_textChanged(const QString &arg1);

    void on_btnClear_clicked();

    void on_pushButton_13_clicked();

private:
    Ui::DlgPassword *ui;

    void click(const QString &text);

    double fMax;

    C5User *fUser;
};

#endif // DLGPASSWORD_H
