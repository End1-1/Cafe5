#include "worderinspector.h"
#include "wdishkitchentimesdlg.h"
#include <QApplication>
#include <QBrush>
#include <QClipboard>
#include <QColor>
#include <QCoreApplication>
#include <QDateTime>
#include <QHeaderView>
#include <QJsonDocument>
#include <QMouseEvent>
#include <QTableWidgetItem>
#include <QToolButton>
#include "c5user.h"
#include "c5utils.h"
#include "dict_dish_state.h"
#include "ninterface.h"
#include "ui_worderinspector.h"

namespace
{

QString kitchenStatusShort(int status)
{
    switch(status) {
    case 1:
        return QCoreApplication::translate("WOrderInspector", "Accepted");
    case 2:
        return QCoreApplication::translate("WOrderInspector", "Cooking");
    case 3:
        return QCoreApplication::translate("WOrderInspector", "Ready");
    case 4:
        return QCoreApplication::translate("WOrderInspector", "Served");
    default:
        return QString();
    }
}

QString earliestKitchenStatusTime(const QJsonObject &process, int status)
{
    QString best;
    QDateTime bestDt;

    for(int sub = 1; sub <= 4; ++sub) {
        const QString key = QStringLiteral("f_status_%1_%2_time").arg(status).arg(sub);
        const QString raw = process.value(key).toString().trimmed();
        if(raw.isEmpty()) {
            continue;
        }

        const QDateTime dt = QDateTime::fromString(raw, QStringLiteral("yyyy-MM-dd HH:mm:ss"));
        if(!dt.isValid()) {
            if(best.isEmpty()) {
                best = raw;
            }
            continue;
        }

        if(!bestDt.isValid() || dt < bestDt) {
            bestDt = dt;
            best = raw;
        }
    }

    return best;
}

QString kitchenCellText(const WaiterDish &dish)
{
    const int st = dish.data.value(QStringLiteral("f_kitchen_status")).toInt();
    const QJsonObject process = dish.data.value(QStringLiteral("f_goods_process")).toObject();

    if(process.isEmpty() && st < 1) {
        return QCoreApplication::translate("WOrderInspector", "—");
    }

    QString text;
    if(st >= 1 && st <= 4) {
        text = kitchenStatusShort(st);
    } else if(st == 6) {
        text = QCoreApplication::translate("WOrderInspector", "Closed");
    } else if(st > 0) {
        text = QString::number(st);
    }

    int timeStatus = (st >= 1 && st <= 4) ? st : 1;
    QString time = earliestKitchenStatusTime(process, timeStatus);

    if(time.isEmpty()) {
        for(int s = 4; s >= 1; --s) {
            const QString t = earliestKitchenStatusTime(process, s);
            if(!t.isEmpty()) {
                timeStatus = s;
                time = t;
                if(text.isEmpty()) {
                    text = kitchenStatusShort(s);
                }
                break;
            }
        }
    }

    if(!time.isEmpty()) {
        if(text.isEmpty()) {
            text = kitchenStatusShort(timeStatus);
        }
        text += QStringLiteral("\n") + time;
    }

    return text.isEmpty() ? QCoreApplication::translate("WOrderInspector", "—") : text;
}

} // namespace

