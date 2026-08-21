#include "dlgstoreinputpayment.h"
#include "ui_dlgstoreinputpayment.h"
#include "c5codenameselectorfunctions.h"
#include "c5message.h"
#include "dict_payment_type.h"
#include <QCoreApplication>
#include <QDialogButtonBox>
#include <QListWidgetItem>
#include <QSignalBlocker>

QString StorePaymentPreset::label() const
{
    if (isUnpaid()) {
        return QString();
    }
    return QStringLiteral("%1 / %2 / %3").arg(cashboxName, paymentTypeName, currencyName);
}

bool StorePaymentPreset::matches(const StorePaymentPreset &o) const
{
    return cashboxId == o.cashboxId
           && paymentTypeId == o.paymentTypeId
           && currencyId == o.currencyId;
}

DlgStoreInputPayment::DlgStoreInputPayment(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgStoreInputPayment)
{
    ui->setupUi(this);
    ui->wCashbox->selectorCallback = cashboxItemSelector;
    ui->wCurrency->selectorCallback = currencyItemSelector;
    ui->wCurrency->setCodeAndName(1, tr("Armenian dram"));

    for (int pt : payment_types) {
        const char *nm = payment_names.value(pt);
        if (!nm) {
            continue;
        }
        ui->cbPaymentType->addItem(QCoreApplication::translate("PaymentType", nm), pt);
    }
    const int cashIdx = ui->cbPaymentType->findData(PAYMENT_TYPE_CASH);
    if (cashIdx >= 0) {
        ui->cbPaymentType->setCurrentIndex(cashIdx);
    }

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &DlgStoreInputPayment::tryAccept);
    connect(ui->btnAddPreset, &QPushButton::clicked, this, &DlgStoreInputPayment::onAddPreset);
    connect(ui->btnDeletePreset, &QPushButton::clicked, this, &DlgStoreInputPayment::onDeletePreset);
    connect(ui->lwPresets, &QListWidget::currentRowChanged, this, &DlgStoreInputPayment::onPresetSelectionChanged);
    connect(ui->lwPresets, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *) {
        tryAccept();
    });
}

DlgStoreInputPayment::~DlgStoreInputPayment()
{
    delete ui;
}

void DlgStoreInputPayment::setPresets(const QVector<StorePaymentPreset> &presets, const StorePaymentPreset &selected)
{
    mPresets = presets;
    int idx = findPresetIndex(selected);
    if (idx < 0 && !selected.isUnpaid()) {
        mPresets.append(selected);
        idx = mPresets.size() - 1;
    }
    rebuildList(idx);
    if (idx >= 0) {
        writeForm(mPresets.at(idx));
    } else if (!selected.isUnpaid()) {
        writeForm(selected);
    } else if (!mPresets.isEmpty()) {
        writeForm(mPresets.first());
    } else {
        writeForm(StorePaymentPreset());
        ui->wCurrency->setCodeAndName(1, tr("Armenian dram"));
        const int cashIdx = ui->cbPaymentType->findData(PAYMENT_TYPE_CASH);
        if (cashIdx >= 0) {
            ui->cbPaymentType->setCurrentIndex(cashIdx);
        }
    }
}

QVector<StorePaymentPreset> DlgStoreInputPayment::presets() const
{
    return mPresets;
}

StorePaymentPreset DlgStoreInputPayment::selectedPreset() const
{
    StorePaymentPreset p;
    if (!readForm(&p, false)) {
        const int row = ui->lwPresets->currentRow();
        if (row >= 0 && row < mPresets.size()) {
            return mPresets.at(row);
        }
        return StorePaymentPreset();
    }
    return p;
}

bool DlgStoreInputPayment::edit(QVector<StorePaymentPreset> &presets, StorePaymentPreset &selected, QWidget *parent)
{
    DlgStoreInputPayment dlg(parent);
    dlg.setPresets(presets, selected);
    if (dlg.exec() != QDialog::Accepted) {
        return false;
    }
    presets = dlg.presets();
    selected = dlg.selectedPreset();
    return true;
}

