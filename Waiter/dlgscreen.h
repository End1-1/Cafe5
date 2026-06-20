#ifndef DLGSCREEN_H
#define DLGSCREEN_H

#include "c5waiterdialog.h"

#include <QJsonArray>

class QTimer;
class ZkfingerprintReader;

namespace Ui
{
class DlgScreen;
}

class DlgScreen : public C5WaiterDialog
{
    Q_OBJECT

public:
    explicit DlgScreen(C5User *user);

    ~DlgScreen();

protected:
    virtual bool eventFilter(QObject *o, QEvent *e) override;

    virtual void paintEvent(QPaintEvent *e) override;

    virtual void showEvent(QShowEvent *e) override;

    void closeEvent(QCloseEvent *event) override;

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

    void on_btnFingerPrint_clicked();

    void onFingerprintProbe(const QByteArray &probe);

private:
    Ui::DlgScreen* ui;

    ZkfingerprintReader *mReader = nullptr;
    QJsonArray mFingerprintItems;
    bool mFingerprintBusy = false;

    void tryExit();

    void updatePin();

    void loadFingerprints();
    void startFingerprintScan();
    void stopFingerprintScan();
    int matchFingerprintUser(const QByteArray &probe);
    void completeLogin(C5User *user);
    void showFingerPrintStatus(const QString &text, const QColor &color, int hideMs = 0);
    void clearFingerPrintStatus();

    QString mPin;
    QTimer *mFingerPrintStatusTimer = nullptr;
};

#endif // DLGSCREEN_H
