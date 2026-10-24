#include "dlgcashdiscrepancy.h"
#include "ui_dlgcashdiscrepancy.h"
#include "c5message.h"
#include "c5user.h"
#include "ninterface.h"
#include <QJsonObject>

namespace {

constexpr int kCommentMaxLength = 1024;
constexpr int kTypeShortage = 1;

}

DlgCashDiscrepancy::DlgCashDiscrepancy(C5User *user, int id, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgCashDiscrepancy)
    , mUser(user)
    , mId(id)
{
    ui->setupUi(this);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &DlgCashDiscrepancy::trySave);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    loadData();
}

DlgCashDiscrepancy::~DlgCashDiscrepancy()
{
    delete ui;
}

void DlgCashDiscrepancy::applyRow(const QJsonObject &row)
{
    mId = row.value(QStringLiteral("f_id")).toVariant().toInt();
    ui->lbDate->setText(row.value(QStringLiteral("f_date")).toString());
    ui->lbCashbox->setText(row.value(QStringLiteral("f_cashbox_name")).toString());
    ui->lbEvent->setText(row.value(QStringLiteral("f_stage_name")).toString());
    ui->lbType->setText(row.value(QStringLiteral("f_type_name")).toString());
    ui->lbType->setStyleSheet(row.value(QStringLiteral("f_type")).toVariant().toInt() == kTypeShortage
                              ? QStringLiteral("color: #b00020; font-weight: bold;")
                              : QStringLiteral("color: #1b7f3b; font-weight: bold;"));
    ui->lbAmount->setText(row.value(QStringLiteral("f_amount")).toString());
    const int sessionId = row.value(QStringLiteral("f_session_id")).toVariant().toInt();
    ui->lbShift->setText(sessionId > 0 ? QStringLiteral("#%1").arg(sessionId) : QString());
    ui->lbShiftStaff->setText(row.value(QStringLiteral("f_shift_user")).toString());
    ui->lbCountedBy->setText(row.value(QStringLiteral("f_counted_user")).toString());
    ui->chkResolved->setChecked(row.value(QStringLiteral("f_resolved")).toVariant().toInt() == 1);

    const QString resolvedBy = row.value(QStringLiteral("f_resolved_user")).toString();
    const QString resolvedAt = row.value(QStringLiteral("f_resolved_date")).toString();
    ui->lbResolvedInfo->setText(ui->chkResolved->isChecked()
                                ? tr("Resolved by %1, %2").arg(resolvedBy, resolvedAt)
                                : QString());
    ui->lbResolvedInfo->setVisible(ui->chkResolved->isChecked());
    ui->teComment->setPlainText(row.value(QStringLiteral("f_comment")).toString());
}

void DlgCashDiscrepancy::loadData()
{
    NInterface::query1(QStringLiteral("/engine/v2/officen/editors/get-item"),
                       mUser->mSessionKey,
                       this,
                       {{QStringLiteral("editor"), QStringLiteral("form_cash_discrepancies")},
                        {QStringLiteral("id"), mId}},
                       [this](const QJsonObject &jdoc) {
                           applyRow(jdoc.value(QStringLiteral("row")).toObject());
                       });
}

void DlgCashDiscrepancy::trySave()
{
    const QString comment = ui->teComment->toPlainText().trimmed();
    if(comment.length() > kCommentMaxLength) {
        C5Message::error(tr("Comment is too long"));
        return;
    }

    NInterface::query(QStringLiteral("/engine/v2/officen/editors/save"),
                      mUser->mSessionKey,
                      this,
                      {
                          {QStringLiteral("editor"), QStringLiteral("form_cash_discrepancies")},
                          {QStringLiteral("f_id"), mId},
                          {QStringLiteral("f_resolved"), ui->chkResolved->isChecked() ? 1 : 0},
                          {QStringLiteral("f_comment"), comment},
                      },
                      [this](const QJsonObject &) {
                          accept();
                      },
                      [](const QJsonObject &) { return false; },
                      true);
}
