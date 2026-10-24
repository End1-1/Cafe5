#include "dlgstoreinputpayment.h"
#include "ui_dlgstoreinputpayment.h"
#include "c5codenameselectorfunctions.h"
#include "c5message.h"
#include "dict_payment_type.h"
#include "paymenttypenames.h"
#include <QCheckBox>
#include <QCoreApplication>
#include <QDialogButtonBox>
#include <QListWidgetItem>
#include <QSettings>
#include <QSet>

namespace {

QString paymentTypeTitle(int pt)
{
    return PaymentTypeNames::title(pt);
}

QString settingsPath(const QString &group)
{
    return QStringLiteral("%1\\%2").arg(QLatin1String(_APPLICATION_), group);
}

} // namespace

QString StorePaymentPreset::label() const
{
    if (isUnpaid()) {
        return QCoreApplication::translate("C5StoreInput", "Unpaid");
    }
    if (paymentTypeId == PAYMENT_TYPE_CASH) {
        return cashboxName.isEmpty() ? paymentTypeTitle(PAYMENT_TYPE_CASH) : cashboxName;
    }
    return paymentTypeTitle(paymentTypeId);
}

bool StorePaymentPreset::matches(const StorePaymentPreset &o) const
{
    return cashboxId == o.cashboxId
           && paymentTypeId == o.paymentTypeId
           && currencyId == o.currencyId;
}

void StorePaymentOptions::load()
{
    QSettings s(_ORGANIZATION_, settingsPath(settingsGroup));
    unpaidEnabled = s.value(QStringLiteral("unpaid"), true).toBool();
    cardEnabled = s.value(QStringLiteral("card"), false).toBool();
    bankEnabled = s.value(QStringLiteral("bank"), false).toBool();
    cardCashboxId = s.value(QStringLiteral("cardCashboxId"), 0).toInt();
    cardCashboxName = s.value(QStringLiteral("cardCashboxName")).toString();
    bankCashboxId = s.value(QStringLiteral("bankCashboxId"), 0).toInt();
    bankCashboxName = s.value(QStringLiteral("bankCashboxName")).toString();
    bank2Enabled = s.value(QStringLiteral("bank2"), false).toBool();
    bank2CashboxId = s.value(QStringLiteral("bank2CashboxId"), 0).toInt();
    bank2CashboxName = s.value(QStringLiteral("bank2CashboxName")).toString();
    bank3Enabled = s.value(QStringLiteral("bank3"), false).toBool();
    bank3CashboxId = s.value(QStringLiteral("bank3CashboxId"), 0).toInt();
    bank3CashboxName = s.value(QStringLiteral("bank3CashboxName")).toString();
    cashboxesConfigured = s.contains(QStringLiteral("cashboxIds"));
    enabledCashboxIds.clear();
    const QVariantList ids = s.value(QStringLiteral("cashboxIds")).toList();
    for (const QVariant &v : ids) {
        const int id = v.toInt();
        if (id > 0) {
            enabledCashboxIds.append(id);
        }
    }
    if (!cashboxesConfigured && s.value(QStringLiteral("cash"), true).toBool()) {
        cashboxesConfigured = false;
    } else if (!cashboxesConfigured && !s.value(QStringLiteral("cash"), true).toBool()) {
        cashboxesConfigured = true;
        enabledCashboxIds.clear();
    }
}

void StorePaymentOptions::save() const
{
    QSettings s(_ORGANIZATION_, settingsPath(settingsGroup));
    s.setValue(QStringLiteral("unpaid"), unpaidEnabled);
    s.setValue(QStringLiteral("card"), cardEnabled);
    s.setValue(QStringLiteral("bank"), bankEnabled);
    s.setValue(QStringLiteral("cardCashboxId"), cardCashboxId);
    s.setValue(QStringLiteral("cardCashboxName"), cardCashboxName);
    s.setValue(QStringLiteral("bankCashboxId"), bankCashboxId);
    s.setValue(QStringLiteral("bankCashboxName"), bankCashboxName);
    s.setValue(QStringLiteral("bank2"), bank2Enabled);
    s.setValue(QStringLiteral("bank2CashboxId"), bank2CashboxId);
    s.setValue(QStringLiteral("bank2CashboxName"), bank2CashboxName);
    s.setValue(QStringLiteral("bank3"), bank3Enabled);
    s.setValue(QStringLiteral("bank3CashboxId"), bank3CashboxId);
    s.setValue(QStringLiteral("bank3CashboxName"), bank3CashboxName);
    QVariantList ids;
    for (int id : enabledCashboxIds) {
        if (id > 0) {
            ids.append(id);
        }
    }
    s.setValue(QStringLiteral("cashboxIds"), ids);
    s.setValue(QStringLiteral("cash"), !ids.isEmpty());
}

