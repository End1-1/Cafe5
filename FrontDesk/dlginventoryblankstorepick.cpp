#include "dlginventoryblankstorepick.h"
#include "ui_dlginventoryblankstorepick.h"

#include <QJsonObject>

DlgInventoryBlankStorePick::DlgInventoryBlankStorePick(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgInventoryBlankStorePick)
{
    ui->setupUi(this);
}

DlgInventoryBlankStorePick::~DlgInventoryBlankStorePick()
{
    delete ui;
}

void DlgInventoryBlankStorePick::setStores(const QJsonArray &stores)
{
    ui->cbStore->clear();
    ui->cbStore->addItem(tr("All stores (one common blank)"), -1);
    for(const QJsonValue &v : stores) {
        const QJsonObject o = v.toObject();
        const int id = o.value(QStringLiteral("store_id")).toInt();
        QString name = o.value(QStringLiteral("store_name")).toString().trimmed();
        if(name.isEmpty()) {
            name = tr("Store #%1").arg(id);
        }
        ui->cbStore->addItem(name, id);
    }
    ui->cbStore->setCurrentIndex(0);
}

int DlgInventoryBlankStorePick::selectedStoreId() const
{
    return ui->cbStore->currentData().toInt();
}

bool DlgInventoryBlankStorePick::isCommonAll() const
{
    return selectedStoreId() < 0;
}

bool DlgInventoryBlankStorePick::pick(const QJsonArray &stores, int &storeId, bool &commonAll, QWidget *parent)
{
    DlgInventoryBlankStorePick dlg(parent);
    dlg.setStores(stores);
    if(dlg.exec() != QDialog::Accepted) {
        return false;
    }
    storeId = dlg.selectedStoreId();
    commonAll = dlg.isCommonAll();
    return true;
}
