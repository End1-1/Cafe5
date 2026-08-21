#include "dlgstoreinputpricewarn.h"
#include "ui_dlgstoreinputpricewarn.h"

DlgStoreInputPriceWarn::DlgStoreInputPriceWarn(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgStoreInputPriceWarn)
{
    ui->setupUi(this);
}

DlgStoreInputPriceWarn::~DlgStoreInputPriceWarn()
{
    delete ui;
}

void DlgStoreInputPriceWarn::setMode(int mode)
{
    switch(mode) {
    case StoreInputPriceWarnOff:
        ui->rbOff->setChecked(true);
        break;
    case StoreInputPriceWarnOnFocus:
        ui->rbOnFocus->setChecked(true);
        break;
    case StoreInputPriceWarnOnSave:
    default:
        ui->rbOnSave->setChecked(true);
        break;
    }
}

void DlgStoreInputPriceWarn::setPercent(int percent)
{
    ui->sbPercent->setValue(qBound(1, percent, 100));
}

void DlgStoreInputPriceWarn::setAllowZeroPrice(bool allow)
{
    if(allow) {
        ui->rbZeroAllow->setChecked(true);
    } else {
        ui->rbZeroForbid->setChecked(true);
    }
}

int DlgStoreInputPriceWarn::mode() const
{
    if(ui->rbOff->isChecked()) {
        return StoreInputPriceWarnOff;
    }
    if(ui->rbOnFocus->isChecked()) {
        return StoreInputPriceWarnOnFocus;
    }
    return StoreInputPriceWarnOnSave;
}

int DlgStoreInputPriceWarn::percent() const
{
    return ui->sbPercent->value();
}

bool DlgStoreInputPriceWarn::allowZeroPrice() const
{
    return ui->rbZeroAllow->isChecked();
}

bool DlgStoreInputPriceWarn::edit(int &mode, int &percent, bool &allowZeroPrice, QWidget *parent)
{
    DlgStoreInputPriceWarn dlg(parent);
    dlg.setMode(mode);
    dlg.setPercent(percent);
    dlg.setAllowZeroPrice(allowZeroPrice);
    if(dlg.exec() != QDialog::Accepted) {
        return false;
    }
    mode = dlg.mode();
    percent = dlg.percent();
    allowZeroPrice = dlg.allowZeroPrice();
    return true;
}