QVector<StorePaymentPreset> StorePaymentOptions::buildPresets(const QVector<QPair<int, QString>> &cashboxes) const
{
    QVector<StorePaymentPreset> out;
    // Currency is chosen elsewhere later; keep AMD as backend default for now.
    constexpr int curId = 1;

    QSet<int> allowed;
    if (!cashboxesConfigured) {
        for (const auto &cb : cashboxes) {
            if (cb.first > 0) {
                allowed.insert(cb.first);
            }
        }
    } else {
        for (int id : enabledCashboxIds) {
            if (id > 0) {
                allowed.insert(id);
            }
        }
    }

    for (const auto &cb : cashboxes) {
        if (cb.first <= 0 || !allowed.contains(cb.first)) {
            continue;
        }
        StorePaymentPreset p;
        p.cashboxId = cb.first;
        p.cashboxName = cb.second;
        p.paymentTypeId = PAYMENT_TYPE_CASH;
        p.paymentTypeName = paymentTypeTitle(PAYMENT_TYPE_CASH);
        p.currencyId = curId;
        out.append(p);
    }
    if (cardEnabled && cardCashboxId > 0) {
        StorePaymentPreset p;
        p.cashboxId = cardCashboxId;
        p.cashboxName = cardCashboxName;
        p.paymentTypeId = PAYMENT_TYPE_CARD;
        p.paymentTypeName = paymentTypeTitle(PAYMENT_TYPE_CARD);
        p.currencyId = curId;
        out.append(p);
    }
    const struct {
        bool enabled;
        int cashboxId;
        const QString &cashboxName;
        int paymentTypeId;
    } banks[] = {
        {bankEnabled, bankCashboxId, bankCashboxName, PAYMENT_TYPE_BANK},
        {bank2Enabled, bank2CashboxId, bank2CashboxName, PAYMENT_TYPE_BANK2},
        {bank3Enabled, bank3CashboxId, bank3CashboxName, PAYMENT_TYPE_BANK3},
    };
    for (const auto &b : banks) {
        if (!b.enabled || b.cashboxId <= 0) {
            continue;
        }
        StorePaymentPreset p;
        p.cashboxId = b.cashboxId;
        p.cashboxName = b.cashboxName;
        p.paymentTypeId = b.paymentTypeId;
        p.paymentTypeName = paymentTypeTitle(b.paymentTypeId);
        p.currencyId = curId;
        out.append(p);
    }
    return out;
}

DlgStoreInputPayment::DlgStoreInputPayment(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgStoreInputPayment)
{
    ui->setupUi(this);
    ui->wCardCashbox->selectorCallback = cashboxItemSelector;
    ui->wBankCashbox->selectorCallback = cashboxItemSelector;
    ui->wBank2Cashbox->selectorCallback = cashboxItemSelector;
    ui->wBank3Cashbox->selectorCallback = cashboxItemSelector;
    ui->chkCard->setText(paymentTypeTitle(PAYMENT_TYPE_CARD));
    ui->chkBank->setText(paymentTypeTitle(PAYMENT_TYPE_BANK));
    ui->chkBank2->setText(paymentTypeTitle(PAYMENT_TYPE_BANK2));
    ui->chkBank3->setText(paymentTypeTitle(PAYMENT_TYPE_BANK3));

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &DlgStoreInputPayment::tryAccept);
    connect(ui->chkCard, &QCheckBox::toggled, this, &DlgStoreInputPayment::syncCashboxEnabled);
    connect(ui->chkBank, &QCheckBox::toggled, this, &DlgStoreInputPayment::syncCashboxEnabled);
    connect(ui->chkBank2, &QCheckBox::toggled, this, &DlgStoreInputPayment::syncCashboxEnabled);
    connect(ui->chkBank3, &QCheckBox::toggled, this, &DlgStoreInputPayment::syncCashboxEnabled);
    syncCashboxEnabled();
}

DlgStoreInputPayment::~DlgStoreInputPayment()
{
    delete ui;
}

void DlgStoreInputPayment::setCashboxes(const QVector<QPair<int, QString>> &cashboxes)
{
    fillCashboxList(cashboxes, {}, true);
}

void DlgStoreInputPayment::fillCashboxList(const QVector<QPair<int, QString>> &cashboxes,
                                          const QVector<int> &checkedIds,
                                          bool checkAllIfEmpty)
{
    ui->lwCashboxes->clear();
    const QSet<int> checked(checkedIds.begin(), checkedIds.end());
    const bool selectAll = checkAllIfEmpty && checkedIds.isEmpty();
    for (const auto &cb : cashboxes) {
        if (cb.first <= 0) {
            continue;
        }
        auto *item = new QListWidgetItem(cb.second, ui->lwCashboxes);
        item->setData(Qt::UserRole, cb.first);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        const bool on = selectAll || checked.contains(cb.first);
        item->setCheckState(on ? Qt::Checked : Qt::Unchecked);
    }
}

