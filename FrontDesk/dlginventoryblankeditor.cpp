#include "dlginventoryblankeditor.h"
#include "ui_dlginventoryblankeditor.h"

#include "c5htmlprint.h"
#include "c5message.h"
#include "c5utils.h"
#include "dlginventoryblankdiff.h"
#include "ninterface.h"

#include <QAbstractItemView>
#include <QCoreApplication>
#include <QEvent>
#include <QHeaderView>
#include <QJsonArray>
#include <QMap>
#include <QPageSize>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QSet>
#include <QTableWidgetItem>
#include <QTextDocument>
#include <QTimer>
#include <QVector>
#include <algorithm>

namespace {

enum {
    ColStore = 0,
    ColGroup,
    ColCode,
    ColName,
    ColUnit,
    ColSku
};

QString buildInventoryBlankHtml(const QJsonObject &blank, bool showBalances)
{
    QString html = loadTemplate(QStringLiteral("inventory_blank_a4.html"));
    if(html.isEmpty()) {
        return {};
    }

    const int colSpan = showBalances ? 6 : 5;
    QStringList rows;
    const QJsonArray stores = blank.value(QStringLiteral("stores")).toArray();
    for(const QJsonValue &sv : stores) {
        const QJsonObject store = sv.toObject();
        const QString storeName = store.value(QStringLiteral("store_name")).toString();
        if(!storeName.isEmpty()) {
            rows.append(QStringLiteral(
                            "<tr class='store'><td colspan='%1'><b>%2</b></td></tr>")
                            .arg(colSpan)
                            .arg(htmlEscape(storeName)));
        }

        QVector<QJsonObject> items;
        const QJsonArray rawItems = store.value(QStringLiteral("items")).toArray();
        items.reserve(rawItems.size());
        for(const QJsonValue &iv : rawItems) {
            const QJsonObject item = iv.toObject();
            if(item.value(QStringLiteral("name")).toString().trimmed().isEmpty()) {
                continue;
            }
            items.append(item);
        }
        std::sort(items.begin(), items.end(), [](const QJsonObject &a, const QJsonObject &b) {
            const int g = QString::compare(a.value(QStringLiteral("group_name")).toString(),
                                           b.value(QStringLiteral("group_name")).toString(),
                                           Qt::CaseInsensitive);
            if(g != 0) {
                return g < 0;
            }
            return QString::compare(a.value(QStringLiteral("name")).toString(),
                                    b.value(QStringLiteral("name")).toString(),
                                    Qt::CaseInsensitive) < 0;
        });

        QString lastGroup;
        bool hasGroupHeader = false;
        for(const QJsonObject &item : items) {
            const QString groupName = item.value(QStringLiteral("group_name")).toString().trimmed();
            if(groupName != lastGroup) {
                lastGroup = groupName;
                if(!groupName.isEmpty()) {
                    rows.append(QStringLiteral(
                                    "<tr class='group'><td colspan='%1'>%2</td></tr>")
                                    .arg(colSpan)
                                    .arg(htmlEscape(groupName)));
                    hasGroupHeader = true;
                } else if(hasGroupHeader) {
                    rows.append(QStringLiteral(
                                    "<tr class='group'><td colspan='%1'>&nbsp;</td></tr>")
                                    .arg(colSpan));
                }
            }

            QString balanceCell;
            if(showBalances) {
                QString bal = item.value(QStringLiteral("qty")).toVariant().toString().trimmed();
                if(bal.isEmpty()) {
                    bal = item.value(QStringLiteral("qty_sys")).toVariant().toString().trimmed();
                }
                if(!bal.isEmpty()) {
                    bal = float_str(str_float(bal), 3);
                }
                balanceCell = QStringLiteral("<td class='bal'>%1</td>").arg(htmlEscape(bal));
            }

            rows.append(QStringLiteral(
                            "<tr>"
                            "<td class='code'>%1</td>"
                            "<td class='name'>%2</td>"
                            "%3"
                            "<td class='qty'></td>"
                            "<td class='unit'>%4</td>"
                            "<td class='sku'>%5</td>"
                            "</tr>")
                            .arg(htmlEscape(item.value(QStringLiteral("code")).toString()),
                                 htmlEscape(item.value(QStringLiteral("name")).toString()),
                                 balanceCell,
                                 htmlEscape(item.value(QStringLiteral("unit")).toString()),
                                 htmlEscape(item.value(QStringLiteral("sku")).toString())));
        }
    }

    for(int i = 0; i < 10; ++i) {
        QString balanceCell;
        if(showBalances) {
            balanceCell = QStringLiteral("<td class='bal'></td>");
        }
        rows.append(QStringLiteral(
                        "<tr>"
                        "<td class='code'></td>"
                        "<td class='name'></td>"
                        "%1"
                        "<td class='qty'></td>"
                        "<td class='unit'></td>"
                        "<td class='sku'></td>"
                        "</tr>")
                        .arg(balanceCell));
    }

    QString balanceHeader;
    if(showBalances) {
        balanceHeader = QStringLiteral("<th class='bal'>%1</th>")
                            .arg(htmlEscape(QCoreApplication::translate("DlgInventoryBlankEditor", "Balance")));
    }

    QMap<QString, QString> vars;
    vars.insert(QStringLiteral("title"), htmlEscape(blank.value(QStringLiteral("title")).toString()));
    vars.insert(QStringLiteral("created"), htmlEscape(blank.value(QStringLiteral("created")).toString()));
    vars.insert(QStringLiteral("col_code"),
                htmlEscape(QCoreApplication::translate("DlgInventoryBlankEditor", "Code")));
    vars.insert(QStringLiteral("col_name"),
                htmlEscape(QCoreApplication::translate("DlgInventoryBlankEditor", "Name")));
    vars.insert(QStringLiteral("col_qty"),
                htmlEscape(QCoreApplication::translate("DlgInventoryBlankEditor", "Qty")));
    vars.insert(QStringLiteral("col_unit"),
                htmlEscape(QCoreApplication::translate("DlgInventoryBlankEditor", "Unit")));
    vars.insert(QStringLiteral("col_sku"),
                htmlEscape(QCoreApplication::translate("DlgInventoryBlankEditor", "SKU")));
    vars.insert(QStringLiteral("balance_header"), balanceHeader);
    vars.insert(QStringLiteral("rows"), rows.join(QString()));
    return applyTemplate(html, vars);
}

} // namespace

