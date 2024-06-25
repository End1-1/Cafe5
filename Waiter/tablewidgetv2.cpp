#include "tablewidgetv2.h"
#include "ui_tablewidgetv2.h"
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

void TableWidgetV2::configOrder(const QJsonObject &jo)
{
    TableWidget::configOrder(jo);
    ui->lbGuest->clear();
    ui->lbPrepaid->clear();
    if (jo.length() > 0) {
        ui->lbGuest->setText(jo["f_guestname"].toString());
        ui->lbPrepaid->setText(float_str(jo["f_prepaid"].toDouble(), 2));
    }
    if (jo["f_tablename"].toString() != jo["f_fortablename"].toString()) {
        ui->tw1lbName->setText(QString("%1 -> %2").arg(jo["f_tablename"].toString(), jo["f_fortablename"].toString()));
    }
}
