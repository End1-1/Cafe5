#include "worderinspector.h"
#include "dict_dish_state.h"
#include "ui_worderinspector.h"
#include "c5utils.h"
#include <QBrush>
#include <QColor>
#include <QCoreApplication>
#include <QHeaderView>
#include <QJsonDocument>
#include <QTableWidgetItem>

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
    ui->splitter->setStretchFactor(0, 3);
    ui->splitter->setStretchFactor(1, 2);
    connect(ui->mDishes, &QTableWidget::currentCellChanged, this, &WOrderInspector::dishSelectionChanged);
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

        if(auto *it = ui->mDishes->item(i, 0)) {
            it->setData(Qt::UserRole, toPrettyJson(d.data));
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
