#include "rwaiterorder.h"
#include "ui_rwaiterorder.h"
#include "c5dialog.h"
#include "dict_dish_state.h"
#include "dict_payment_type.h"
#include "c5utils.h"
#include <QScreen>

RWaiterOrder::RWaiterOrder(QWidget *parent)
    : RAbstractSpecialWidget(parent),
      ui(new Ui::RWaiterOrder)
{
    ui->setupUi(this);
    auto *s = C5Dialog::mScreen < 0 ? qApp->screens().first() : qApp->screens().at(C5Dialog::mScreen);
    setMinimumHeight(s->geometry().height() - 200);
    setMinimumWidth(s->geometry().width() - 300);
    ui->wcashier->setSelectorName(tr("Cashier"));
    ui->wstaff->setSelectorName(tr("Staff"));
}

RWaiterOrder::~RWaiterOrder()
{
    delete ui;
}

void RWaiterOrder::setup(const QJsonObject &jdoc)
{
    RAbstractSpecialWidget::setup(jdoc);
    mOrder = JsonParser<WaiterOrder>::fromJson(jdoc);
    ui->leSessionID->setInteger(mOrder.cashSessionId);
    ui->leTableName->setText(mOrder.tableName);
    ui->leOpened->setDateTime(mOrder.dateOpen());
    ui->leClosed->setDateTime(mOrder.dateClose());
    ui->tblDishes->setRowCount(mOrder.dishes.size());
    ui->wcashier->setCodeAndName(mOrder.cashierId, mOrder.cashierName);
    ui->wstaff->setCodeAndName(mOrder.staffId, mOrder.staffName);
    ui->leTotal->setDouble(mOrder.totalDue);

    for(int i = 0; i < mOrder.dishes.size(); i++) {
        auto const &d = mOrder.dishes.at(i);
        ui->tblDishes->setString(i, 0, dish_state_names[d.state]);
        ui->tblDishes->setString(i, 1, d.dishName);
        ui->tblDishes->setDouble(i, 2, d.qty);
        ui->tblDishes->setDouble(i, 3, d.price);
        ui->tblDishes->setDouble(i, 4, d.total(false));
        ui->tblDishes->setString(i, 5, d.dataValue("f_append_time").toString());
        ui->tblDishes->setString(i, 6, d.dataValue("f_print_time").toString());
        ui->tblDishes->setString(i, 7, d.dataValue("f_remove_time").toString());
        ui->tblDishes->setString(i, 8, d.removeReason());
    }

    ui->tblDishes->resizeColumnsToContents();
    QJsonArray jlog = mOrder.dataValue("log").toArray();
    ui->tblLog->setRowCount(jlog.size());

    for(int i = 0; i < jlog.size(); i++) {
        auto const &jl = jlog.at(i).toObject();
        ui->tblLog->setString(i, 0, jl.value("host").toString());
        ui->tblLog->setString(i, 1, jl.value("ts").toString());
        ui->tblLog->setString(i, 2, jl.value("action").toString());
        ui->tblLog->setString(i, 3, jl.value("user").toString());
        ui->tblLog->setString(i, 4, jl.value("comment").toString());
        ui->tblLog->setString(i, 5, jl.value("important").toBool() ? "!!!" : "");
    }

    ui->tblLog->resizeColumnsToContents();

    for(auto pt : payment_types) {
        if(mOrder.payment(payment_fields[pt]) > 0.01) {
            ui->hlPayment->addWidget(new QLabel(QString("%1: ").arg(payment_names[pt])));
            ui->hlPayment->addWidget(new QLabel(float_str(mOrder.payment(payment_fields[pt]), 2)));
        }
    }

    ui->hlPayment->addStretch();
}