DlgInventoryBlankEditor::DlgInventoryBlankEditor(const QString &sessionKey,
                                                 const QJsonObject &reportFilter,
                                                 QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgInventoryBlankEditor)
    , mSessionKey(sessionKey)
    , mReportFilter(reportFilter)
{
    ui->setupUi(this);
    ui->tblItems->setColumnCount(6);
    ui->tblItems->setHorizontalHeaderLabels({
        tr("Store"), tr("Group"), tr("Code"), tr("Name"), tr("Unit"), tr("SKU")
    });
    ui->tblItems->horizontalHeader()->setStretchLastSection(true);
    ui->tblItems->horizontalHeader()->setSectionResizeMode(ColName, QHeaderView::Stretch);
    ui->tblItems->setDragDropMode(QAbstractItemView::InternalMove);
    ui->tblItems->setDragEnabled(true);
    ui->tblItems->setAcceptDrops(true);
    ui->tblItems->setDropIndicatorShown(true);
    ui->tblItems->setDefaultDropAction(Qt::MoveAction);
    // InternalMove leaves blank rows behind; strip them after each drop.
    ui->tblItems->viewport()->installEventFilter(this);
    connect(ui->btnAdd, &QPushButton::clicked, this, &DlgInventoryBlankEditor::onAddRow);
    connect(ui->btnRemove, &QPushButton::clicked, this, &DlgInventoryBlankEditor::onRemoveRow);
    connect(ui->btnRefresh, &QPushButton::clicked, this, &DlgInventoryBlankEditor::onRefresh);
    connect(ui->btnSave, &QPushButton::clicked, this, &DlgInventoryBlankEditor::onSave);
}

bool DlgInventoryBlankEditor::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->tblItems->viewport() && event->type() == QEvent::Drop) {
        QTimer::singleShot(0, this, &DlgInventoryBlankEditor::cleanupEmptyRows);
    }
    return QDialog::eventFilter(watched, event);
}