WOrderInspector::WOrderInspector(C5User *user, const QString &title, QIcon icon, QWidget *parent)
    : C5OfficeWidget(parent),
      ui(new Ui::WOrderInspector)
{
    mUser = user;
    fLabel = title;
    fIcon = icon;
    ui->setupUi(this);
    ui->mDishes->horizontalHeader()->setStretchLastSection(true);
    ui->mDishes->verticalHeader()->setVisible(false);
    ui->mCalcQueue->horizontalHeader()->setStretchLastSection(true);
    ui->mCalcQueue->verticalHeader()->setVisible(false);
    ui->mCalcQueue->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->mCalcQueue->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    ui->mOrderLog->horizontalHeader()->setStretchLastSection(true);
    ui->mOrderLog->verticalHeader()->setVisible(false);
    ui->splitter->setStretchFactor(0, 3);
    ui->splitter->setStretchFactor(1, 2);
    ui->mOrderId->setCursor(Qt::PointingHandCursor);
    ui->mOrderId->installEventFilter(this);
    connect(ui->btnCopyOrderId, &QToolButton::clicked, this, &WOrderInspector::copyOrderIdToClipboard);
    connect(ui->mDishes, &QTableWidget::currentCellChanged, this, &WOrderInspector::dishSelectionChanged);
    connect(ui->mDishes, &QTableWidget::cellDoubleClicked, this, &WOrderInspector::dishCellDoubleClicked);
    connect(ui->mCalcQueue, &QTableWidget::cellChanged, this, &WOrderInspector::calcQueueCellChanged);
}

WOrderInspector::~WOrderInspector()
{
    delete ui;
}

void WOrderInspector::setOrderFromJson(const QJsonObject &jo)
{
    setOrder(JsonParser<WaiterOrder>::fromJson(jo));
}

void WOrderInspector::setOrder(const WaiterOrder &order)
{
    mOrder = order;
    fillHeader();
    fillDishes();
    fillLog();
    fillCalcQueue();
    ui->mRawOrder->setPlainText(toPrettyJson(mOrder.rawBody));
}

void WOrderInspector::dishSelectionChanged(int currentRow, int, int, int)
{
    if(currentRow < 0 || currentRow >= ui->mDishes->rowCount()) {
        ui->mDishData->clear();
        return;
    }

    auto *cell = ui->mDishes->item(currentRow, 0);

    if(!cell) {
        ui->mDishData->clear();
        return;
    }

    ui->mDishData->setPlainText(cell->data(Qt::UserRole).toString());
}

void WOrderInspector::dishCellDoubleClicked(int row, int column)
{
    if(column != dishColAppendTime && column != dishColPrintTime && column != dishColKitchen) {
        return;
    }

    if(row < 0 || row >= mOrder.dishes.size()) {
        return;
    }

    WDishKitchenTimesDlg dlg(this);
    dlg.setUser(mUser);
    dlg.setDish(mOrder.dishes.at(row));

    if(dlg.exec() != QDialog::Accepted) {
        return;
    }

    const QJsonObject orderJo = dlg.savedOrder();
    if(!orderJo.isEmpty()) {
        setOrder(JsonParser<WaiterOrder>::fromJson(orderJo));
    }
}

bool WOrderInspector::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->mOrderId && event->type() == QEvent::MouseButtonRelease) {
        const auto *mouseEvent = static_cast<const QMouseEvent *>(event);

        if(mouseEvent->button() == Qt::LeftButton) {
            copyOrderIdToClipboard();
            return true;
        }
    }

    return C5OfficeWidget::eventFilter(watched, event);
}

void WOrderInspector::copyOrderIdToClipboard()
{
    const QString id = mOrder.id.trimmed();

    if(id.isEmpty()) {
        return;
    }

    QApplication::clipboard()->setText(id);
}

QString WOrderInspector::toPrettyJson(const QJsonObject &obj)
{
    return QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Indented));
}

QString WOrderInspector::toPrettyJson(const QJsonValue &value)
{
    if(value.isObject()) {
        return toPrettyJson(value.toObject());
    }

    if(value.isArray()) {
        return QString::fromUtf8(QJsonDocument(value.toArray()).toJson(QJsonDocument::Indented));
    }

    return value.toVariant().toString();
}

QString WOrderInspector::orderStateName(int state)
{
    switch(state) {
    case 1:
        return tr("Open");
    case 2:
        return tr("Closed");
    case 3:
        return tr("Empty");
    case 4:
        return tr("Moved");
    case 5:
        return tr("Preorder");
    default:
        return tr("State %1").arg(state);
    }
}

