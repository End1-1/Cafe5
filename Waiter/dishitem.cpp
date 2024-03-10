#include "dishitem.h"
#include "ui_dishitem.h"
#include "c5orderdriver.h"
#include "datadriver.h"
#include "dlgqty.h"
#include "c5utils.h"
#include <QPaintEvent>
#include <QPainter>
#include <QPushButton>

DishItem::DishItem(C5OrderDriver *od, int index, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DishItem),
    fOrder(od),
    fIndex(index)
{
    ui->setupUi(this);
    ui->wmodificators->setVisible(false);
    fFocus = false;
    fReadOnly = false;
    ui->btnChecked->setVisible(false);

    QString name = dbdish->name(fOrder->dishesValue("f_dish", fIndex).toInt());
    if (od->dishesValue("f_fromtable", index).toInt() > 0) {
        name = QString("(%1) %2").arg(dbtable->name(od->dishesValue("f_fromtable", index).toInt()), name);
    }
    ui->btnDish->setText(name);
    setChanges();
    adjustSize();
    QFont f(font());
    f.setWeight(99);
    ui->lbQty1->setFont(f);
    ui->lbComment->installEventFilter(this);
    ui->lbQty1->installEventFilter(this);
    ui->lbTimeOfDish->installEventFilter(this);
    ui->lbTotal->installEventFilter(this);
    installEventFilter(this);
}

DishItem::~DishItem()
{
    delete ui;
}

void DishItem::clearFocus(int index)
{
    fFocus = index == fIndex;
    setChanges();
}

void DishItem::setChanges()
{
    if (fOrder->dishesValue("f_state", fIndex) == DISH_STATE_NONE) {
        setVisible(false);
    } else {
        ui->lbQty1->setText(float_str(fOrder->dishesValue("f_qty1", fIndex).toDouble(), 2));
        ui->lbComment->setText(fOrder->dishesValue("f_comment2", fIndex).toString() + " " + fOrder->dishesValue("f_comment", fIndex).toString());
        ui->lbComment->setVisible(!ui->lbComment->text().isEmpty());
        ui->lbTotal->setText(float_str(fOrder->dishTotal2(fIndex), 2));
        if (fOrder->dishesValue("f_state", fIndex) == DISH_STATE_OK) {
            ui->lbQty1->setProperty("state", fOrder->dishesValue("f_qty2", fIndex).toDouble() > 0.001 ? "1" : "0");
            ui->lbQty1->style()->polish(ui->lbQty1);
            QDateTime dt = fOrder->dishesValue("f_qty2", fIndex).toDouble() > 0.001 ?
                                fOrder->dishesValue("f_printtime", fIndex).toDateTime() :
                                fOrder->dishesValue("f_appendtime", fIndex).toDateTime();
            QString dd = dt.date() == QDate::currentDate() ? dt.toString(FORMAT_TIME_TO_SHORT_STR) :
                    QString("<html><body><p align=\"center\" "
                           "style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"> "
                           "<span style=\" font-size:6pt;\">%1</span></p>\n<p align=\"center\" "
                           "style=\" margin-top:0px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"> "
                           "<span style=\" font-size:8pt;\">%2</span></p></body></html>")
                                                             .arg(dt.toString(FORMAT_DATE_TO_STR), dt.toString(FORMAT_TIME_TO_STR));
            ui->lbTimeOfDish->setText(fOrder->dishesValue("f_state", fIndex).toInt() == DISH_STATE_OK ? dd : "-");
            ui->orderDishFrame->setProperty("dish_focused", fFocus ? "1" : "2");
            ui->orderDishFrame->style()->polish(ui->orderDishFrame);
        } else if (fOrder->dishesValue("f_state", fIndex) == DISH_STATE_SET) {
            ui->orderDishFrame->setProperty("state", "4");
            ui->orderDishFrame->style()->polish(ui->orderDishFrame);
            ui->lbTimeOfDish->setText(fOrder->dishesValue("f_removetime", fIndex).toDateTime().toString(FORMAT_TIME_TO_SHORT_STR));
            ui->lbComment->setProperty("state", "4");
            ui->lbQty1->setProperty("state", "4");
            ui->lbTimeOfDish->setProperty("state", "4");
            ui->btnDish->setProperty("state", "4");
            ui->lbTotal->setProperty("state", "4");
            ui->lbTimeOfDish->style()->polish(ui->lbTimeOfDish);
            ui->lbTotal->style()->polish(ui->lbTotal);
            ui->lbComment->style()->polish(ui->lbComment);
            ui->lbQty1->style()->polish(ui->lbQty1);
            ui->btnDish->style()->polish(ui->btnDish);
        } else {
            ui->orderDishFrame->setProperty("state", "3");
            ui->orderDishFrame->style()->polish(ui->orderDishFrame);
            ui->lbTimeOfDish->setText(fOrder->dishesValue("f_removetime", fIndex).toDateTime().toString(FORMAT_TIME_TO_SHORT_STR));
            ui->lbComment->setProperty("state", "3");
            ui->lbQty1->setProperty("state", "3");
            ui->lbTimeOfDish->setProperty("state", "3");
            ui->btnDish->setProperty("state", "3");
            ui->lbTotal->setProperty("state", "3");
            ui->lbTimeOfDish->style()->polish(ui->lbTimeOfDish);
            ui->lbTotal->style()->polish(ui->lbTotal);
            ui->lbComment->style()->polish(ui->lbComment);
            ui->lbQty1->style()->polish(ui->lbQty1);
            ui->btnDish->style()->polish(ui->btnDish);
        }
    }
    adjustSize();
}

bool DishItem::isFocused()
{
    return fFocus;
}

int DishItem::index()
{
    return fIndex;
}

void DishItem::setCheckMode(bool v)
{
    if (fOrder->dishesValue("f_state", fIndex).toInt() != DISH_STATE_OK
        && fOrder->dishesValue("f_state", fIndex).toInt() != DISH_STATE_SET) {
        return;
    }
    ui->btnChecked->setVisible(v);
    ui->btnChecked->setChecked(v ? false : ui->btnChecked->isChecked());
}

void DishItem::setChecked(bool v)
{
    ui->btnChecked->setChecked(v);
}

bool DishItem::isChecked()
{
    return ui->btnChecked->isChecked();
}

void DishItem::setReadyOnly(bool v)
{
    fReadOnly = v;
}

bool DishItem::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::MouseButtonRelease:
        ui->btnDish->click();
        break;
    default:
        break;
    }
    return QWidget::event(event);
}

void DishItem::on_btnDish_clicked()
{
    if (ui->btnChecked->isVisible()) {
        return;
    }
    fFocus = true;
    setChanges();
    emit focused(fIndex);
}
