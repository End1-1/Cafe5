#include "dlginventoryblankchooser.h"
#include "ui_dlginventoryblankchooser.h"

#include "c5message.h"
#include "dlginventoryblankeditor.h"
#include "dlginventoryblankstorepick.h"
#include "ninterface.h"

#include <QJsonArray>
#include <QListWidgetItem>

DlgInventoryBlankChooser::DlgInventoryBlankChooser(const QString &sessionKey,
                                                   const QJsonObject &reportFilter,
                                                   QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgInventoryBlankChooser)
    , mSessionKey(sessionKey)
    , mReportFilter(reportFilter)
{
    ui->setupUi(this);
    connect(ui->btnNew, &QPushButton::clicked, this, &DlgInventoryBlankChooser::onNew);
    connect(ui->btnEdit, &QPushButton::clicked, this, &DlgInventoryBlankChooser::onEdit);
    connect(ui->btnPrint, &QPushButton::clicked, this, &DlgInventoryBlankChooser::onPrint);
    connect(ui->btnDelete, &QPushButton::clicked, this, &DlgInventoryBlankChooser::onDelete);
    connect(ui->lwBlanks, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *) {
        onEdit();
    });
    reloadList();
}

DlgInventoryBlankChooser::~DlgInventoryBlankChooser()
{
    delete ui;
}

void DlgInventoryBlankChooser::open(const QString &sessionKey,
                                    const QJsonObject &reportFilter,
                                    QWidget *parent)
{
    DlgInventoryBlankChooser dlg(sessionKey, reportFilter, parent);
    dlg.exec();
}

void DlgInventoryBlankChooser::reloadList()
{
    NInterface::query1(QStringLiteral("/engine/v2/reports/inventory-blank/list"),
                       mSessionKey,
                       this,
                       {},
                       [this](const QJsonObject &jo) {
                           ui->lwBlanks->clear();
                           const QJsonArray items = jo.value(QStringLiteral("items")).toArray();
                           for(const QJsonValue &v : items) {
                               const QJsonObject o = v.toObject();
                               const QString title = o.value(QStringLiteral("title")).toString();
                               const QString created = o.value(QStringLiteral("created")).toString();
                               auto *item = new QListWidgetItem(
                                   created.isEmpty() ? title : QStringLiteral("%1 — %2").arg(title, created));
                               item->setData(Qt::UserRole, o.value(QStringLiteral("id")).toInt());
                               ui->lwBlanks->addItem(item);
                           }
                       });
}

int DlgInventoryBlankChooser::selectedId() const
{
    QListWidgetItem *item = ui->lwBlanks->currentItem();
    return item ? item->data(Qt::UserRole).toInt() : 0;
}

void DlgInventoryBlankChooser::onNew()
{
    QJsonObject params = mReportFilter;
    NInterface::query1(QStringLiteral("/engine/v2/reports/inventory-blank/stores"),
                       mSessionKey,
                       this,
                       params,
                       [this](const QJsonObject &jo) {
                           const QJsonArray stores = jo.value(QStringLiteral("stores")).toArray();
                           int storeId = -1;
                           bool commonAll = true;
                           if(!DlgInventoryBlankStorePick::pick(stores, storeId, commonAll, this)) {
                               return;
                           }
                           QJsonObject buildParams = mReportFilter;
                           buildParams.insert(QStringLiteral("common_all"), commonAll ? 1 : 0);
                           buildParams.insert(QStringLiteral("store_id"), commonAll ? -1 : storeId);
                           NInterface::query1(QStringLiteral("/engine/v2/reports/inventory-blank/build"),
                                              mSessionKey,
                                              this,
                                              buildParams,
                                              [this](const QJsonObject &built) {
                                                  QJsonObject blank = built.value(QStringLiteral("blank")).toObject();
                                                  if(DlgInventoryBlankEditor::edit(mSessionKey, mReportFilter, blank, this)) {
                                                      reloadList();
                                                  }
                                              });
                       });
}

void DlgInventoryBlankChooser::onEdit()
{
    const int id = selectedId();
    if(id <= 0) {
        C5Message::info(tr("Select a blank"));
        return;
    }
    loadAndEdit(id);
}

void DlgInventoryBlankChooser::onPrint()
{
    const int id = selectedId();
    if(id <= 0) {
        C5Message::info(tr("Select a blank"));
        return;
    }
    loadAndPrint(id);
}

void DlgInventoryBlankChooser::onDelete()
{
    const int id = selectedId();
    if(id <= 0) {
        C5Message::info(tr("Select a blank"));
        return;
    }
    if(C5Message::question(tr("Delete selected blank?")) != QDialog::Accepted) {
        return;
    }
    QJsonObject params;
    params.insert(QStringLiteral("id"), id);
    NInterface::query1(QStringLiteral("/engine/v2/reports/inventory-blank/remove"),
                       mSessionKey,
                       this,
                       params,
                       [this](const QJsonObject &) {
                           reloadList();
                       });
}

void DlgInventoryBlankChooser::loadAndEdit(int id)
{
    QJsonObject params;
    params.insert(QStringLiteral("id"), id);
    NInterface::query1(QStringLiteral("/engine/v2/reports/inventory-blank/get"),
                       mSessionKey,
                       this,
                       params,
                       [this](const QJsonObject &jo) {
                           QJsonObject blank = jo.value(QStringLiteral("blank")).toObject();
                           if(DlgInventoryBlankEditor::edit(mSessionKey, mReportFilter, blank, this)) {
                               reloadList();
                           }
                       });
}

void DlgInventoryBlankChooser::loadAndPrint(int id)
{
    QJsonObject params;
    params.insert(QStringLiteral("id"), id);
    NInterface::query1(QStringLiteral("/engine/v2/reports/inventory-blank/get"),
                       mSessionKey,
                       this,
                       params,
                       [this](const QJsonObject &jo) {
                           DlgInventoryBlankEditor::printBlank(
                               jo.value(QStringLiteral("blank")).toObject(), this);
                       });
}