bool DlgStoreInputPayment::readForm(StorePaymentPreset *out, bool showErrors) const
{
    StorePaymentPreset p;
    p.cashboxId = ui->wCashbox->value();
    p.cashboxName = ui->wCashbox->name();
    p.currencyId = ui->wCurrency->value();
    p.currencyName = ui->wCurrency->name();
    p.paymentTypeId = ui->cbPaymentType->currentData().toInt();
    p.paymentTypeName = ui->cbPaymentType->currentText();

    if (p.cashboxId <= 0) {
        if (showErrors) {
            C5Message::error(tr("Cashbox not selected"));
        }
        return false;
    }
    if (p.paymentTypeId <= 0) {
        if (showErrors) {
            C5Message::error(tr("Payment type not specified"));
        }
        return false;
    }
    if (p.currencyId <= 0) {
        if (showErrors) {
            C5Message::error(tr("Currency not selected"));
        }
        return false;
    }
    if (out) {
        *out = p;
    }
    return true;
}

void DlgStoreInputPayment::writeForm(const StorePaymentPreset &preset)
{
    if (preset.cashboxId > 0) {
        ui->wCashbox->setCodeAndName(preset.cashboxId, preset.cashboxName);
    } else {
        ui->wCashbox->setCodeAndName(0, QString());
    }
    if (preset.currencyId > 0) {
        ui->wCurrency->setCodeAndName(preset.currencyId, preset.currencyName);
    } else {
        ui->wCurrency->setCodeAndName(1, tr("Armenian dram"));
    }
    if (preset.paymentTypeId > 0) {
        const int idx = ui->cbPaymentType->findData(preset.paymentTypeId);
        if (idx >= 0) {
            ui->cbPaymentType->setCurrentIndex(idx);
        }
    }
}

void DlgStoreInputPayment::rebuildList(int selectIndex)
{
    const QSignalBlocker blocker(ui->lwPresets);
    ui->lwPresets->clear();
    for (const StorePaymentPreset &p : mPresets) {
        ui->lwPresets->addItem(p.label());
    }
    if (selectIndex >= 0 && selectIndex < mPresets.size()) {
        ui->lwPresets->setCurrentRow(selectIndex);
    } else if (!mPresets.isEmpty()) {
        ui->lwPresets->setCurrentRow(0);
    }
    ui->btnDeletePreset->setEnabled(ui->lwPresets->currentRow() >= 0);
}

int DlgStoreInputPayment::findPresetIndex(const StorePaymentPreset &preset) const
{
    if (preset.isUnpaid()) {
        return -1;
    }
    for (int i = 0; i < mPresets.size(); ++i) {
        if (mPresets.at(i).matches(preset)) {
            return i;
        }
    }
    return -1;
}

void DlgStoreInputPayment::onAddPreset()
{
    StorePaymentPreset p;
    if (!readForm(&p, true)) {
        return;
    }
    const int existing = findPresetIndex(p);
    if (existing >= 0) {
        mPresets[existing] = p;
        rebuildList(existing);
        return;
    }
    mPresets.append(p);
    rebuildList(mPresets.size() - 1);
}

void DlgStoreInputPayment::onDeletePreset()
{
    const int row = ui->lwPresets->currentRow();
    if (row < 0 || row >= mPresets.size()) {
        return;
    }
    if (C5Message::question(tr("Delete selected payment variant?")) != QDialog::Accepted) {
        return;
    }
    mPresets.removeAt(row);
    int next = row;
    if (next >= mPresets.size()) {
        next = mPresets.size() - 1;
    }
    rebuildList(next);
    if (next >= 0) {
        writeForm(mPresets.at(next));
    } else {
        writeForm(StorePaymentPreset());
        ui->wCurrency->setCodeAndName(1, tr("Armenian dram"));
    }
}

void DlgStoreInputPayment::onPresetSelectionChanged()
{
    const int row = ui->lwPresets->currentRow();
    ui->btnDeletePreset->setEnabled(row >= 0);
    if (row < 0 || row >= mPresets.size()) {
        return;
    }
    writeForm(mPresets.at(row));
}

void DlgStoreInputPayment::tryAccept()
{
    StorePaymentPreset p;
    if (!readForm(&p, true)) {
        return;
    }
    const int existing = findPresetIndex(p);
    if (existing >= 0) {
        mPresets[existing] = p;
        rebuildList(existing);
    } else {
        mPresets.append(p);
        rebuildList(mPresets.size() - 1);
    }
    accept();
}