void DlgInventoryBlankEditor::cleanupEmptyRows()
{
    for(int r = ui->tblItems->rowCount() - 1; r >= 0; --r) {
        QTableWidgetItem *nameItem = ui->tblItems->item(r, ColName);
        const QString name = nameItem ? nameItem->text().trimmed() : QString();
        const int goodsId = nameItem ? nameItem->data(Qt::UserRole).toInt() : 0;
        // DnD InternalMove leaves abandoned rows (often with store still filled).
        if(name.isEmpty() && goodsId <= 0) {
            ui->tblItems->removeRow(r);
        }
    }
}

DlgInventoryBlankEditor::~DlgInventoryBlankEditor()
{
    delete ui;
}

QString DlgInventoryBlankEditor::itemKey(const QJsonObject &item)
{
    const int storeId = item.value(QStringLiteral("store_id")).toInt(0);
    const int goodsId = item.value(QStringLiteral("goods_id")).toInt(0);
    if(goodsId > 0) {
        return QStringLiteral("%1:%2").arg(storeId).arg(goodsId);
    }
    return QStringLiteral("%1:%2:%3")
        .arg(storeId)
        .arg(item.value(QStringLiteral("code")).toString())
        .arg(item.value(QStringLiteral("name")).toString());
}

bool DlgInventoryBlankEditor::isItemEmpty(const QJsonObject &item)
{
    return item.value(QStringLiteral("name")).toString().trimmed().isEmpty()
           && item.value(QStringLiteral("goods_id")).toInt(0) <= 0
           && item.value(QStringLiteral("code")).toString().trimmed().isEmpty();
}

void DlgInventoryBlankEditor::appendItemRow(const QJsonObject &item, const QString &storeName, int storeId)
{
    const int row = ui->tblItems->rowCount();
    ui->tblItems->insertRow(row);

    auto *storeItem = new QTableWidgetItem(storeName);
    storeItem->setData(Qt::UserRole, storeId);
    ui->tblItems->setItem(row, ColStore, storeItem);

    auto *groupItem = new QTableWidgetItem(item.value(QStringLiteral("group_name")).toString());
    groupItem->setData(Qt::UserRole, item.value(QStringLiteral("group_id")).toInt(0));
    ui->tblItems->setItem(row, ColGroup, groupItem);

    ui->tblItems->setItem(row, ColCode, new QTableWidgetItem(item.value(QStringLiteral("code")).toString()));

    auto *nameItem = new QTableWidgetItem(item.value(QStringLiteral("name")).toString());
    nameItem->setData(Qt::UserRole, item.value(QStringLiteral("goods_id")).toInt(0));
    ui->tblItems->setItem(row, ColName, nameItem);

    ui->tblItems->setItem(row, ColUnit, new QTableWidgetItem(item.value(QStringLiteral("unit")).toString()));
    ui->tblItems->setItem(row, ColSku, new QTableWidgetItem(item.value(QStringLiteral("sku")).toString()));
}

void DlgInventoryBlankEditor::setBlank(const QJsonObject &blank)
{
    mBlankId = blank.value(QStringLiteral("id")).toInt(0);
    mCreated = blank.value(QStringLiteral("created")).toString();
    ui->leTitle->setText(blank.value(QStringLiteral("title")).toString());
    ui->tblItems->setRowCount(0);

    const QJsonArray stores = blank.value(QStringLiteral("stores")).toArray();
    for(const QJsonValue &sv : stores) {
        const QJsonObject store = sv.toObject();
        const QString storeName = store.value(QStringLiteral("store_name")).toString();
        const int storeId = store.value(QStringLiteral("store_id")).toInt(0);
        const QJsonArray items = store.value(QStringLiteral("items")).toArray();
        for(const QJsonValue &iv : items) {
            const QJsonObject item = iv.toObject();
            if(isItemEmpty(item)) {
                continue;
            }
            appendItemRow(item, storeName, storeId);
        }
    }
    cleanupEmptyRows();
}