void DlgStoreInputPayment::setOptions(const StorePaymentOptions &options)
{
    ui->chkUnpaid->setChecked(options.unpaidEnabled);
    ui->chkCard->setChecked(options.cardEnabled);
    ui->chkBank->setChecked(options.bankEnabled);
    if (options.cardCashboxId > 0) {
        ui->wCardCashbox->setCodeAndName(options.cardCashboxId, options.cardCashboxName);
    } else {
        ui->wCardCashbox->setCodeAndName(0, QString());
    }
    if (options.bankCashboxId > 0) {
        ui->wBankCashbox->setCodeAndName(options.bankCashboxId, options.bankCashboxName);
    } else {
        ui->wBankCashbox->setCodeAndName(0, QString());
    }
    ui->chkBank2->setChecked(options.bank2Enabled);
    ui->wBank2Cashbox->setCodeAndName(options.bank2CashboxId > 0 ? options.bank2CashboxId : 0,
                                      options.bank2CashboxId > 0 ? options.bank2CashboxName : QString());
    ui->chkBank3->setChecked(options.bank3Enabled);
    ui->wBank3Cashbox->setCodeAndName(options.bank3CashboxId > 0 ? options.bank3CashboxId : 0,
                                      options.bank3CashboxId > 0 ? options.bank3CashboxName : QString());

    QVector<QPair<int, QString>> existing;
    for (int i = 0; i < ui->lwCashboxes->count(); ++i) {
        QListWidgetItem *item = ui->lwCashboxes->item(i);
        existing.append({item->data(Qt::UserRole).toInt(), item->text()});
    }
    fillCashboxList(existing,
                    options.enabledCashboxIds,
                    !options.cashboxesConfigured);
    syncCashboxEnabled();
}

StorePaymentOptions DlgStoreInputPayment::options() const
{
    StorePaymentOptions o;
    o.unpaidEnabled = ui->chkUnpaid->isChecked();
    o.cardEnabled = ui->chkCard->isChecked();
    o.bankEnabled = ui->chkBank->isChecked();
    o.cardCashboxId = ui->wCardCashbox->value();
    o.cardCashboxName = ui->wCardCashbox->name();
    o.bankCashboxId = ui->wBankCashbox->value();
    o.bankCashboxName = ui->wBankCashbox->name();
    o.bank2Enabled = ui->chkBank2->isChecked();
    o.bank2CashboxId = ui->wBank2Cashbox->value();
    o.bank2CashboxName = ui->wBank2Cashbox->name();
    o.bank3Enabled = ui->chkBank3->isChecked();
    o.bank3CashboxId = ui->wBank3Cashbox->value();
    o.bank3CashboxName = ui->wBank3Cashbox->name();
    o.cashboxesConfigured = true;
    for (int i = 0; i < ui->lwCashboxes->count(); ++i) {
        QListWidgetItem *item = ui->lwCashboxes->item(i);
        if (item->checkState() == Qt::Checked) {
            o.enabledCashboxIds.append(item->data(Qt::UserRole).toInt());
        }
    }
    return o;
}

bool DlgStoreInputPayment::configure(StorePaymentOptions &options,
                                     const QVector<QPair<int, QString>> &cashboxes,
                                     QWidget *parent,
                                     const QString &unpaidText,
                                     const QString &hintText)
{
    DlgStoreInputPayment dlg(parent);
    if (!unpaidText.isEmpty()) {
        dlg.ui->chkUnpaid->setText(unpaidText);
    }
    if (!hintText.isEmpty()) {
        dlg.ui->lbHint->setText(hintText);
    }
    dlg.setCashboxes(cashboxes);
    dlg.setOptions(options);
    if (dlg.exec() != QDialog::Accepted) {
        return false;
    }
    const QString group = options.settingsGroup;
    options = dlg.options();
    options.settingsGroup = group;
    options.save();
    return true;
}

void DlgStoreInputPayment::syncCashboxEnabled()
{
    ui->wCardCashbox->setEnabled(ui->chkCard->isChecked());
    ui->wBankCashbox->setEnabled(ui->chkBank->isChecked());
    ui->wBank2Cashbox->setEnabled(ui->chkBank2->isChecked());
    ui->wBank3Cashbox->setEnabled(ui->chkBank3->isChecked());
}

void DlgStoreInputPayment::tryAccept()
{
    int checkedCash = 0;
    for (int i = 0; i < ui->lwCashboxes->count(); ++i) {
        if (ui->lwCashboxes->item(i)->checkState() == Qt::Checked) {
            ++checkedCash;
        }
    }
    if (!ui->chkUnpaid->isChecked()
        && checkedCash == 0
        && !ui->chkCard->isChecked()
        && !ui->chkBank->isChecked()
        && !ui->chkBank2->isChecked()
        && !ui->chkBank3->isChecked()) {
        C5Message::error(tr("Enable at least one payment method"));
        return;
    }
    if ((ui->chkCard->isChecked() && ui->wCardCashbox->value() <= 0)
        || (ui->chkBank->isChecked() && ui->wBankCashbox->value() <= 0)
        || (ui->chkBank2->isChecked() && ui->wBank2Cashbox->value() <= 0)
        || (ui->chkBank3->isChecked() && ui->wBank3Cashbox->value() <= 0)) {
        C5Message::error(tr("Cashbox not selected"));
        return;
    }
    accept();
}
