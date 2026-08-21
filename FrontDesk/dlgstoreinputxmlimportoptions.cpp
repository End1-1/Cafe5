#include "dlgstoreinputxmlimportoptions.h"
#include "ui_dlgstoreinputxmlimportoptions.h"

#include "c5codenameselectorfunctions.h"
#include "c5message.h"

DlgStoreInputXmlImportOptions::DlgStoreInputXmlImportOptions(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgStoreInputXmlImportOptions)
{
    ui->setupUi(this);
    ui->wStore->selectorCallback = storeItemSelector;
}

DlgStoreInputXmlImportOptions::~DlgStoreInputXmlImportOptions()
{
    delete ui;
}

StoreInputXmlImportOptions DlgStoreInputXmlImportOptions::options() const
{
    StoreInputXmlImportOptions o;
    if (ui->rbGoodsAutoCreate->isChecked()) {
        o.goodsMissing = StoreInputXmlImportOptions::GoodsAutoCreate;
    } else {
        o.goodsMissing = StoreInputXmlImportOptions::GoodsMatch;
    }
    if (ui->rbPartnerDoNotCreate->isChecked()) {
        o.partnerMissing = StoreInputXmlImportOptions::PartnerDoNotCreate;
    } else {
        o.partnerMissing = StoreInputXmlImportOptions::PartnerAutoCreate;
    }
    if (ui->rbDupSkip->isChecked()) {
        o.duplicates = StoreInputXmlImportOptions::DupSkip;
    } else if (ui->rbDupAbort->isChecked()) {
        o.duplicates = StoreInputXmlImportOptions::DupAbort;
    } else {
        o.duplicates = StoreInputXmlImportOptions::DupOverwrite;
    }
    o.storeId = ui->wStore->value();
    o.storeName = ui->wStore->name();
    if (ui->rbSaveImmediateDraft->isChecked()) {
        o.saveMode = StoreInputXmlImportOptions::SaveImmediateDraft;
    } else if (ui->rbSaveImmediatePosted->isChecked()) {
        o.saveMode = StoreInputXmlImportOptions::SaveImmediatePosted;
    } else {
        o.saveMode = StoreInputXmlImportOptions::SaveOpenInWindows;
    }
    return o;
}

void DlgStoreInputXmlImportOptions::setOptions(const StoreInputXmlImportOptions &options)
{
    ui->rbGoodsAutoCreate->setChecked(options.goodsMissing == StoreInputXmlImportOptions::GoodsAutoCreate);
    ui->rbGoodsMatch->setChecked(options.goodsMissing == StoreInputXmlImportOptions::GoodsMatch);
    ui->rbPartnerAutoCreate->setChecked(options.partnerMissing == StoreInputXmlImportOptions::PartnerAutoCreate);
    ui->rbPartnerDoNotCreate->setChecked(options.partnerMissing == StoreInputXmlImportOptions::PartnerDoNotCreate);
    ui->rbDupOverwrite->setChecked(options.duplicates == StoreInputXmlImportOptions::DupOverwrite);
    ui->rbDupSkip->setChecked(options.duplicates == StoreInputXmlImportOptions::DupSkip);
    ui->rbDupAbort->setChecked(options.duplicates == StoreInputXmlImportOptions::DupAbort);
    ui->rbSaveOpenWindows->setChecked(options.saveMode == StoreInputXmlImportOptions::SaveOpenInWindows);
    ui->rbSaveImmediateDraft->setChecked(options.saveMode == StoreInputXmlImportOptions::SaveImmediateDraft);
    ui->rbSaveImmediatePosted->setChecked(options.saveMode == StoreInputXmlImportOptions::SaveImmediatePosted);
    if (options.storeId > 0) {
        ui->wStore->setCodeAndName(options.storeId, options.storeName);
    }
}

void DlgStoreInputXmlImportOptions::accept()
{
    if (ui->wStore->value() <= 0) {
        C5Message::error(tr("Select input store"));
        return;
    }
    QDialog::accept();
}

bool DlgStoreInputXmlImportOptions::edit(StoreInputXmlImportOptions &options, QWidget *parent)
{
    DlgStoreInputXmlImportOptions dlg(parent);
    dlg.setOptions(options);
    if (dlg.exec() != QDialog::Accepted) {
        return false;
    }
    options = dlg.options();
    return true;
}