QJsonObject DlgInventoryBlankEditor::blank() const
{
    QMap<QString, QJsonObject> byStore;
    QStringList storeOrder;

    for(int r = 0; r < ui->tblItems->rowCount(); ++r) {
        const QString storeName = ui->tblItems->item(r, ColStore)
                                      ? ui->tblItems->item(r, ColStore)->text().trimmed()
                                      : QString();
        const QString name = ui->tblItems->item(r, ColName)
                                 ? ui->tblItems->item(r, ColName)->text().trimmed()
                                 : QString();
        if(name.isEmpty()) {
            continue;
        }
        const QString key = storeName;
        if(!byStore.contains(key)) {
            QJsonObject store;
            store.insert(QStringLiteral("store_id"),
                         ui->tblItems->item(r, ColStore)
                             ? ui->tblItems->item(r, ColStore)->data(Qt::UserRole).toInt()
                             : 0);
            store.insert(QStringLiteral("store_name"), storeName);
            store.insert(QStringLiteral("items"), QJsonArray());
            byStore.insert(key, store);
            storeOrder.append(key);
        }
        QJsonObject item;
        item.insert(QStringLiteral("goods_id"),
                    ui->tblItems->item(r, ColName)
                        ? ui->tblItems->item(r, ColName)->data(Qt::UserRole).toInt()
                        : 0);
        item.insert(QStringLiteral("group_id"),
                    ui->tblItems->item(r, ColGroup)
                        ? ui->tblItems->item(r, ColGroup)->data(Qt::UserRole).toInt()
                        : 0);
        item.insert(QStringLiteral("group_name"),
                    ui->tblItems->item(r, ColGroup) ? ui->tblItems->item(r, ColGroup)->text() : QString());
        item.insert(QStringLiteral("code"),
                    ui->tblItems->item(r, ColCode) ? ui->tblItems->item(r, ColCode)->text() : QString());
        item.insert(QStringLiteral("name"), name);
        item.insert(QStringLiteral("unit"),
                    ui->tblItems->item(r, ColUnit) ? ui->tblItems->item(r, ColUnit)->text() : QString());
        item.insert(QStringLiteral("sku"),
                    ui->tblItems->item(r, ColSku) ? ui->tblItems->item(r, ColSku)->text() : QString());

        QJsonObject store = byStore.value(key);
        QJsonArray items = store.value(QStringLiteral("items")).toArray();
        items.append(item);
        store.insert(QStringLiteral("items"), items);
        byStore.insert(key, store);
    }

    QJsonArray stores;
    for(const QString &key : storeOrder) {
        stores.append(byStore.value(key));
    }

    QJsonObject out;
    out.insert(QStringLiteral("id"), mBlankId);
    out.insert(QStringLiteral("title"), ui->leTitle->text().trimmed());
    out.insert(QStringLiteral("created"), mCreated);
    out.insert(QStringLiteral("stores"), stores);
    return out;
}

void DlgInventoryBlankEditor::fillBuildParams(QJsonObject &params) const
{
    params = mReportFilter;
    QSet<int> storeIds;
    for(int r = 0; r < ui->tblItems->rowCount(); ++r) {
        if(!ui->tblItems->item(r, ColStore)) {
            continue;
        }
        storeIds.insert(ui->tblItems->item(r, ColStore)->data(Qt::UserRole).toInt());
    }
    if(storeIds.isEmpty()) {
        params.insert(QStringLiteral("common_all"), 1);
        params.insert(QStringLiteral("store_id"), -1);
        return;
    }
    if(storeIds.size() == 1 && storeIds.contains(0)) {
        params.insert(QStringLiteral("common_all"), 1);
        params.insert(QStringLiteral("store_id"), -1);
        return;
    }
    if(storeIds.size() == 1) {
        params.insert(QStringLiteral("common_all"), 0);
        params.insert(QStringLiteral("store_id"), *storeIds.constBegin());
        return;
    }
    params.insert(QStringLiteral("common_all"), 0);
    params.insert(QStringLiteral("store_id"), 0);
}

bool DlgInventoryBlankEditor::edit(const QString &sessionKey,
                                   const QJsonObject &reportFilter,
                                   QJsonObject &blank,
                                   QWidget *parent)
{
    DlgInventoryBlankEditor dlg(sessionKey, reportFilter, parent);
    dlg.setBlank(blank);
    if(dlg.exec() != QDialog::Accepted) {
        return false;
    }
    blank = dlg.blank();
    return true;
}

