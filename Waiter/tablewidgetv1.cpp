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
    bool commentVisible = !dboheader->comment(orderid).isEmpty();
    ui->lbComment->setVisible(commentVisible);
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
        ui->lbComment->setText(dboheader->comment(orderid));
        ui->tw1lbStaff->setText(dbuser->fullShort(dboheader->staff(orderid)));
        ui->tw1lbAmount->setText(float_str(dboheader->amount(orderid), 2));
        QString dd = QString("<html><body><p align=\"center\" "
               "style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"> "
               "<span style=\" font-size:6pt;\">%1</span></p>\n<p align=\"center\" "
               "style=\" margin-top:0px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"> "
               "<span style=\" font-size:8pt;\">%2</span></p></body></html>")
                                                 .arg(dboheader->dateOpen(orderid).toString(FORMAT_DATE_TO_STR))
                                                 .arg(dboheader->timeOpen(orderid).toString(FORMAT_TIME_TO_SHORT_STR));
        ui->tw1lbTime->setText(QDate::currentDate() == dboheader->dateOpen(orderid) ?
                                   dboheader->timeOpen(orderid).toString(FORMAT_TIME_TO_SHORT_STR) : dd );
    }
    ui->frame->setProperty("t1_state", state);
    ui->frame->style()->polish(ui->frame);
}

void TableWidgetV1::mouseReleaseEvent(QMouseEvent *me)
{
    QWidget::mouseReleaseEvent(me);
    emit clicked(fTable);
}
