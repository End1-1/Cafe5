#include "dlgscreen.h"
#include "ui_dlgscreen.h"
#include "c5user.h"
#include "dlgface.h"
#include "ndataprovider.h"
#include "c5permissions.h"
#include "c5message.h"
#include "c5connectiondialog.h"
#include "dict_workstation.h"
#include "ninterface.h"
#include "c5utils.h"
#include "struct_workstationitem.h"
#include "dlgdashboard.h"
#include "zkfingerprintreader.h"
#include <QCloseEvent>
#include <QJsonArray>
#include <QKeyEvent>
#include <QPainter>
#include <QTimer>

DlgScreen::DlgScreen(C5User *user) :
    C5WaiterDialog(user),
    ui(new Ui::DlgScreen)
{
    ui->setupUi(this);
    QPalette pal = palette();
    pal.setBrush(QPalette::Window, QBrush(QPixmap(":/waiterbg.jpg")));
    setAutoFillBackground(true);
    setPalette(pal);

    ui->lbVersion->setText(NDataProvider::mFileVersion);
    ui->lbFingerPrint->clear();
    updatePin();
    installEventFilter(this);
    setFocusPolicy(Qt::StrongFocus);
    setFocus();
}

DlgScreen::~DlgScreen()
{
    stopFingerprintScan();
    delete ui;
}

bool DlgScreen::eventFilter(QObject *o, QEvent *e)
{
    if(e->type() == QEvent::KeyPress) {
        auto *k = static_cast<QKeyEvent*>(e);

        if(k->key() == Qt::Key_Escape ||
                k->key() == Qt::Key_Control ||
                k->key() == Qt::Key_Shift ||
                k->key() == Qt::Key_Alt ||
                k->key() == Qt::Key_Meta) {
            return true;
        }

        if(k->key() == Qt::Key_Return || k->key() == Qt::Key_Enter) {
            on_btnAccept_clicked();
            return true;
        }

        if(k->key() == Qt::Key_Backspace && !mPin.isEmpty()) {
            mPin.chop(1);
            updatePin();
            return true;
        }

        if(!k->text().isEmpty()) {
            mPin += k->text();
            updatePin();
            return true;
        }
    }

    return C5WaiterDialog::eventFilter(o, e);
}

void DlgScreen::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter p(this);
    QPixmap mBg(":/waiterbg.jpg");

    if(!mBg.isNull()) {
        // cover: сохраняем пропорции, обрезаем лишнее
        QPixmap scaled = mBg.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        QPoint topLeft((width() - scaled.width()) / 2, (height() - scaled.height()) / 2);
        p.drawPixmap(topLeft, scaled);
    }
}

void DlgScreen::showEvent(QShowEvent *e)
{
    C5WaiterDialog::showEvent(e);
    NInterface::query1("/engine/v2/common/workstation/get-config",
                       mUser->mSessionKey,
                       this,
                       {{"type", WORKSTATION_WAITER},
                        {"station_account", hostusername()},
                        {"workstation", hostinfo}},
                       [](const QJsonObject &jdoc) {
                           mWorkStation = JsonParser<WorkstationItem>::fromJson(jdoc);
                       });
    loadFingerprints();
}

void DlgScreen::closeEvent(QCloseEvent *event)
{
    stopFingerprintScan();
    C5WaiterDialog::closeEvent(event);
}

void DlgScreen::loadFingerprints()
{
    NInterface::query1(QStringLiteral("/engine/v2/officen/user/list-fingerprints"),
                       mUser->mSessionKey,
                       this,
                       {},
                       [this](const QJsonObject &jo) {
                           mFingerprintItems = jo.value(QStringLiteral("items")).toArray();
                           if(!mFingerprintItems.isEmpty()) {
                               startFingerprintScan();
                           }
                       });
}

