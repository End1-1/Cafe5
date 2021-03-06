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
    explicit DlgPassword();

    ~DlgPassword();

    static bool getUser(const QString &title, C5User *user);

    static bool getUserDB(const QString &title, C5User *user);

    static bool getQty(const QString &title, int &qty);

    static bool getAmount(const QString &title, double &amount, bool defaultAmount = false);

private slots:
    void handlePassword(const QJsonObject &obj);

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

private:
    Ui::DlgPassword *ui;

    void click(const QString &text);

    C5User *fUser;

    double fMax;

    bool fAutoFromDb;
};

#endif // DLGPASSWORD_H
