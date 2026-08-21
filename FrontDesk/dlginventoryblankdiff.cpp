#include "dlginventoryblankdiff.h"
#include "ui_dlginventoryblankdiff.h"

#include <QJsonObject>

DlgInventoryBlankDiff::DlgInventoryBlankDiff(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgInventoryBlankDiff)
{
    ui->setupUi(this);
    connect(ui->btnApplyAll, &QPushButton::clicked, this, &DlgInventoryBlankDiff::onApply);
    connect(ui->btnRemoveObsolete, &QPushButton::clicked, this, &DlgInventoryBlankDiff::onRemoveOnly);
    connect(ui->btnAddNew, &QPushButton::clicked, this, &DlgInventoryBlankDiff::onAddOnly);
}

DlgInventoryBlankDiff::~DlgInventoryBlankDiff()
{
    delete ui;
}

void DlgInventoryBlankDiff::setDiff(const QJsonArray &removed, const QJsonArray &added)
{
    mRemoved = removed;
    mAdded = added;
    ui->lwRemoved->clear();
    ui->lwAdded->clear();
    for(const QJsonValue &v : removed) {
        const QJsonObject o = v.toObject();
        const QString group = o.value(QStringLiteral("group_name")).toString();
        const QString name = o.value(QStringLiteral("name")).toString();
        const QString code = o.value(QStringLiteral("code")).toString();
        const QString text = group.isEmpty()
                                 ? QStringLiteral("%1 (%2)").arg(name, code)
                                 : QStringLiteral("[%1] %2 (%3)").arg(group, name, code);
        ui->lwRemoved->addItem(text);
    }
    for(const QJsonValue &v : added) {
        const QJsonObject o = v.toObject();
        const QString group = o.value(QStringLiteral("group_name")).toString();
        const QString name = o.value(QStringLiteral("name")).toString();
        const QString code = o.value(QStringLiteral("code")).toString();
        const QString text = group.isEmpty()
                                 ? QStringLiteral("%1 (%2)").arg(name, code)
                                 : QStringLiteral("[%1] %2 (%3)").arg(group, name, code);
        ui->lwAdded->addItem(text);
    }
    ui->btnRemoveObsolete->setEnabled(!removed.isEmpty());
    ui->btnAddNew->setEnabled(!added.isEmpty());
    ui->btnApplyAll->setEnabled(!removed.isEmpty() || !added.isEmpty());
    ui->lbRemoved->setText(tr("No longer in recipes (%1)").arg(removed.size()));
    ui->lbAdded->setText(tr("New in recipes (%1)").arg(added.size()));
}

void DlgInventoryBlankDiff::onApply()
{
    mRemoveObsolete = !mRemoved.isEmpty();
    mAddNew = !mAdded.isEmpty();
    accept();
}

void DlgInventoryBlankDiff::onRemoveOnly()
{
    mRemoveObsolete = true;
    mAddNew = false;
    accept();
}

void DlgInventoryBlankDiff::onAddOnly()
{
    mRemoveObsolete = false;
    mAddNew = true;
    accept();
}

bool DlgInventoryBlankDiff::showDiff(const QJsonArray &removed,
                                     const QJsonArray &added,
                                     bool &doRemove,
                                     bool &doAdd,
                                     QWidget *parent)
{
    doRemove = false;
    doAdd = false;
    if(removed.isEmpty() && added.isEmpty()) {
        return false;
    }
    DlgInventoryBlankDiff dlg(parent);
    dlg.setDiff(removed, added);
    if(dlg.exec() != QDialog::Accepted) {
        return false;
    }
    doRemove = dlg.removeObsolete();
    doAdd = dlg.addNew();
    return doRemove || doAdd;
}
