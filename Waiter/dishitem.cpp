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
    if (fOrder->dishesValue("f_state", fIndex) != DISH_STATE_OK) {
        setVisible(false);
    }
    ui->lbQty1->setText(float_str(fOrder->dishesValue("f_qty1", fIndex).toDouble(), 2));
    ui->lbQty1->setProperty("state", fOrder->dishesValue("f_qty2", fIndex).toDouble() > 0.001 ? "1" : "");
    ui->lbQty1->style()->polish(ui->lbQty1);
    ui->lbTimeOfDish->setText(fOrder->dishesValue("f_appendtime", fIndex).toDateTime().toString(FORMAT_TIME_TO_SHORT_STR));
    ui->frame->setProperty("dish_focused", fFocus ? "1" : "2");
    ui->frame->style()->polish(ui->frame);
    ui->lbComment->setText(fOrder->dishesValue("f_comment", fIndex).toString());
    ui->lbComment->setVisible(!ui->lbComment->text().isEmpty());
    ui->lbTotal->setText(float_str(fOrder->dishTotal(fIndex), 2));
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

void DishItem::on_btnDish_clicked()
{
    fFocus = true;
    setChanges();
    emit focused(fIndex);
}
