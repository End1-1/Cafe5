#include "waiterdishwidget.h"
#include "ui_waiterdishwidget.h"
#include "dict_dish_state.h"
#include "c5utils.h"
#include "format_date.h"
#include <QPaintEvent>
#include <QPainter>
#include <QPushButton>
#include <QStyle>

WaiterDishWidget::WaiterDishWidget(const WaiterDish &d, bool showRemoved, QWidget *parent) :
    WaiterOrderItemWidget(d, parent),
    mShowRemoved(showRemoved),
    ui(new Ui::WaiterDishWidget)
{
    ui->setupUi(this);
    ui->wmodificators->setVisible(false);
    ui->btnChecked->setVisible(false);
    updateDish(d);
    adjustSize();
    QFont f(font());
    f.setBold(true);
    ui->lbQty1->setFont(f);
    ui->lbComment->installEventFilter(this);
    ui->lbQty1->installEventFilter(this);
    ui->lbTimeOfDish->installEventFilter(this);
    ui->lbTotal->installEventFilter(this);
    ui->lbComplimentary->setVisible(false);
    installEventFilter(this);
}

WaiterDishWidget::~WaiterDishWidget()
{
    delete ui;
}

void WaiterDishWidget::pulish()
{
    ui->lbTimeOfDish->style()->unpolish(ui->lbTimeOfDish);
    ui->lbTotal->style()->unpolish(ui->lbTotal);
    ui->lbComment->style()->unpolish(ui->lbComment);
    ui->lbQty1->style()->unpolish(ui->lbQty1);
    ui->btnDish->style()->unpolish(ui->btnDish);
    ui->lbTimeOfDish->style()->polish(ui->lbTimeOfDish);
    ui->lbTotal->style()->polish(ui->lbTotal);
    ui->lbComment->style()->polish(ui->lbComment);
    ui->lbQty1->style()->polish(ui->lbQty1);
    ui->btnDish->style()->polish(ui->btnDish);
}

void WaiterDishWidget::updateDish(WaiterDish value)
{
    WaiterOrderItemWidget::updateDish(value);
    QString name = mOrderItem.dishName;

    if(!mOrderItem.fromTable().isEmpty()) {
        name = QString("(%1) %2").arg(mOrderItem.fromTable(), name);
    }

    ui->btnDish->setText(name);
    QDateTime dt = mOrderItem.isPrinted() ?
                   str_to_datetime(mOrderItem.appendedTime()) :
                   str_to_datetime(mOrderItem.appendedTime());
    QString dd = dt.date() == QDate::currentDate() ? dt.toString(FORMAT_TIME_TO_SHORT_STR) :
                 QString("<html><body><p align=\"center\" "
            "style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"> "
            "<span style=\" font-size:8pt;\">%1</span></p>\n<p align=\"center\" "
            "style=\" margin-top:0px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"> "
            "<span style=\" font-size:8pt;\">%2</span></p></body></html>")
                 .arg(dt.date().toString(FORMAT_DATE_TO_STR), dt.time().toString(FORMAT_TIME_TO_STR));
    ui->lbTimeOfDish->setText(mOrderItem.state == DISH_STATE_OK ? dd : "-");

    if(mOrderItem.state == DISH_STATE_NONE) {
        setVisible(false);
    } else {
        ui->lbEmarks->setVisible(!mOrderItem.emarks.isEmpty());
        ui->lbComplimentary->setVisible(mOrderItem.complimentary());
        ui->lbQty1->setText(float_str(mOrderItem.qty, 2));
        ui->lbTotal->setText(float_str(mOrderItem.total(false), 2));
        ui->lbComment->setVisible(!mOrderItem.comment().isEmpty() || !mOrderItem.removeReason().isEmpty());
        ui->lbComment->setText(mOrderItem.comment() + "<br>" + mOrderItem.removeReason());

        if(mOrderItem.state == DISH_STATE_OK) {
            ui->lbQty1->setProperty("state", mOrderItem.isPrinted() ? "1" : "0");
            ui->lbQty1->style()->polish(ui->lbQty1);
            pulish();
        } else if(mOrderItem.state == DISH_STATE_SET) {
            ui->orderDishFrame->setProperty("state", "4");
            ui->lbTimeOfDish->setText(mOrderItem.removedTime());
            ui->lbComment->setProperty("state", "4");
            ui->lbQty1->setProperty("state", "4");
            ui->lbTimeOfDish->setProperty("state", "4");
            ui->btnDish->setProperty("state", "4");
            ui->lbTotal->setProperty("state", "4");
            pulish();
        } else {
            setVisible(mShowRemoved);
            ui->orderDishFrame->setProperty("state", "3");
            ui->lbTimeOfDish->setText(mOrderItem.removedTime());
            ui->lbComment->setProperty("state", "3");
            ui->lbComment->setVisible(!mOrderItem.comment().isEmpty() || !mOrderItem.removeReason().isEmpty());
            ui->lbComment->setText(mOrderItem.comment() + "<br>" + mOrderItem.removeReason());
            ui->lbQty1->setProperty("state", "3");
            ui->lbTimeOfDish->setProperty("state", "3");
            ui->btnDish->setProperty("state", "3");
            ui->lbTotal->setProperty("state", "3");
            pulish();
        }
    }

    adjustSize();
}

void WaiterDishWidget::setCheckMode(bool v)
{
    if(mOrderItem.state != DISH_STATE_OK && mOrderItem.state != DISH_STATE_SET) {
        return;
    }

    ui->btnChecked->setVisible(v);
    ui->btnChecked->setChecked(v ? false : ui->btnChecked->isChecked());
}

void WaiterDishWidget::setChecked(bool v)
{
    ui->btnChecked->setChecked(v);
}

bool WaiterDishWidget::isChecked()
{
    return ui->btnChecked->isChecked();
}

bool WaiterDishWidget::event(QEvent *event)
{
    switch(event->type()) {
    case QEvent::MouseButtonRelease:
        ui->btnDish->click();
        break;

    default:
        break;
    }

    return QWidget::event(event);
}

QWidget* WaiterDishWidget::container()
{
    return ui->orderDishFrame;
}

void WaiterDishWidget::on_btnDish_clicked()
{
    if(ui->btnChecked->isVisible()) {
        return;
    }

    setFocused(true);
    updateDish(mOrderItem);
    emit focused(mOrderItem.id);
}
