#pragma once

#include "c5dialog.h"

#include <QJsonArray>
#include <functional>

class QTimer;
class ZkfingerprintReader;

namespace Ui {
class DlgAttendanceAuth;
}

class DlgAttendanceAuth : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgAttendanceAuth(C5User *workstationUser, QWidget *parent = nullptr);
    ~DlgAttendanceAuth() override;

protected:
    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void appendDigit();
    void on_btnCancel_clicked();
    void on_btnAccept_clicked();
    void on_btnFingerPrint_clicked();
    void onFingerprintProbe(const QByteArray &probe);

private:
    void updatePin();
    void loadFingerprints();
    void startFingerprintScan();
    void stopFingerprintScan();
    int matchFingerprintUser(const QByteArray &probe) const;
    void proceedAttendance(int userId, const std::function<void(bool completed)> &done);
    void toggleAttendance(int userId, bool currentlyActive, const QString &userName, const std::function<void(bool completed)> &done);
    void showFingerPrintStatus(const QString &text, const QColor &color, int hideMs = 0);
    void clearFingerPrintStatus();

    Ui::DlgAttendanceAuth *ui;
    ZkfingerprintReader *mReader = nullptr;
    QJsonArray mFingerprintItems;
    bool mFingerprintBusy = false;
    QString mWorkstationSession;
    QString mPin;
    QTimer *mFingerPrintStatusTimer = nullptr;
};