void DlgScreen::startFingerprintScan()
{
    if(mFingerprintBusy || mFingerprintItems.isEmpty()) {
        return;
    }

    if(!mReader) {
        mReader = new ZkfingerprintReader(this);
        connect(mReader, &ZkfingerprintReader::probeReady, this, &DlgScreen::onFingerprintProbe);
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

void DlgScreen::stopFingerprintScan()
{
    if(!mReader) {
        return;
    }

    mReader->cancelCapture();
    mReader->closeDevice();
    mFingerprintBusy = false;
}

void DlgScreen::showFingerPrintStatus(const QString &text, const QColor &color, int hideMs)
{
    if(!mFingerPrintStatusTimer) {
        mFingerPrintStatusTimer = new QTimer(this);
        mFingerPrintStatusTimer->setSingleShot(true);
        connect(mFingerPrintStatusTimer, &QTimer::timeout, this, &DlgScreen::clearFingerPrintStatus);
    }

    mFingerPrintStatusTimer->stop();
    ui->lbFingerPrint->setStyleSheet(QStringLiteral("color: %1;").arg(color.name()));
    ui->lbFingerPrint->setText(text);

    if(hideMs > 0) {
        mFingerPrintStatusTimer->start(hideMs);
    }
}

void DlgScreen::clearFingerPrintStatus()
{
    if(mFingerPrintStatusTimer) {
        mFingerPrintStatusTimer->stop();
    }

    ui->lbFingerPrint->clear();
    ui->lbFingerPrint->setStyleSheet(QString());
}

int DlgScreen::matchFingerprintUser(const QByteArray &probe)
{
    if(!mReader) {
        return 0;
    }

    int score = 0;
    return mReader->identifyProbe(mFingerprintItems, probe, &score);
}

void DlgScreen::onFingerprintProbe(const QByteArray &probe)
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

    showFingerPrintStatus(tr("Login successful"), QColor(0, 128, 0));
    stopFingerprintScan();

    auto *user = new C5User();
    NDataProvider::sessionKey = mUser->mSessionKey;
    user->authorizeByUserId(userId, fHttp, [this, user](const QJsonObject &) {
        completeLogin(user);
    }, [this, user]() {
        user->deleteLater();
        showFingerPrintStatus(tr("Access denied"), Qt::red, 2500);
        NDataProvider::sessionKey = mUser->mSessionKey;
        QTimer::singleShot(2500, this, [this]() {
            loadFingerprints();
        });
    });
}

void DlgScreen::completeLogin(C5User *user)
{
    if(!user->check(cp_t5_waiter_edit_order)) {
        DlgDashboard(QJsonObject(), user).exec();
        user->deleteLater();
        NDataProvider::sessionKey = mUser->mSessionKey;
        loadFingerprints();
        return;
    }

    QTimer::singleShot(1, user, [this, user]() {
        int finish = 0;

        do {
            QDialog *dlg;

            switch(finish) {
            case 2:
                dlg = new DlgDashboard(QJsonObject(), user);
                break;

            case 3:
                dlg = new DlgFace(user);
                break;

            default:
                dlg = new DlgFace(user);
            }

            finish = dlg->exec();
            delete dlg;
        } while(finish > 1);

        user->deleteLater();
        NDataProvider::sessionKey = mUser->mSessionKey;
        loadFingerprints();
    });
}

void DlgScreen::on_btnCancel_clicked()
{
    mPin.clear();
    updatePin();
}

void DlgScreen::on_btn1_clicked()
{
    mPin.append(static_cast<QToolButton*>(sender())->text());
    updatePin();
}
void DlgScreen::on_btn2_clicked()
{
    mPin.append(static_cast<QToolButton*>(sender())->text());
    updatePin();
}

void DlgScreen::on_btn3_clicked()
{
    mPin.append(static_cast<QToolButton*>(sender())->text());
    updatePin();
}

void DlgScreen::on_btn4_clicked()
{
    mPin.append(static_cast<QToolButton*>(sender())->text());
    updatePin();
}

void DlgScreen::on_btn5_clicked()
{
    mPin.append(static_cast<QToolButton*>(sender())->text());
    updatePin();
}

void DlgScreen::on_btn6_clicked()
{
    mPin.append(static_cast<QToolButton*>(sender())->text());
    updatePin();
}

void DlgScreen::on_btn7_clicked()
{
    mPin.append(static_cast<QToolButton*>(sender())->text());
    updatePin();
}

void DlgScreen::on_btn8_clicked()
{
    mPin.append(static_cast<QToolButton*>(sender())->text());
    updatePin();
}

void DlgScreen::on_btn9_clicked()
{
    mPin.append(static_cast<QToolButton*>(sender())->text());
    updatePin();
}

void DlgScreen::on_btn0_clicked()
{
    mPin.append(static_cast<QToolButton*>(sender())->text());
    updatePin();
}

void DlgScreen::on_btnAccept_clicked()
{
    stopFingerprintScan();

    auto *user = new C5User();
    QString pin = mPin;
    mPin.clear();
    updatePin();
    user->authorize(pin, fHttp, [this, user](const QJsonObject &) {
        completeLogin(user);
    }, [user]() {
        user->deleteLater();
    });
}

void DlgScreen::on_lePassword_returnPressed()
{
    on_btnAccept_clicked();
}

void DlgScreen::tryExit()
{
    if(C5Message::question(tr("Are you sure to close application")) == QDialog::Accepted) {
        qApp->quit();
    }
}

void DlgScreen::updatePin()
{
    ui->lbPin->clear();

    for(int i = 0; i < mPin.length(); i++) {
        ui->lbPin->setText(ui->lbPin->text() + "●");
    }
}

void DlgScreen::on_btnSettings_clicked()
{
    C5ConnectionDialog::showSettings(this);
}

void DlgScreen::on_btnClose_clicked()
{
    if(C5Message::question(tr("Are you sure to close application")) == QDialog::Accepted) {
        qApp->quit();
    }
}

void DlgScreen::on_btnFingerPrint_clicked()
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
