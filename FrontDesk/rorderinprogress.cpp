#include "rorderinprogress.h"
#include <QAbstractItemModel>
#include <QBrush>
#include <QColor>
#include <QHeaderView>
#include <QHideEvent>
#include <QJsonObject>
#include <QPalette>
#include <QShowEvent>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include "c5mainwindow.h"
#include "c5message.h"
#include "c5utils.h"
#include "ninterface.h"
#include "struct_waiter_order.h"
#include "worderinspector.h"

namespace
{

/** Must match orderinprogress.php ordersToReportRows column order. */
constexpr int kColOrderId = 0;
constexpr int kColStatusText = 13;
/** Hidden numeric min kitchen status for row background (from o_goods_process.f_status). */
constexpr int kColKitchenStatus = 14;

QColor kitchenRowBackground(int minLineStatus)
{
    switch(minLineStatus) {
    case 1:
        return QColor(QStringLiteral("#455a71"));
    case 2:
        return QColor(QStringLiteral("#b86228"));
    case 3:
        return QColor(QStringLiteral("#2d7a54"));
    case 4:
    case 6:
        return QColor(QStringLiteral("#54687a"));
    default:
        return QColor(QStringLiteral("#495963"));
    }
}

class KitchenInProgressStyleDelegate : public QStyledItemDelegate
{
public:
    explicit KitchenInProgressStyleDelegate(QObject *parent = nullptr)
        : QStyledItemDelegate(parent)
    {
    }

    void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const override
    {
        QStyledItemDelegate::initStyleOption(option, index);

        if(!index.isValid() || !index.model()) {
            return;
        }

        const QModelIndex stIdx = index.model()->index(index.row(), kColKitchenStatus, index.parent());

        if(!stIdx.isValid()) {
            return;
        }

        bool ok = false;
        int st = stIdx.data(Qt::DisplayRole).toInt(&ok);

        if(!ok) {
            return;
        }

        if(st == 6) {
            st = 4;
        }

        const QColor bg = kitchenRowBackground(st);
        const QColor fg(QStringLiteral("#f5f7fa"));
        option->backgroundBrush = QBrush(bg);
        option->palette.setBrush(QPalette::Base, QBrush(bg));
        option->palette.setColor(QPalette::Text, fg);
        option->palette.setColor(QPalette::HighlightedText, fg);
    }
};

} // namespace

ROrderInProgress::ROrderInProgress(const QString &title, QIcon icon, const QString &editorName)
    : RAbstractEditorReport(title, icon, editorName)
{
    mReloadTimer.setInterval(30000);
    mReloadTimer.setSingleShot(false);
    connect(&mReloadTimer, &QTimer::timeout, this, [this]() {
        if(isLiveQueueMode()) {
            getData();
        }
    });

    ui->tbl->setWordWrap(true);
    ui->tbl->setTextElideMode(Qt::ElideNone);
    ui->tbl->setItemDelegate(new KitchenInProgressStyleDelegate(ui->tbl));

    if(QHeaderView *vh = ui->tbl->verticalHeader()) {
        vh->setDefaultSectionSize(56);
        vh->setSectionResizeMode(QHeaderView::ResizeToContents);
    }

    if(QAbstractItemModel *mdl = ui->tbl->model()) {
        connect(mdl, &QAbstractItemModel::modelReset, this, &ROrderInProgress::onTableModelReset);
        connect(mdl, &QAbstractItemModel::layoutChanged, this, &ROrderInProgress::onTableModelReset);
    }
}

void ROrderInProgress::tuneReportTableLayout()
{
    if(!ui->tbl->model() || ui->tbl->model()->columnCount() <= kColStatusText) {
        return;
    }

    if(QHeaderView *hdr = ui->tbl->horizontalHeader()) {
        hdr->setSectionResizeMode(kColStatusText, QHeaderView::ResizeToContents);
    }

    const int statusW = ui->tbl->columnWidth(kColStatusText);
    if(statusW < 120) {
        ui->tbl->setColumnWidth(kColStatusText, 120);
    }
}

void ROrderInProgress::onTableModelReset()
{
    ui->tbl->resizeColumnsToContents();
    ui->tbl->resizeRowsToContents();
    tuneReportTableLayout();

    const int cols = ui->tbl->model() ? ui->tbl->model()->columnCount() : 0;
    ui->tblTotal->setColumnCount(cols);

    for(int i = 0; i < cols; ++i) {
        ui->tblTotal->setColumnWidth(i, ui->tbl->columnWidth(i));
    }
}

bool ROrderInProgress::isLiveQueueMode() const
{
    const QJsonObject modeObj = filterObject(QStringLiteral("mode"));
    return modeObj.value(QStringLiteral("mode")).toInt(0) == 0;
}

void ROrderInProgress::startAutoReload()
{
    if(isLiveQueueMode()) {
        mReloadTimer.start();
    }
}

void ROrderInProgress::stopAutoReload()
{
    mReloadTimer.stop();
}

void ROrderInProgress::showEvent(QShowEvent *e)
{
    RAbstractEditorReport::showEvent(e);
    startAutoReload();
}

void ROrderInProgress::hideEvent(QHideEvent *e)
{
    stopAutoReload();
    RAbstractEditorReport::hideEvent(e);
}

void ROrderInProgress::applyFilter()
{
    RAbstractEditorReport::applyFilter();
    stopAutoReload();
    startAutoReload();
}

void ROrderInProgress::openOrderInspector(const QString &orderId)
{
    NInterface::query1(QStringLiteral("/engine/v2/waiter/order/query-order"),
                       mUser->mSessionKey,
                       this,
                       {{QStringLiteral("id"), orderId}},
                       [this](const QJsonObject &jo) {
                           const QJsonObject orderJo = jo.value(QStringLiteral("order")).toObject();
                           if(orderJo.isEmpty()) {
                               C5Message::error(tr("Order not found"));
                               return;
                           }

                           WaiterOrder order = JsonParser<WaiterOrder>::fromJson(orderJo);
                           auto *sw = new WOrderInspector(mUser, tr("Order"), QIcon());
                           __mainWindow->addWidget(sw);
                           sw->setOrder(order);
                       });
}

void ROrderInProgress::on_tbl_doubleClicked(const QModelIndex &index)
{
    if(!index.isValid()) {
        return;
    }

    const QModelIndex srcIndex = reportMapViewIndexToSource(index);
    if(!srcIndex.isValid()) {
        return;
    }

    const QString orderId = reportSourceCellData(srcIndex.row(), kColOrderId).toString().trimmed();
    if(orderId.isEmpty()) {
        return;
    }

    openOrderInspector(orderId);
}
