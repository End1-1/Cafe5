#include "dlgdebtsredeem.h"
#include "ui_dlgdebtsredeem.h"
#include "c5cache.h"
#include "c5codenameselectorfunctions.h"
#include "c5message.h"
#include "c5user.h"
#include "c5utils.h"
#include "dict_payment_type.h"
#include "format_date.h"
#include "ninterface.h"
#include "paymenttypenames.h"
#include <QComboBox>
#include <QCoreApplication>
#include <QDate>
#include <QDialogButtonBox>
#include <QPointer>
#include <QPushButton>
#include <QSignalBlocker>

DlgDebtsRedeem::DlgDebtsRedeem(C5User *user, QWidget *parent, int docType, int currencyId)
    : QDialog(parent)
    , ui(new Ui::DlgDebtsRedeem)
    , mUser(user)
    , mDocType(docType)
    , mCurrencyId(currencyId > 0 ? currencyId : 1)
{
    ui->setupUi(this);
    ui->deDate->setDate(QDate::currentDate());
    const QString kind = mDocType == 1
        ? tr("Supplier debt (we owe)")
        : tr("Customer debt (they owe us)");
    ui->lbContext->setText(kind);
    clearPartnerBalance();
    ui->wPartner->setSelectorName(tr("Partner"));
    ui->wPartner->selectorCallback = [this](C5CodeNameSelector *s) {
        partnerItemSelector(s);
        refreshPartnerBalance();
    };
    initPayment();

    ui->btnDelete->setVisible(false);
    ui->btnDelete->setStyleSheet(QStringLiteral("QPushButton { color: #b00020; }"));
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &DlgDebtsRedeem::tryAccept);
    connect(ui->btnDelete, &QPushButton::clicked, this, &DlgDebtsRedeem::onDeleteClicked);
    connect(ui->cbPayment, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &DlgDebtsRedeem::onPaymentChanged);
    connect(ui->btnPaymentConfig, &QPushButton::clicked, this, &DlgDebtsRedeem::onPaymentConfigClicked);
}

DlgDebtsRedeem::~DlgDebtsRedeem()
{
    delete ui;
}

void DlgDebtsRedeem::loadRedeem(const QJsonObject &redeem)
{
    mDebtId = redeem.value(QStringLiteral("debt_id")).toInt();
    setWindowTitle(tr("Edit debt payment"));
    ui->btnDelete->setVisible(true);

    const QString partnerName = redeem.value(QStringLiteral("partner_name")).toString();
    ui->wPartner->setCodeAndName(redeem.value(QStringLiteral("partner_id")).toInt(), partnerName);
    ui->wPartner->setEnabled(false);

    const QDate d = QDate::fromString(redeem.value(QStringLiteral("date")).toString(), QStringLiteral("yyyy-MM-dd"));
    if (d.isValid()) {
        ui->deDate->setDate(d);
    }
    ui->spAmount->setDouble(redeem.value(QStringLiteral("amount")).toDouble());
    ui->leComment->setText(redeem.value(QStringLiteral("comment")).toString());

    StorePaymentPreset preset;
    preset.cashboxId = redeem.value(QStringLiteral("cashbox_id")).toVariant().toInt();
    preset.cashboxName = redeem.value(QStringLiteral("cashbox_name")).toString();
    preset.paymentTypeId = redeem.value(QStringLiteral("payment_type_id")).toVariant().toInt();
    if (preset.cashboxId <= 0 || preset.paymentTypeId <= 0) {
        preset = StorePaymentPreset();
        mPaymentOptions.unpaidEnabled = true;
    }
    selectPayment(preset);
    ui->cbPayment->setEnabled(false);
    ui->btnPaymentConfig->setEnabled(false);
    refreshPartnerBalance();
}

int DlgDebtsRedeem::debtId() const
{
    return mDebtId;
}

bool DlgDebtsRedeem::isEditMode() const
{
    return mDebtId > 0;
}

bool DlgDebtsRedeem::deleteRequested() const
{
    return mDeleteRequested;
}

int DlgDebtsRedeem::partnerId() const
{
    return ui->wPartner->value();
}

double DlgDebtsRedeem::amount() const
{
    return ui->spAmount->getDouble();
}

int DlgDebtsRedeem::cashboxId() const
{
    return mCurrentPayment.isUnpaid() ? 0 : mCurrentPayment.cashboxId;
}

int DlgDebtsRedeem::paymentTypeId() const
{
    // Till is not used: the ledger payment stays a bank transfer.
    return mCurrentPayment.isUnpaid() ? PAYMENT_TYPE_BANK : mCurrentPayment.paymentTypeId;
}

QString DlgDebtsRedeem::comment() const
{
    return ui->leComment->text().trimmed();
}

QString DlgDebtsRedeem::redeemDateMysql() const
{
    return ui->deDate->date().toString(FORMAT_DATE_TO_STR_MYSQL);
}

QVector<QPair<int, QString>> DlgDebtsRedeem::loadCashboxList() const
{
    QVector<QPair<int, QString>> out;
    C5Cache *cache = C5Cache::cache(cache_cash_names);
    if (!cache) {
        return out;
    }
    for (int r = 0; r < cache->rowCount(); ++r) {
        const int id = cache->getInt(r, 0);
        if (id > 0) {
            out.append({id, cache->getString(r, 1)});
        }
    }
    return out;
}

