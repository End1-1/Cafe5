#include "rsolditemsreport.h"

#include <QJsonObject>
#include <QMenu>
#include <QShowEvent>

#include "c5mainwindow.h"
#include "c5message.h"
#include "c5user.h"
#include "dlgsolditemorders.h"
#include "ninterface.h"
#include "struct_waiter_order.h"
#include "ui_rabstracteditorreport.h"
#include "worderinspector.h"

RSoldItemsReport::RSoldItemsReport(const QString &title, QIcon icon, const QString &editorName,
                                   QWidget *parent)
    : RAbstractEditorReport(title, icon, editorName, parent)
{
}

void RSoldItemsReport::showEvent(QShowEvent *e)
{
    RAbstractEditorReport::showEvent(e);
    setupContextMenu();
}

void RSoldItemsReport::setupContextMenu()
{
    if (mContextMenuReady || !ui || !ui->tbl) {
        return;
    }
    mContextMenuReady = true;
    ui->tbl->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tbl, &QWidget::customContextMenuRequested, this, &RSoldItemsReport::showContextMenu);
}

void RSoldItemsReport::showContextMenu(const QPoint &pos)
{
    const QModelIndex index = ui->tbl->indexAt(pos);
    if (!index.isValid()) {
        return;
    }
    ui->tbl->setCurrentIndex(index);

    QMenu menu(this);
    QAction *act = menu.addAction(tr("View by orders"));
    connect(act, &QAction::triggered, this, &RSoldItemsReport::openSelectedItemByOrders);
    menu.exec(ui->tbl->viewport()->mapToGlobal(pos));
}

int RSoldItemsReport::selectedGoodsId(QString *itemName) const
{
    const QModelIndex viewIndex = ui->tbl->currentIndex();
    if (!viewIndex.isValid()) {
        return 0;
    }
    const QModelIndex srcIndex = reportMapViewIndexToSource(viewIndex);
    if (!srcIndex.isValid()) {
        return 0;
    }

    // Summarize: col0 = goods id. Detail: last col = goods id, col0 = order id.
    const int summarize = filterObject(QStringLiteral("summarize")).value(QStringLiteral("summarize")).toInt(1);
    int goodsCol = 0;
    if (summarize == 0) {
        goodsCol = reportColumnFromEnd(-1);
    }
    const int goodsId = reportSourceCellData(srcIndex.row(), goodsCol).toInt();
    if (itemName) {
        *itemName = reportSourceCellData(srcIndex.row(), 5).toString().trimmed();
    }
    return goodsId;
}

void RSoldItemsReport::currentPeriod(QString *date1, QString *date2, int *groupId, int *fiscalReceipt) const
{
    const QJsonObject d1 = filterObject(QStringLiteral("date1"));
    const QJsonObject d2 = filterObject(QStringLiteral("date2"));
    const QJsonObject gr = filterObject(QStringLiteral("group_id"));
    const QJsonObject fr = filterObject(QStringLiteral("fiscal_receipt"));
    if (date1) {
        *date1 = d1.value(QStringLiteral("date1")).toString();
    }
    if (date2) {
        *date2 = d2.value(QStringLiteral("date2")).toString();
    }
    if (groupId) {
        *groupId = gr.value(QStringLiteral("group_id")).toVariant().toInt();
    }
    if (fiscalReceipt) {
        *fiscalReceipt = fr.value(QStringLiteral("fiscal_receipt")).toVariant().toInt();
    }
}

void RSoldItemsReport::openSelectedItemByOrders()
{
    QString itemName;
    const int goodsId = selectedGoodsId(&itemName);
    if (goodsId <= 0) {
        C5Message::error(tr("Select an item"));
        return;
    }

    QString date1;
    QString date2;
    int groupId = 0;
    int fiscalReceipt = 0;
    currentPeriod(&date1, &date2, &groupId, &fiscalReceipt);
    if (date1.isEmpty() || date2.isEmpty()) {
        C5Message::error(tr("Set the period in the filter"));
        return;
    }

    auto *dlg = new DlgSoldItemOrders(mUser, goodsId, itemName, date1, date2, groupId, fiscalReceipt, this);
    dlg->show();
    dlg->raise();
    dlg->activateWindow();
}

void RSoldItemsReport::on_tbl_doubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }

    const int summarize = filterObject(QStringLiteral("summarize")).value(QStringLiteral("summarize")).toInt(1);
    if (summarize == 1) {
        // Summarized by item → open the by-orders window for this goods.
        ui->tbl->setCurrentIndex(index);
        openSelectedItemByOrders();
        return;
    }

    // Detail mode: open the order (col0 = order id).
    const QModelIndex srcIndex = reportMapViewIndexToSource(index);
    if (!srcIndex.isValid()) {
        return;
    }
    const QString orderId = reportSourceCellData(srcIndex.row(), 0).toString().trimmed();
    if (orderId.isEmpty()) {
        return;
    }

    NInterface::query1(QStringLiteral("/engine/v2/waiter/order/query-order"),
                       mUser->mSessionKey,
                       this,
                       {{QStringLiteral("id"), orderId}},
                       [this](const QJsonObject &jo) {
                           const QJsonObject orderJo = jo.value(QStringLiteral("order")).toObject();
                           if (orderJo.isEmpty()) {
                               C5Message::error(tr("Order not found"));
                               return;
                           }
                           WaiterOrder order = JsonParser<WaiterOrder>::fromJson(orderJo);
                           auto *sw = new WOrderInspector(mUser, tr("Order"), QIcon());
                           __mainWindow->addWidget(sw);
                           sw->setOrder(order);
                       });
}
