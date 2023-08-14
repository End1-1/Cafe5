#ifndef C5CHANGEPASSWORD_H
#define C5CHANGEPASSWORD_H

#include "c5dialog.h"

namespace Ui {
class C5ChangePassword;
}

class C5ChangePassword : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5ChangePassword(const QStringList &dbParams);

    ~C5ChangePassword();

    static bool changePassword(const QStringList &dbParams, QString &password);

private slots:
    void on_btnCancel_clicked();

    void on_btnOK_clicked();

private:
    Ui::C5ChangePassword *ui;
};

#endif // C5CHANGEPASSWORD_H
