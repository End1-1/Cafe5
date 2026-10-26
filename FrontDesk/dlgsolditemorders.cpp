#include "dlgsolditemorders.h"
#include "ui_dlgsolditemorders.h"

#include <QHeaderView>
#include <QJsonArray>
#include <QJsonObject>
#include <QTableWidgetItem>

#include "c5mainwindow.h"
#include "c5message.h"
#include "c5user.h"
#include "ninterface.h"
#include "struct_waiter_order.h"
#include "worderinspector.h"

namespace {

QTableWidgetItem *makeItem(const QString &text, Qt::Alignment align = Qt::AlignLeft | Qt::AlignVCenter)
{
    auto *item = new QTableWidgetItem(text);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    item->setTextAlignment(align);
    return item;
}

} // namespace

DlgSoldItemOrders::DlgSoldItemOrders(C5User *user,
                                     int goodsId,
                                     const QString &goodsName,
                                     const QString &date1,
                                     const QString &date2,
                                     int groupId,
                                     int fiscalReceipt,
                                     QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgSoldItemOrders)
    , mUser(user)
    , mGoodsId(goodsId)
    , mGoodsName(goodsName)
    , mDate1(date1)
    , mDate2(date2)
    , mGroupId(groupId)
    , mFiscalReceipt(fiscalReceipt)
{
    ui->setupUi(this);
    setWindowTitle(tr("Orders") + QStringLiteral(": ") + mGoodsName);
    setWindowModality(Qt::NonModal);
    setAttribute(Qt::WA_DeleteOnClose);

    ui->tbl->setColumnHidden(0, true);
    ui->tbl->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->tbl->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->tbl->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    ui->tbl->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    ui->tbl->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    ui->tbl->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    ui->tbl->horizontalHeader()->setSectionResizeMode(7, QHeaderView::ResizeToContents);

    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(ui->tbl, &QTableWidget::cellDoubleClicked, this, &DlgSoldItemOrders::onTableDoubleClicked);

    loadData();
}

DlgSoldItemOrders::~DlgSoldItemOrders()
{
    delete ui;
}

void DlgSoldItemOrders::loadData()
{
    ui->lbHeader->setText(tr("Loading..."));
    ui->tbl->setRowCount(0);

    NInterface::query1(QStringLiteral("/engine/v2/officen/editors/by-orders"),
                       mUser->mSessionKey,
                       this,
                       {
                           {QStringLiteral("editor"), QStringLiteral("form_sold_items")},
                           {QStringLiteral("goods_id"), mGoodsId},
                           {QStringLiteral("date1"), mDate1},
                           {QStringLiteral("date2"), mDate2},
                           {QStringLiteral("group_id"), mGroupId},
                           {QStringLiteral("fiscal_receipt"), mFiscalReceipt},
                       },
                       [this](const QJsonObject &jo) { fillTable(jo); });
}

void DlgSoldItemOrders::fillTable(const QJsonObject &response)
{
    const QString title = response.value(QStringLiteral("title")).toString();
    if (!title.isEmpty()) {
        setWindowTitle(title);
    }
    const QString name = response.value(QStringLiteral("goods_name")).toString(mGoodsName);
    const QString d1 = response.value(QStringLiteral("date1")).toString(mDate1);
    const QString d2 = response.value(QStringLiteral("date2")).toString(mDate2);
    ui->lbHeader->setText(tr("%1 — %2 … %3").arg(name, d1, d2));

    const QJsonArray rows = response.value(QStringLiteral("rows")).toArray();
    ui->tbl->setRowCount(rows.size());
    for (int r = 0; r < rows.size(); ++r) {
        const QJsonArray row = rows.at(r).toArray();
        for (int c = 0; c < row.size() && c < ui->tbl->columnCount(); ++c) {
            const QString text = row.at(c).toVariant().toString();
            const Qt::Alignment align = (c >= 5)
                                            ? (Qt::AlignRight | Qt::AlignVCenter)
                                            : (Qt::AlignLeft | Qt::AlignVCenter);
            ui->tbl->setItem(r, c, makeItem(text, align));
        }
    }

    if (rows.isEmpty()) {
        ui->lbHeader->setText(tr("No orders found for %1 in %2 … %3").arg(name, d1, d2));
    }
}

void DlgSoldItemOrders::onTableDoubleClicked(int row, int column)
{
    Q_UNUSED(column);
    if (row < 0) {
        return;
    }
    QTableWidgetItem *idItem = ui->tbl->item(row, 0);
    if (!idItem) {
        return;
    }
    const QString orderId = idItem->text().trimmed();
    if (orderId.isEmpty()) {
        return;
    }
    openOrder(orderId);
}

void DlgSoldItemOrders::openOrder(const QString &orderId)
{
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
