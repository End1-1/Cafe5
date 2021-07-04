#include "tablewidgetv1.h"
#include "ui_tablewidgetv1.h"
#include "c5utils.h"
#include "datadriver.h"
#include <QDebug>
#include <QStyle>

TableWidgetV1::TableWidgetV1(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TableWidgetV1)
{
    ui->setupUi(this);
    installEventFilter(this);
    configOrder("");
}

TableWidgetV1::~TableWidgetV1()
{
    delete ui;
}

void TableWidgetV1::config(int id)
{
    fTable = id;
    ui->tw1lbName->setText(dbtable->name(id));
}

void TableWidgetV1::configOrder(const QString &orderid)
{
    QString state = "1";
    if (orderid.isEmpty()) {
        ui->tw1lbStaff->setText("-");
        ui->tw1lbTime->setText("00:00");
        ui->tw1lbAmount->clear();
    } else {
        state = "2";
        if (dboheader->precheck(orderid) > 0) {
            state = "3";
        }
        if (dboheader->print(orderid) > 0) {
            state = "4";
        }
        ui->tw1lbStaff->setText(dbuser->fullShort(dboheader->staff(orderid)));
        ui->tw1lbAmount->setText(float_str(dboheader->amount(orderid), 2));
        ui->tw1lbTime->setText(QDate::currentDate() == dboheader->dateOpen(orderid) ? dboheader->timeOpen(orderid).toString(FORMAT_TIME_TO_SHORT_STR) : dboheader->dateOpen(orderid).toString(FORMAT_DATE_TO_STR) + "\n" + dboheader->timeOpen(orderid).toString(FORMAT_TIME_TO_SHORT_STR));
    }
    ui->frame->setProperty("t1_state", state);
    ui->frame->style()->polish(ui->frame);
}

void TableWidgetV1::mouseReleaseEvent(QMouseEvent *me)
{
    QWidget::mouseReleaseEvent(me);
    emit clicked(fTable);
}
