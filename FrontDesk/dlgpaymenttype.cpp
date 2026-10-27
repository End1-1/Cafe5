#include "dlgpaymenttype.h"
#include "ui_dlgpaymenttype.h"
#include "c5user.h"
#include "dict_payment_type.h"
#include "ninterface.h"
#include "paymenttypenames.h"
#include <QJsonObject>

DlgPaymentType::DlgPaymentType(C5User *user, int id, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgPaymentType)
    , mUser(user)
    , mId(id)
{
    ui->setupUi(this);
    ui->labelRequirePartner->setVisible(false);
    ui->chRequirePartner->setVisible(false);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &DlgPaymentType::trySave);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    loadData();
}

DlgPaymentType::~DlgPaymentType()
{
    delete ui;
}

void DlgPaymentType::applyRow(const QJsonObject &row)
{
    mId = row.value(QStringLiteral("f_id")).toVariant().toInt();
    ui->leCode->setText(QString::number(mId));
    ui->leDefaultName->setText(row.value(QStringLiteral("f_default_name")).toString());
    ui->leName->setText(row.value(QStringLiteral("f_name")).toString());
    ui->chWaiter->setChecked(row.value(QStringLiteral("f_waiter")).toInt(1) != 0);
    const bool transfer = mId == PAYMENT_TYPE_BANK || mId == PAYMENT_TYPE_BANK2 || mId == PAYMENT_TYPE_BANK3;
    ui->labelRequirePartner->setVisible(transfer);
    ui->chRequirePartner->setVisible(transfer);
    ui->chRequirePartner->setChecked(row.value(QStringLiteral("f_require_partner")).toInt(1) != 0);
}

void DlgPaymentType::loadData()
{
    NInterface::query1(QStringLiteral("/engine/v2/officen/editors/get-item"),
                       mUser->mSessionKey,
                       this,
                       {{QStringLiteral("editor"), QStringLiteral("form_payment_types")},
                        {QStringLiteral("id"), mId}},
                       [this](const QJsonObject &jdoc) {
                           applyRow(jdoc.value(QStringLiteral("row")).toObject());
                       });
}

void DlgPaymentType::trySave()
{
    const QString name = ui->leName->text().trimmed();
    const bool waiter = ui->chWaiter->isChecked();
    const bool requirePartner = ui->chRequirePartner->isChecked();
    NInterface::query(QStringLiteral("/engine/v2/officen/editors/save"),
                      mUser->mSessionKey,
                      this,
                      {
                          {QStringLiteral("editor"), QStringLiteral("form_payment_types")},
                          {QStringLiteral("f_id"), mId},
                          {QStringLiteral("f_name"), name},
                          {QStringLiteral("f_waiter"), waiter ? 1 : 0},
                          {QStringLiteral("f_require_partner"), requirePartner ? 1 : 0},
                      },
                      [this, name, waiter, requirePartner](const QJsonObject &) {
                          PaymentTypeNames::setCustomName(mId, name);
                          PaymentTypeNames::setWaiterEnabled(mId, waiter);
                          PaymentTypeNames::setPartnerRequired(mId, requirePartner);
                          accept();
                      },
                      [](const QJsonObject &) { return false; },
                      true);
}
