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

    QString name = dbdish->name(fOrder->dishesValue("f_dish", fIndex).toInt());
    if (od->dishesValue("f_fromtable", index).toInt() > 0) {
        name = QString("(%1) %2").arg(dbtable->name(od->dishesValue("f_fromtable", index).toInt())).arg(name);
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
            ui->lbQty1->setProperty("state", fOrder->dishesValue("f_qty2", fIndex).toDouble() > 0.001 ? "1" : "");
            ui->lbQty1->style()->polish(ui->lbQty1);
            ui->lbTimeOfDish->setText(fOrder->dishesValue("f_state", fIndex).toInt() == DISH_STATE_OK ?
                                          (fOrder->dishesValue("f_qty2", fIndex).toDouble() > 0.001 ?
                                          fOrder->dishesValue("f_printtime", fIndex).toDateTime().toString(FORMAT_TIME_TO_SHORT_STR) :
                                          fOrder->dishesValue("f_appendtime", fIndex).toDateTime().toString(FORMAT_TIME_TO_SHORT_STR)) : "-");
            ui->frame->setProperty("dish_focused", fFocus ? "1" : "2");
            ui->frame->style()->polish(ui->frame);
            ui->btnReprint->setVisible(property("reprint").toBool() && fOrder->dishesValue("f_qty2", fIndex).toDouble() > 0.001);
        } else {
            ui->frame->setProperty("state", "3");
            ui->frame->style()->polish(ui->frame);
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
    }
    return QWidget::event(event);
}

void DishItem::on_btnDish_clicked()
{
    fFocus = true;
    setChanges();
    emit focused(fIndex);
}

void DishItem::on_btnReprint_clicked()
{
    int reprint = fOrder->dishesValue("f_reprint", fIndex).toInt();
    fOrder->setDishesValue("f_reprint", ui->btnReprint->isChecked() ? (abs(reprint) + 1) * -1 : abs(reprint), fIndex);
}
