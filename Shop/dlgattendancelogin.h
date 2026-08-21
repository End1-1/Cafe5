#ifndef DLGATTENDANCELOGIN_H
#define DLGATTENDANCELOGIN_H

#include "c5dialog.h"
#include <functional>

namespace Ui
{
class DlgAttendanceLogin;
}

class DlgAttendanceLogin : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgAttendanceLogin(C5User *user, QWidget *parent = nullptr);
    ~DlgAttendanceLogin() override;

    /** Show dialog; returns true if checkin/checkout completed. */
    static bool run(C5User *user, QWidget *parent = nullptr);

private slots:
    void on_btnOK_clicked();
    void on_btnCancel_clicked();

private:
    Ui::DlgAttendanceLogin *ui;
    QString mSession;

    void toggleAttendance(int userId, bool currentlyActive, const QString &userName,
                          const std::function<void(bool completed)> &done);
};

#endif // DLGATTENDANCELOGIN_H