void WOrderInspector::fillHeader()
{
    ui->mOrderId->setText(mOrder.id);
    ui->mPrefix->setText(mOrder.receiptNumber);
    ui->mState->setText(orderStateName(mOrder.state));
    ui->mTable->setText(mOrder.tableName);
    ui->mHall->setText(mOrder.hallName);
    ui->mStaff->setText(mOrder.staffName);
    ui->mCashier->setText(mOrder.cashierName);
    ui->mAmount->setText(float_str(mOrder.totalDue, 2));
    ui->mHeaderData->setPlainText(toPrettyJson(mOrder.data));
}

void WOrderInspector::fillDishes()
{
    ui->mDishes->setRowCount(0);
    ui->mDishes->setRowCount(mOrder.dishes.size());

    for(int i = 0; i < mOrder.dishes.size(); i++) {
        const WaiterDish &d = mOrder.dishes.at(i);
        QString stateText;

        if(dish_state_names.contains(d.state)) {
            stateText = QCoreApplication::translate("DishState", dish_state_names[d.state]);
        } else {
            stateText = QString::number(d.state);
        }

        auto setText = [this, i](int c, const QString &text) {
            auto *it = new QTableWidgetItem(text);
            ui->mDishes->setItem(i, c, it);
        };

        setText(0, stateText);
        setText(1, QString::number(d.type));
        setText(2, QString::number(d.row));
        setText(3, d.dishName);
        setText(4, float_str(d.qty, 3));
        setText(5, float_str(d.price, 2));
        setText(6, float_str(d.total(false), 2));
        setText(7, d.state == DISH_STATE_OK ? QString() : tr("Yes"));
        setText(8, d.removeReason());
        setText(9, d.data.value("f_append_time").toString());
        setText(10, d.data.value("f_print_time").toString());
        setText(dishColKitchen, kitchenCellText(d));

        if(auto *it = ui->mDishes->item(i, 0)) {
            it->setData(Qt::UserRole, toPrettyJson(d.toJson()));
        }

        if(d.state != DISH_STATE_OK) {
            for(int c = 0; c < ui->mDishes->columnCount(); c++) {
                auto *it = ui->mDishes->item(i, c);

                if(!it) {
                    continue;
                }

                it->setForeground(QBrush(Qt::darkRed));
                QColor bg = it->background().color();
                bg.setAlpha(35);
                bg.setRed(255);
                it->setBackground(bg);
            }
        }
    }

    ui->mDishes->resizeColumnsToContents();

    if(ui->mDishes->rowCount() > 0) {
        ui->mDishes->setCurrentCell(0, 0);
    } else {
        ui->mDishData->clear();
    }
}

void WOrderInspector::fillLog()
{
    static const QStringList knownKeys = {
        QStringLiteral("ts"),
        QStringLiteral("host"),
        QStringLiteral("action"),
        QStringLiteral("user"),
        QStringLiteral("comment"),
        QStringLiteral("important"),
    };

    const QJsonArray jlog = mOrder.dataValue(QStringLiteral("log")).toArray();
    ui->mOrderLog->setRowCount(jlog.size());

    for(int i = 0; i < jlog.size(); i++) {
        const QJsonObject jl = jlog.at(i).toObject();
        QStringList extraParts;

        for(auto it = jl.begin(); it != jl.end(); ++it) {
            if(knownKeys.contains(it.key())) {
                continue;
            }

            const QJsonValue v = it.value();

            if(v.isDouble()) {
                extraParts << QStringLiteral("%1=%2").arg(it.key(), float_str(v.toDouble(), 2));
            } else if(v.isBool()) {
                extraParts << QStringLiteral("%1=%2").arg(it.key(), v.toBool() ? QStringLiteral("true") : QStringLiteral("false"));
            } else {
                extraParts << QStringLiteral("%1=%2").arg(it.key(), v.toVariant().toString());
            }
        }

        auto setText = [this, i](int c, const QString &text) {
            ui->mOrderLog->setItem(i, c, new QTableWidgetItem(text));
        };

        setText(0, jl.value(QStringLiteral("ts")).toString());
        setText(1, jl.value(QStringLiteral("action")).toString());
        setText(2, jl.value(QStringLiteral("user")).toString());
        setText(3, jl.value(QStringLiteral("host")).toString());
        setText(4, jl.value(QStringLiteral("comment")).toString());
        setText(5, extraParts.join(QStringLiteral("; ")));

        if(!jl.value(QStringLiteral("important")).toBool()) {
            continue;
        }

        for(int c = 0; c < ui->mOrderLog->columnCount(); c++) {
            auto *it = ui->mOrderLog->item(i, c);

            if(!it) {
                continue;
            }

            it->setForeground(QBrush(Qt::darkRed));
            QFont f = it->font();
            f.setBold(true);
            it->setFont(f);
        }
    }

    ui->mOrderLog->resizeColumnsToContents();
}

