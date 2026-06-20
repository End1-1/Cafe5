#include "dlguserfingerprint.h"
#include "ui_dlguserfingerprint.h"

#include <QCloseEvent>
#include "c5user.h"
#include "ninterface.h"
#include "zkfingerprintreader.h"

DlgUserFingerprint::DlgUserFingerprint(int userId, C5User *user)
    : C5Dialog(user)
    , ui(new Ui::DlgUserFingerprint)
    , mUserId(userId)
{
    ui->setupUi(this);

    mReader = new ZkfingerprintReader(this);

    connect(mReader, &ZkfingerprintReader::imageReady, this, [this](const QImage &image) {
        ui->lbPreview->setPixmap(
            QPixmap::fromImage(image).scaled(ui->lbPreview->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    });

    connect(mReader, &ZkfingerprintReader::progress, this, [this](int step, int total, const QString &message) {
        ui->progressBar->setMaximum(total);
        ui->progressBar->setValue(step);
        ui->lbProgress->setText(message);
    });

    connect(mReader, &ZkfingerprintReader::templateReady, this, [this](const QByteArray &mergedTemplate, int verifyScore) {
        mEnrolling = false;
        updateUi();
        saveTemplate(mergedTemplate, verifyScore);
    });

    connect(mReader, &ZkfingerprintReader::error, this, [this](const QString &message) {
        ui->lbProgress->setText(message);
        C5Message::info(message);
    });

    loadUser();
}

DlgUserFingerprint::~DlgUserFingerprint()
{
    stopEnrollment();
    if(mReader) {
        mReader->closeDevice();
    }
    delete ui;
}

void DlgUserFingerprint::edit(int userId, C5User *user)
{
    DlgUserFingerprint dlg(userId, user);
    dlg.exec();
}

void DlgUserFingerprint::loadUser()
{
    NInterface::query1(QStringLiteral("/engine/v2/officen/user/open"),
                       mUser->mSessionKey,
                       this,
                       {{QStringLiteral("user_id"), mUserId}},
                       [this](const QJsonObject &jo) {
                           ui->lbStaff->setText(jo.value(QStringLiteral("f_name")).toString());
                           mHasFingerprint = jo.value(QStringLiteral("has_fingerprint")).toBool();
                           mUpdatedAt = jo.value(QStringLiteral("f_updated")).toString();
                           updateUi();
                       });
}

void DlgUserFingerprint::updateUi()
{
    if(mHasFingerprint) {
        QString status = tr("Fingerprint is registered");
        if(!mUpdatedAt.isEmpty()) {
            status += QStringLiteral(" (%1)").arg(mUpdatedAt);
        }
        ui->lbStatus->setText(status);
    } else {
        ui->lbStatus->setText(tr("No fingerprint registered"));
    }

    ui->btnRemove->setEnabled(mHasFingerprint && !mEnrolling);
    ui->btnReplace->setText(mEnrolling
                                ? tr("Cancel")
                                : (mHasFingerprint ? tr("Replace fingerprint") : tr("Register fingerprint")));
}

void DlgUserFingerprint::stopEnrollment()
{
    if(!mEnrolling) {
        return;
    }

    mReader->cancelCapture();
    mEnrolling = false;
    ui->progressBar->setValue(0);
    ui->lbProgress->clear();
    updateUi();
}

void DlgUserFingerprint::saveTemplate(const QByteArray &templateData, int verifyScore)
{
    Q_UNUSED(verifyScore);

    NInterface::query1(QStringLiteral("/engine/v2/officen/user/save-fingerprint"),
                       mUser->mSessionKey,
                       this,
                       {{QStringLiteral("user_id"), mUserId},
                        {QStringLiteral("template"), QString::fromLatin1(templateData.toBase64())},
                        {QStringLiteral("size"), templateData.size()}},
                       [this](const QJsonObject &) {
                           C5Message::info(tr("Fingerprint saved"));
                           loadUser();
                       });
}

void DlgUserFingerprint::on_btnReplace_clicked()
{
    if(mEnrolling) {
        stopEnrollment();
        return;
    }

    if(!mReader->openDevice()) {
        return;
    }

    mEnrolling = true;
    ui->lbPreview->clear();
    ui->progressBar->setValue(0);
    ui->lbProgress->setText(tr("Waiting for scanner..."));
    updateUi();
    mReader->startEnrollment();
}

void DlgUserFingerprint::on_btnRemove_clicked()
{
    if(!mHasFingerprint || mEnrolling) {
        return;
    }

    if(C5Message::question(tr("Remove fingerprint for this user?")) != QDialog::Accepted) {
        return;
    }

    NInterface::query1(QStringLiteral("/engine/v2/officen/user/remove-fingerprint"),
                       mUser->mSessionKey,
                       this,
                       {{QStringLiteral("user_id"), mUserId}},
                       [this](const QJsonObject &) {
                           C5Message::info(tr("Fingerprint removed"));
                           ui->lbPreview->clear();
                           loadUser();
                       });
}

void DlgUserFingerprint::on_btnClose_clicked()
{
    reject();
}

void DlgUserFingerprint::closeEvent(QCloseEvent *event)
{
    stopEnrollment();
    if(mReader) {
        mReader->closeDevice();
    }
    C5Dialog::closeEvent(event);
}
