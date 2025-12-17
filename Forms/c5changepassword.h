#ifndef C5CHANGEPASSWORD_H
#define C5CHANGEPASSWORD_H

#include "c5officedialog.h"

namespace Ui
{
class C5ChangePassword;
}

class C5ChangePassword : public C5OfficeDialog
{
    Q_OBJECT

public:
    explicit C5ChangePassword();

    ~C5ChangePassword();

    static bool changePassword(QString &password);

private slots:
    void on_btnCancel_clicked();

    void on_btnOK_clicked();

private:
    Ui::C5ChangePassword* ui;
};

#endif // C5CHANGEPASSWORD_H