void WOrderInspector::fillCalcQueue()
{
    mCalcQueueFilling = true;
    const QJsonArray rows = mOrder.calcQueue;
    ui->mCalcQueue->setRowCount(rows.size());

    for(int i = 0; i < rows.size(); i++) {
        const QJsonObject row = rows.at(i).toObject();
        const double soldQty = row.value(QStringLiteral("f_sold_qty")).toVariant().toDouble();
        const double consumedQty = row.value(QStringLiteral("f_qty")).toVariant().toDouble();
        const double price = row.value(QStringLiteral("f_price")).toVariant().toDouble();
        const double total = consumedQty * price;
        const QString rowId = row.value(QStringLiteral("f_id")).toString();

        auto setText = [this, i](int c, const QString &text, Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled) {
            auto *it = new QTableWidgetItem(text);
            it->setFlags(flags);
            ui->mCalcQueue->setItem(i, c, it);
        };

        setText(0, row.value(QStringLiteral("f_dish_name")).toString());
        setText(1, row.value(QStringLiteral("f_component_name")).toString());
        setText(2, float_str(soldQty, 3));
        setText(3, float_str(consumedQty, 4));
        setText(4, float_str(price, 2), Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
        setText(5, float_str(total, 2));

        if(auto *it = ui->mCalcQueue->item(i, 0)) {
            it->setData(Qt::UserRole, rowId);
        }
    }

    if(!rows.isEmpty()) {
        const int sumRow = rows.size();
        ui->mCalcQueue->setRowCount(sumRow + 1);

        auto setSum = [this, sumRow](int c, const QString &text, bool bold = false) {
            auto *it = new QTableWidgetItem(text);
            it->setFlags(Qt::ItemIsEnabled);
            if(bold) {
                QFont f = it->font();
                f.setBold(true);
                it->setFont(f);
            }
            ui->mCalcQueue->setItem(sumRow, c, it);
        };

        setSum(0, tr("Total"), true);
    }

    updateCalcQueueSumRow();
    ui->mCalcQueue->resizeColumnsToContents();
    mCalcQueueFilling = false;
}

void WOrderInspector::updateCalcQueueSumRow()
{
    const int rowCount = mOrder.calcQueue.size();
    if(rowCount == 0) {
        return;
    }

    const int sumRow = rowCount;
    double grandTotal = 0;

    for(int i = 0; i < rowCount; i++) {
        const QJsonObject row = mOrder.calcQueue.at(i).toObject();
        const double consumedQty = row.value(QStringLiteral("f_qty")).toVariant().toDouble();
        const double price = row.value(QStringLiteral("f_price")).toVariant().toDouble();
        grandTotal += consumedQty * price;
    }

    mCalcQueueFilling = true;

    if(ui->mCalcQueue->rowCount() <= sumRow) {
        ui->mCalcQueue->setRowCount(sumRow + 1);
    }

    if(auto *sumItem = ui->mCalcQueue->item(sumRow, calcQueueColTotal)) {
        sumItem->setText(float_str(grandTotal, 2));
    } else {
        auto *it = new QTableWidgetItem(float_str(grandTotal, 2));
        it->setFlags(Qt::ItemIsEnabled);
        QFont f = it->font();
        f.setBold(true);
        it->setFont(f);
        ui->mCalcQueue->setItem(sumRow, calcQueueColTotal, it);
    }

    mCalcQueueFilling = false;
}

void WOrderInspector::refreshCalcQueueRowTotal(int row)
{
    if(row < 0 || row >= mOrder.calcQueue.size()) {
        return;
    }

    const QJsonObject calcRow = mOrder.calcQueue.at(row).toObject();
    const double consumedQty = calcRow.value(QStringLiteral("f_qty")).toVariant().toDouble();
    const double price = calcRow.value(QStringLiteral("f_price")).toVariant().toDouble();

    mCalcQueueFilling = true;

    if(auto *costItem = ui->mCalcQueue->item(row, calcQueueColCost)) {
        costItem->setText(float_str(price, 2));
    }

    if(auto *totalItem = ui->mCalcQueue->item(row, calcQueueColTotal)) {
        totalItem->setText(float_str(consumedQty * price, 2));
    }

    mCalcQueueFilling = false;
}

void WOrderInspector::calcQueueCellChanged(int row, int column)
{
    if(mCalcQueueFilling || column != calcQueueColCost) {
        return;
    }

    if(row < 0 || row >= mOrder.calcQueue.size()) {
        return;
    }

    auto *idItem = ui->mCalcQueue->item(row, 0);
    auto *costItem = ui->mCalcQueue->item(row, calcQueueColCost);

    if(!idItem || !costItem) {
        return;
    }

    const QString rowId = idItem->data(Qt::UserRole).toString().trimmed();
    if(rowId.isEmpty()) {
        return;
    }

    const QJsonObject calcRow = mOrder.calcQueue.at(row).toObject();
    const double oldPrice = calcRow.value(QStringLiteral("f_price")).toVariant().toDouble();
    const double consumedQty = calcRow.value(QStringLiteral("f_qty")).toVariant().toDouble();

    bool ok = false;
    const double newPrice = QString(costItem->text()).trimmed().replace(QLatin1Char(','), QLatin1Char('.')).toDouble(&ok);
    if(!ok || newPrice < 0) {
        mCalcQueueFilling = true;
        costItem->setText(float_str(oldPrice, 2));
        mCalcQueueFilling = false;
        return;
    }

    if(qAbs(newPrice - oldPrice) < 0.0001) {
        mCalcQueueFilling = true;
        costItem->setText(float_str(oldPrice, 2));
        mCalcQueueFilling = false;
        return;
    }

    mCalcQueueFilling = true;
    costItem->setText(float_str(newPrice, 2));

    if(auto *totalItem = ui->mCalcQueue->item(row, calcQueueColTotal)) {
        totalItem->setText(float_str(consumedQty * newPrice, 2));
    }

    mCalcQueueFilling = false;

    NInterface::query(QStringLiteral("/engine/v2/waiter/order/set-calc-queue-price"),
                      mUser->mSessionKey,
                      this,
                      {
                          {QStringLiteral("id"), rowId},
                          {QStringLiteral("price"), newPrice},
                      },
                      [this, row, newPrice](const QJsonObject &jo) {
                          if(jo.contains(QStringLiteral("calc_queue"))) {
                              mOrder.calcQueue = jo.value(QStringLiteral("calc_queue")).toArray();
                          } else {
                              QJsonArray arr = mOrder.calcQueue;
                              QJsonObject obj = arr.at(row).toObject();
                              obj.insert(QStringLiteral("f_price"), newPrice);
                              arr.replace(row, obj);
                              mOrder.calcQueue = arr;
                          }

                          refreshCalcQueueRowTotal(row);
                          updateCalcQueueSumRow();
                      },
                      [this, row, oldPrice](const QJsonObject &) {
                          refreshCalcQueueRowTotal(row);
                          updateCalcQueueSumRow();
                          return false;
                      },
                      false);
}