void DlgDebtsRedeem::initPayment()
{
    mPaymentOptions.settingsGroup = QStringLiteral("DebtRedeemPayment");
    mPaymentOptions.load();
    selectPayment(StorePaymentPreset());
    QPointer<DlgDebtsRedeem> self(this);
    PaymentTypeNames::load(mUser->mSessionKey, this, [self]() {
        if (self) {
            self->selectPayment(self->mCurrentPayment);
        }
    });
}

void DlgDebtsRedeem::selectPayment(const StorePaymentPreset &preset)
{
    mPaymentPresets = mPaymentOptions.buildPresets(loadCashboxList());
    mCurrentPayment = StorePaymentPreset();
    if (!preset.isUnpaid()) {
        for (const StorePaymentPreset &p : mPaymentPresets) {
            if (p.cashboxId == preset.cashboxId && p.paymentTypeId == preset.paymentTypeId) {
                mCurrentPayment = p;
                break;
            }
        }
        if (mCurrentPayment.isUnpaid()) {
            mCurrentPayment = preset;
            mCurrentPayment.paymentTypeName = PaymentTypeNames::title(preset.paymentTypeId);
            mCurrentPayment.currencyId = 1;
            mPaymentPresets.append(mCurrentPayment);
        }
    }
    rebuildPaymentCombo();
}

void DlgDebtsRedeem::rebuildPaymentCombo()
{
    const QSignalBlocker blocker(ui->cbPayment);
    ui->cbPayment->clear();
    const bool showUnpaid = mPaymentOptions.unpaidEnabled;
    if (showUnpaid) {
        ui->cbPayment->addItem(tr("Not from cashbox"));
    }
    for (const StorePaymentPreset &p : mPaymentPresets) {
        ui->cbPayment->addItem(p.label());
    }

    int idx = -1;
    if (mCurrentPayment.isUnpaid()) {
        if (showUnpaid) {
            idx = 0;
        } else if (!mPaymentPresets.isEmpty()) {
            mCurrentPayment = mPaymentPresets.first();
            idx = 0;
        }
    } else {
        for (int i = 0; i < mPaymentPresets.size(); ++i) {
            if (mPaymentPresets.at(i).matches(mCurrentPayment)) {
                idx = i + (showUnpaid ? 1 : 0);
                break;
            }
        }
    }
    if (idx >= 0) {
        ui->cbPayment->setCurrentIndex(idx);
    }
}

void DlgDebtsRedeem::onPaymentChanged(int index)
{
    if (index < 0) {
        return;
    }
    const bool showUnpaid = mPaymentOptions.unpaidEnabled;
    if (showUnpaid && index == 0) {
        mCurrentPayment = StorePaymentPreset();
        return;
    }
    const int presetIndex = index - (showUnpaid ? 1 : 0);
    if (presetIndex >= 0 && presetIndex < mPaymentPresets.size()) {
        mCurrentPayment = mPaymentPresets.at(presetIndex);
    }
}

void DlgDebtsRedeem::onPaymentConfigClicked()
{
    if (!DlgStoreInputPayment::configure(mPaymentOptions,
                                         loadCashboxList(),
                                         this,
                                         tr("Not from cashbox"),
                                         tr("Enable payment methods available when paying a debt"))) {
        return;
    }
    selectPayment(mCurrentPayment);
}

void DlgDebtsRedeem::clearPartnerBalance()
{
    ui->lbBalance->setText(tr("Current debt: —"));
}

void DlgDebtsRedeem::setPartnerBalance(double balance)
{
    const QString amount = float_str(balance, 2);
    if (mDocType == 1) {
        ui->lbBalance->setText(tr("We owe: %1").arg(amount));
    } else {
        ui->lbBalance->setText(tr("They owe us: %1").arg(amount));
    }
}

void DlgDebtsRedeem::refreshPartnerBalance()
{
    if (!mUser) {
        clearPartnerBalance();
        return;
    }
    const int partner = ui->wPartner->value();
    if (partner <= 0) {
        clearPartnerBalance();
        return;
    }

    QPointer<DlgDebtsRedeem> self(this);
    const int requestPartner = partner;
    NInterface::query1(QStringLiteral("/engine/v2/waiter/cashbox/partner-debt-balance"),
                       mUser->mSessionKey,
                       this,
                       {
                           {QStringLiteral("partner_id"), partner},
                           {QStringLiteral("doc_type"), mDocType},
                           {QStringLiteral("currency_id"), mCurrencyId},
                       },
                       [self, requestPartner](const QJsonObject &jo) {
                           if (!self) {
                               return;
                           }
                           if (self->ui->wPartner->value() != requestPartner) {
                               return;
                           }
                           self->setPartnerBalance(jo.value(QStringLiteral("balance")).toDouble());
                       });
}

void DlgDebtsRedeem::tryAccept()
{
    if(ui->wPartner->value() <= 0) {
        C5Message::error(tr("Select partner"));
        return;
    }
    if(ui->spAmount->getDouble() <= 0.00001) {
        C5Message::error(tr("Amount must be greater than zero"));
        return;
    }
    if(ui->cbPayment->currentIndex() < 0 || paymentTypeId() <= 0) {
        C5Message::error(tr("Payment type not specified"));
        return;
    }
    if(!isEditMode() && !mCurrentPayment.isUnpaid() && mCurrentPayment.cashboxId <= 0) {
        C5Message::error(tr("Cashbox not selected"));
        return;
    }
    QDialog::accept();
}

void DlgDebtsRedeem::onDeleteClicked()
{
    if(C5Message::question(tr("Delete this debt payment?")) != QDialog::Accepted) {
        return;
    }
    mDeleteRequested = true;
    QDialog::accept();
}
