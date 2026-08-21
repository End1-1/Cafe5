#include "dlgattendancelogin.h"
#include "ui_dlgattendancelogin.h"
#include "c5message.h"
#include "c5user.h"
#include "ninterface.h"

DlgAttendanceLogin::DlgAttendanceLogin(C5User *user, QWidget *parent)
    : C5Dialog(user, parent)
    , ui(new Ui::DlgAttendanceLogin)
    , mSession(user->mSessionKey)
{
    ui->setupUi(this);
    setWindowTitle(tr("Attendance"));
    ui->lbHint->setText(tr("Enter login and password"));
    ui->leLogin->setFocus();
}

DlgAttendanceLogin::~DlgAttendanceLogin()
{
    delete ui;
}

bool DlgAttendanceLogin::run(C5User *user, QWidget *parent)
{
    DlgAttendanceLogin dlg(user, parent);
    return dlg.exec() == QDialog::Accepted;
}

void DlgAttendanceLogin::toggleAttendance(int userId, bool currentlyActive, const QString &userName,
                                          const std::function<void(bool completed)> &done)
{
    const QString prompt = currentlyActive
                               ? tr("Confirm checkout for %1").arg(userName)
                               : tr("Confirm checkin for %1").arg(userName);
    if(C5Message::question(prompt) != QDialog::Accepted) {
        if(done) {
            done(false);
        }
        return;
    }

    NInterface::query1(QStringLiteral("/engine/v2/common/attendance/toggle-for-user"),
                       mSession,
                       this,
                       {{QStringLiteral("user_id"), userId}},
                       [done](const QJsonObject &jo) {
                           const QString action = jo.value(QStringLiteral("action")).toString();
                           if(action == QStringLiteral("checkout")) {
                               C5Message::info(tr("Checkout completed"));
                           } else {
                               C5Message::info(tr("Checkin completed"));
                           }
                           if(done) {
                               done(true);
                           }
                       });
}

void DlgAttendanceLogin::on_btnOK_clicked()
{
    const QString login = ui->leLogin->text().trimmed();
    const QString password = ui->lePassword->text();
    if(login.isEmpty() || password.isEmpty()) {
        C5Message::error(tr("Enter login and password"));
        return;
    }

    NInterface::query1(QStringLiteral("/engine/v2/common/attendance/verify-login"),
                       mSession,
                       this,
                       {{QStringLiteral("login"), login},
                        {QStringLiteral("password"), password}},
                       [this](const QJsonObject &jo) {
                           toggleAttendance(jo.value(QStringLiteral("user_id")).toInt(),
                                            jo.value(QStringLiteral("active")).toBool(),
                                            jo.value(QStringLiteral("f_name")).toString(),
                                            [this](bool completed) {
                                                if(completed) {
                                                    accept();
                                                }
                                            });
                       });
}

void DlgAttendanceLogin::on_btnCancel_clicked()
{
    reject();
}
