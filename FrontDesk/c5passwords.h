#ifndef C5PASSWORDS_H
#define C5PASSWORDS_H

#include "c5dialog.h"

namespace Ui {
class C5Passwords;
}

class C5Passwords : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5Passwords(const QStringList &dbParams, QWidget *parent = 0);

    ~C5Passwords();

    static void setPasswords(const QStringList &dbParams, int userId);

private slots:
    void on_btnOK_clicked();

    void on_btnCancel_clicked();

    void on_chFrontPassword_clicked(bool checked);

    void on_chWaiterPassword_clicked(bool checked);

private:
    Ui::C5Passwords *ui;


    int fUserId;

    void setUser(int userId);
};

#endif // C5PASSWORDS_H