void DlgInventoryBlankEditor::printBlank(const QJsonObject &blank, QWidget *parent)
{
    const int choice = C5Message::question(QCoreApplication::translate("DlgInventoryBlankEditor", "Print inventory blank"),
                                           QCoreApplication::translate("DlgInventoryBlankEditor", "With balances"),
                                           QCoreApplication::translate("DlgInventoryBlankEditor", "Cancel"),
                                           QCoreApplication::translate("DlgInventoryBlankEditor", "Without balances"));
    if(choice == QDialog::Rejected) {
        return;
    }
    const bool showBalances = (choice == QDialog::Accepted);

    const QString html = buildInventoryBlankHtml(blank, showBalances);
    if(html.isEmpty()) {
        C5Message::error(QCoreApplication::translate("DlgInventoryBlankEditor", "Template not found"));
        return;
    }
    QTextDocument doc;
    doc.setHtml(html);
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPageSize::A4);
    printer.setFullPage(false);
    QPrintPreviewDialog preview(&printer, parent);
    QObject::connect(&preview, &QPrintPreviewDialog::paintRequested, [&](QPrinter *p) {
        doc.print(p);
    });
    preview.exec();
}

void DlgInventoryBlankEditor::onAddRow()
{
    appendItemRow({}, QString(), 0);
    ui->tblItems->setCurrentCell(ui->tblItems->rowCount() - 1, ColName);
}

void DlgInventoryBlankEditor::onRemoveRow()
{
    const int row = ui->tblItems->currentRow();
    if(row >= 0) {
        ui->tblItems->removeRow(row);
    }
}

