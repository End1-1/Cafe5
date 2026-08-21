#include "dlgcookingprogress.h"
#include "ui_dlgcookingprogress.h"

#include <QCoreApplication>
#include <QHeaderView>
#include <QJsonObject>
#include <QSet>
#include <QTableWidgetItem>
#include "c5message.h"
#include "c5user.h"
#include "c5utils.h"
#include "ninterface.h"

namespace
{

QString statusLabel(int st)
{
    switch(st) {
    case 1:
        return QCoreApplication::translate("DlgCookingProgress", "Accepted");
    case 2:
        return QCoreApplication::translate("DlgCookingProgress", "Cooking");
    case 3:
        return QCoreApplication::translate("DlgCookingProgress", "Ready");
    case 4:
        return QCoreApplication::translate("DlgCookingProgress", "Served");
    default:
        return QString::number(st);
    }
}

} // namespace

DlgCookingProgress::DlgCookingProgress(C5User *user, QWidget *parent)
    : C5ShopDialog(user, parent)
    , ui(new Ui::DlgCookingProgress)
{
    ui->setupUi(this);
    ui->tblOrders->setColumnCount(4);
    ui->tblOrders->setHorizontalHeaderLabels(
        {tr("Order"), tr("Dish"), tr("Qty"), tr("Status")});
    ui->tblOrders->horizontalHeader()->setStretchLastSection(true);
    ui->tblOrders->setColumnWidth(0, 120);
    ui->tblOrders->setColumnWidth(1, 360);
    ui->tblOrders->setColumnWidth(2, 80);
    reload();
}

DlgCookingProgress::~DlgCookingProgress()
{
    delete ui;
}

void DlgCookingProgress::reload()
{
    NInterface::query1(QStringLiteral("/engine/v2/waiter/in-progress/get"),
                       mUser->mSessionKey,
                       this,
                       {},
                       [this](const QJsonObject &jdoc) {
                           mOrders = jdoc.value(QStringLiteral("data")).toArray();
                           fillTable();
                       });
}

void DlgCookingProgress::fillTable()
{
    ui->tblOrders->setRowCount(0);
    for(const auto &ov : mOrders) {
        const QJsonObject order = ov.toObject();
        const QString prefix = order.value(QStringLiteral("f_order_prefix")).toString();
        const QJsonArray lines = order.value(QStringLiteral("lines")).toArray();
        for(const auto &lv : lines) {
            const QJsonObject line = lv.toObject();
            const int st = line.value(QStringLiteral("f_status")).toInt();
            if(st >= 4) {
                continue;
            }
            const int row = ui->tblOrders->rowCount();
            ui->tblOrders->insertRow(row);
            auto *orderItem = new QTableWidgetItem(prefix);
            orderItem->setData(Qt::UserRole, line.value(QStringLiteral("f_goods_row_id")).toString());
            orderItem->setData(Qt::UserRole + 1, st);
            ui->tblOrders->setItem(row, 0, orderItem);
            ui->tblOrders->setItem(row, 1, new QTableWidgetItem(line.value(QStringLiteral("f_goods_name")).toString()));
            ui->tblOrders->setItem(row, 2, new QTableWidgetItem(float_str(line.value(QStringLiteral("f_qty")).toDouble(), 2)));
            ui->tblOrders->setItem(row, 3, new QTableWidgetItem(statusLabel(st)));
        }
    }
}

QStringList DlgCookingProgress::selectedLineIds() const
{
    QStringList ids;
    const auto ranges = ui->tblOrders->selectedRanges();
    QSet<int> rows;
    for(const auto &r : ranges) {
        for(int row = r.topRow(); row <= r.bottomRow(); ++row) {
            rows.insert(row);
        }
    }
    for(int row : rows) {
        auto *item = ui->tblOrders->item(row, 0);
        if(!item) {
            continue;
        }
        const QString id = item->data(Qt::UserRole).toString();
        if(!id.isEmpty()) {
            ids << id;
        }
    }
    return ids;
}

void DlgCookingProgress::setSelectedLinesStatus(int status)
{
    const QStringList ids = selectedLineIds();
    if(ids.isEmpty()) {
        C5Message::error(tr("Select lines"));
        return;
    }
    if(status == 4) {
        if(C5Message::question(tr("Mark as served to the guest?")) != QDialog::Accepted) {
            return;
        }
    }

    const int total = ids.size();
    auto *pending = new int(total);
    for(const QString &id : ids) {
        NInterface::query1(QStringLiteral("/engine/v2/waiter/in-progress/update-status"),
                           mUser->mSessionKey,
                           this,
                           {{QStringLiteral("id"), id},
                            {QStringLiteral("status"), status},
                            {QStringLiteral("substatus"), 1}},
                           [this, pending](const QJsonObject &) {
                               --(*pending);
                               if(*pending <= 0) {
                                   delete pending;
                                   reload();
                               }
                           });
    }
}

void DlgCookingProgress::on_btnRefresh_clicked()
{
    reload();
}

void DlgCookingProgress::on_btnReady_clicked()
{
    setSelectedLinesStatus(3);
}

void DlgCookingProgress::on_btnServed_clicked()
{
    setSelectedLinesStatus(4);
}

void DlgCookingProgress::on_btnClose_clicked()
{
    accept();
}
