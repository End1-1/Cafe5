#include "dlgattendanceauth.h"
#include "ui_dlgattendanceauth.h"

#include "c5message.h"
#include "c5user.h"
#include "ninterface.h"
#include "zkfingerprintreader.h"

#include <QCloseEvent>
#include <QShowEvent>
#include <QTimer>
#include <QToolButton>

DlgAttendanceAuth::DlgAttendanceAuth(C5User *workstationUser, QWidget *parent)
    : C5Dialog(workstationUser, parent)
    , ui(new Ui::DlgAttendanceAuth)
    , mWorkstationSession(workstationUser->mSessionKey)
{
    ui->setupUi(this);
    ui->lbHint->setText(tr("PIN or fingerprint"));

    const QList<QToolButton *> digitButtons = {
        ui->btn0,
        ui->btn1,
        ui->btn2,
        ui->btn3,
        ui->btn4,
        ui->btn5,
        ui->btn6,
        ui->btn7,
        ui->btn8,
        ui->btn9,
    };
    for(QToolButton *btn : digitButtons) {
        connect(btn, &QToolButton::clicked, this, &DlgAttendanceAuth::appendDigit);
    }
}

DlgAttendanceAuth::~DlgAttendanceAuth()
{
    stopFingerprintScan();
    delete ui;
}

void DlgAttendanceAuth::toggleAttendance(int userId, bool currentlyActive, const QString &userName, const std::function<void(bool completed)> &done)
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
                       mWorkstationSession,
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

void DlgAttendanceAuth::proceedAttendance(int userId, const std::function<void(bool completed)> &done)
{
    NInterface::query1(QStringLiteral("/engine/v2/common/attendance/open-user"),
                       mWorkstationSession,
                       this,
                       {{QStringLiteral("user_id"), userId}},
                       [this, userId, done](const QJsonObject &jo) {
                           toggleAttendance(userId,
                                            jo.value(QStringLiteral("active")).toBool(),
                                            jo.value(QStringLiteral("f_name")).toString(),
                                            done);
                       });
}

void DlgAttendanceAuth::showEvent(QShowEvent *event)
{
    C5Dialog::showEvent(event);

    if (event->spontaneous()) {
        return;
    }

    loadFingerprints();
}

void DlgAttendanceAuth::closeEvent(QCloseEvent *event)
{
    stopFingerprintScan();
    C5Dialog::closeEvent(event);
}

void DlgAttendanceAuth::appendDigit()
{
    mPin.append(static_cast<QToolButton *>(sender())->text());
    updatePin();
}

void DlgAttendanceAuth::updatePin()
{
    ui->lbPin->clear();

    for(int i = 0; i < mPin.length(); ++i) {
        ui->lbPin->setText(ui->lbPin->text() + QStringLiteral("●"));
    }
}

void DlgAttendanceAuth::loadFingerprints()
{
    NInterface::query1(QStringLiteral("/engine/v2/officen/user/list-fingerprints"),
                       mWorkstationSession,
                       this,
                       {},
                       [this](const QJsonObject &jo) {
                           mFingerprintItems = jo.value(QStringLiteral("items")).toArray();
                           if(!mFingerprintItems.isEmpty()) {
                               startFingerprintScan();
                           }
                       });
}

void DlgAttendanceAuth::startFingerprintScan()
{
    if(mFingerprintBusy || mFingerprintItems.isEmpty()) {
        return;
    }

    if(!mReader) {
        mReader = new ZkfingerprintReader(this);
        connect(mReader, &ZkfingerprintReader::probeReady, this, &DlgAttendanceAuth::onFingerprintProbe);
        connect(mReader, &ZkfingerprintReader::error, this, [this](const QString &message) {
            showFingerPrintStatus(message, Qt::red, 2500);
        });
    }

    if(!mReader->openDevice()) {
        return;
    }

    mFingerprintBusy = true;
    clearFingerPrintStatus();
    mReader->startIdentification();
}

void DlgAttendanceAuth::stopFingerprintScan()
{
    if(!mReader) {
        return;
    }

    mReader->cancelCapture();
    mReader->closeDevice();
    mFingerprintBusy = false;
}

int DlgAttendanceAuth::matchFingerprintUser(const QByteArray &probe) const
{
    if(!mReader) {
        return 0;
    }

    int score = 0;
    return mReader->identifyProbe(mFingerprintItems, probe, &score);
}

void DlgAttendanceAuth::showFingerPrintStatus(const QString &text, const QColor &color, int hideMs)
{
    if(!mFingerPrintStatusTimer) {
        mFingerPrintStatusTimer = new QTimer(this);
        mFingerPrintStatusTimer->setSingleShot(true);
        connect(mFingerPrintStatusTimer, &QTimer::timeout, this, &DlgAttendanceAuth::clearFingerPrintStatus);
    }

    mFingerPrintStatusTimer->stop();
    ui->lbFingerPrint->setStyleSheet(QStringLiteral("color: %1;").arg(color.name()));
    ui->lbFingerPrint->setText(text);

    if(hideMs > 0) {
        mFingerPrintStatusTimer->start(hideMs);
    }
}

void DlgAttendanceAuth::clearFingerPrintStatus()
{
    if(mFingerPrintStatusTimer) {
        mFingerPrintStatusTimer->stop();
    }

    ui->lbFingerPrint->clear();
    ui->lbFingerPrint->setStyleSheet(QString());
}

void DlgAttendanceAuth::onFingerprintProbe(const QByteArray &probe)
{
    mFingerprintBusy = false;

    const int userId = matchFingerprintUser(probe);
    if(userId <= 0) {
        showFingerPrintStatus(tr("Fingerprint not recognized"), Qt::red, 2500);
        QTimer::singleShot(2500, this, [this]() {
            startFingerprintScan();
        });
        return;
    }

    stopFingerprintScan();
    showFingerPrintStatus(tr("Verified"), QColor(0, 128, 0));

    proceedAttendance(userId, [this](bool completed) {
        if(completed) {
            accept();
        } else {
            loadFingerprints();
        }
    });
}

void DlgAttendanceAuth::on_btnAccept_clicked()
{
    stopFingerprintScan();

    const QString pin = mPin;
    mPin.clear();
    updatePin();

    if(pin.isEmpty()) {
        showFingerPrintStatus(tr("Enter PIN"), Qt::red, 2500);
        loadFingerprints();
        return;
    }

    NInterface::query1(QStringLiteral("/engine/v2/common/attendance/verify-pin"),
                       mWorkstationSession,
                       this,
                       {{QStringLiteral("pin"), pin}},
                       [this](const QJsonObject &jo) {
                           showFingerPrintStatus(tr("Verified"), QColor(0, 128, 0));
                           proceedAttendance(jo.value(QStringLiteral("user_id")).toInt(), [this](bool completed) {
                               if(completed) {
                                   accept();
                               } else {
                                   loadFingerprints();
                               }
                           });
                       });
}

void DlgAttendanceAuth::on_btnCancel_clicked()
{
    if(mPin.isEmpty()) {
        reject();
        return;
    }

    mPin.clear();
    updatePin();
}

void DlgAttendanceAuth::on_btnFingerPrint_clicked()
{
    if(mFingerprintBusy) {
        stopFingerprintScan();
        return;
    }

    if(mFingerprintItems.isEmpty()) {
        loadFingerprints();
        return;
    }

    startFingerprintScan();
}
