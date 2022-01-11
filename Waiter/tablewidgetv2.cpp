#include "tablewidgetv2.h"
#include "ui_tablewidgetv2.h"
#include "datadriver.h"
#include "c5utils.h"
#include <QDebug>

TableWidgetV2::TableWidgetV2(QWidget *parent) :
    TableWidget(parent),
    ui(new Ui::TableWidgetV2)
{
    ui->setupUi(this);
}

TableWidgetV2::~TableWidgetV2()
{
    delete ui;
}

QLabel *TableWidgetV2::labelTable()
{
    return ui->tw1lbName;
}

QLabel *TableWidgetV2::labelStaff()
{
    return ui->tw1lbStaff;
}

QLabel *TableWidgetV2::labelTime()
{
    return ui->tw1lbTime;
}

QLabel *TableWidgetV2::labelAmount()
{
    return ui->tw1lbAmount;
}

QLabel *TableWidgetV2::labelComment()
{
    return ui->lbComment;
}

QFrame *TableWidgetV2::frame()
{
    return ui->frame;
}

void TableWidgetV2::configOrder(const QString &orderid)
{
    TableWidget::configOrder(orderid);
    ui->lbGuest->clear();
    ui->lbPrepaid->clear();
    ui->lbReserveTable->clear();
    if (orderid.length() > 0) {
        ui->lbGuest->setText(dbopreorder->guestName(orderid));
        ui->lbPrepaid->setText(float_str(dbopreorder->amount(orderid), 2));
    }
    if (dbopreorder->table(orderid) > 0) {
        ui->lbReserveTable->setText(dbtable->name(dbopreorder->table(orderid)));
    }
}