void DlgInventoryBlankEditor::onRefresh()
{
    QJsonObject buildParams;
    fillBuildParams(buildParams);
    NInterface::query1(QStringLiteral("/engine/v2/reports/inventory-blank/build"),
                       mSessionKey,
                       this,
                       buildParams,
                       [this](const QJsonObject &built) {
                           const QJsonObject recipeBlank = built.value(QStringLiteral("blank")).toObject();
                           QMap<QString, QJsonObject> recipeMap;
                           QMap<QString, QString> storeNames;
                           const QJsonArray recipeStores = recipeBlank.value(QStringLiteral("stores")).toArray();
                           for(const QJsonValue &sv : recipeStores) {
                               const QJsonObject store = sv.toObject();
                               const int storeId = store.value(QStringLiteral("store_id")).toInt(0);
                               const QString storeName = store.value(QStringLiteral("store_name")).toString();
                               storeNames.insert(QString::number(storeId), storeName);
                               for(const QJsonValue &iv : store.value(QStringLiteral("items")).toArray()) {
                                   QJsonObject item = iv.toObject();
                                   item.insert(QStringLiteral("store_id"), storeId);
                                   item.insert(QStringLiteral("store_name"), storeName);
                                   recipeMap.insert(itemKey(item), item);
                               }
                           }

                           QMap<QString, QJsonObject> blankMap;
                           for(int r = 0; r < ui->tblItems->rowCount(); ++r) {
                               const int storeId = ui->tblItems->item(r, ColStore)
                                                       ? ui->tblItems->item(r, ColStore)->data(Qt::UserRole).toInt()
                                                       : 0;
                               const QString storeName = ui->tblItems->item(r, ColStore)
                                                             ? ui->tblItems->item(r, ColStore)->text()
                                                             : QString();
                               QJsonObject item;
                               item.insert(QStringLiteral("store_id"), storeId);
                               item.insert(QStringLiteral("store_name"), storeName);
                               item.insert(QStringLiteral("goods_id"),
                                           ui->tblItems->item(r, ColName)
                                               ? ui->tblItems->item(r, ColName)->data(Qt::UserRole).toInt()
                                               : 0);
                               item.insert(QStringLiteral("group_id"),
                                           ui->tblItems->item(r, ColGroup)
                                               ? ui->tblItems->item(r, ColGroup)->data(Qt::UserRole).toInt()
                                               : 0);
                               item.insert(QStringLiteral("group_name"),
                                           ui->tblItems->item(r, ColGroup)
                                               ? ui->tblItems->item(r, ColGroup)->text()
                                               : QString());
                               item.insert(QStringLiteral("code"),
                                           ui->tblItems->item(r, ColCode)
                                               ? ui->tblItems->item(r, ColCode)->text()
                                               : QString());
                               item.insert(QStringLiteral("name"),
                                           ui->tblItems->item(r, ColName)
                                               ? ui->tblItems->item(r, ColName)->text()
                                               : QString());
                               item.insert(QStringLiteral("unit"),
                                           ui->tblItems->item(r, ColUnit)
                                               ? ui->tblItems->item(r, ColUnit)->text()
                                               : QString());
                               item.insert(QStringLiteral("sku"),
                                           ui->tblItems->item(r, ColSku)
                                               ? ui->tblItems->item(r, ColSku)->text()
                                               : QString());
                               if(item.value(QStringLiteral("name")).toString().trimmed().isEmpty()) {
                                   continue;
                               }
                               blankMap.insert(itemKey(item), item);
                           }

                           QJsonArray removed;
                           for(auto it = blankMap.constBegin(); it != blankMap.constEnd(); ++it) {
                               if(!recipeMap.contains(it.key())) {
                                   removed.append(it.value());
                               }
                           }
                           QJsonArray added;
                           for(auto it = recipeMap.constBegin(); it != recipeMap.constEnd(); ++it) {
                               if(!blankMap.contains(it.key())) {
                                   added.append(it.value());
                               }
                           }

                           if(removed.isEmpty() && added.isEmpty()) {
                               C5Message::info(tr("Blank matches recipes"));
                               return;
                           }

                           bool doRemove = false;
                           bool doAdd = false;
                           if(!DlgInventoryBlankDiff::showDiff(removed, added, doRemove, doAdd, this)) {
                               return;
                           }

                           if(doRemove) {
                               QSet<QString> removeKeys;
                               for(const QJsonValue &v : removed) {
                                   removeKeys.insert(itemKey(v.toObject()));
                               }
                               for(int r = ui->tblItems->rowCount() - 1; r >= 0; --r) {
                                   const int storeId = ui->tblItems->item(r, ColStore)
                                                           ? ui->tblItems->item(r, ColStore)->data(Qt::UserRole).toInt()
                                                           : 0;
                                   QJsonObject item;
                                   item.insert(QStringLiteral("store_id"), storeId);
                                   item.insert(QStringLiteral("goods_id"),
                                               ui->tblItems->item(r, ColName)
                                                   ? ui->tblItems->item(r, ColName)->data(Qt::UserRole).toInt()
                                                   : 0);
                                   item.insert(QStringLiteral("code"),
                                               ui->tblItems->item(r, ColCode)
                                                   ? ui->tblItems->item(r, ColCode)->text()
                                                   : QString());
                                   item.insert(QStringLiteral("name"),
                                               ui->tblItems->item(r, ColName)
                                                   ? ui->tblItems->item(r, ColName)->text()
                                                   : QString());
                                   if(removeKeys.contains(itemKey(item))) {
                                       ui->tblItems->removeRow(r);
                                   }
                               }
                           }

                           if(doAdd) {
                               for(const QJsonValue &v : added) {
                                   const QJsonObject item = v.toObject();
                                   appendItemRow(item,
                                                 item.value(QStringLiteral("store_name")).toString(),
                                                 item.value(QStringLiteral("store_id")).toInt(0));
                               }
                           }
                       });
}

void DlgInventoryBlankEditor::onSave()
{
    QJsonObject payload = blank();
    if(payload.value(QStringLiteral("title")).toString().trimmed().isEmpty()) {
        C5Message::error(tr("Enter title"));
        return;
    }

    NInterface::query1(QStringLiteral("/engine/v2/reports/inventory-blank/save"),
                       mSessionKey,
                       this,
                       payload,
                       [this](const QJsonObject &jo) {
                           const QJsonObject saved = jo.value(QStringLiteral("blank")).toObject();
                           mBlankId = saved.value(QStringLiteral("id")).toInt(mBlankId);
                           mCreated = saved.value(QStringLiteral("created")).toString(mCreated);
                           accept();
                       });
}
