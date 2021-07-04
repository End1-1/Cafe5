#include "dishitem.h"
#include "ui_dishitem.h"
#include "c5orderdriver.h"
#include "datadriver.h"
#include "dlgqty.h"
#include "c5utils.h"
#include <QPaintEvent>
#include <QPainter>

class QPart2Button : public QPushButton {
public:
    QPart2Button(QWidget *parent = nullptr) :
    QPushButton(parent)
    {
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    }

protected:
    virtual void paintEvent(QPaintEvent *pe) override {
        QPainter p(this);
        QRect r = pe->rect();
        r.adjust(2, 2, -2, -2);
        QTextOption o;
        o.setWrapMode(QTextOption::WordWrap);
        p.drawText(r, text(), o);
    }
};

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
    QPart2Button *btn = new QPart2Button();
    connect(btn, &QPart2Button::clicked, this, &DishItem::on_btnName_clicked);
    ui->hl->insertWidget(0, btn);
    QString name = dbdish->name(fOrder->dishesValue("f_dish", fIndex).toInt());
    if (od->dishesValue("f_fromtable", index).toInt() > 0) {
        name = QString("(%1) %2").arg(dbtable->name(od->dishesValue("f_fromtable", index).toInt())).arg(name);
    }
    btn->setText(name);
    setChanges();
    adjustSize();
    ui->wqty->setVisible(false);
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
    if (!fReadOnly) {
        ui->wqty->setVisible(fFocus);
        ui->winfo->setVisible(!fFocus);
    }
    if (fOrder->dishesValue("f_state", fIndex) != DISH_STATE_OK) {
        setVisible(false);
    }
    ui->frame->setProperty("dish_focused", fFocus ? "1" : "2");
    ui->frame->style()->polish(ui->frame);
    ui->lbQty1->setText(float_str(fOrder->dishesValue("f_qty1", fIndex).toDouble(), 2));
    ui->lbQty2->setText(float_str(fOrder->dishesValue("f_qty2", fIndex).toDouble(), 2));
    ui->labelChangeQty->setText(float_str(fOrder->dishesValue("f_qty1", fIndex).toDouble(), 2));
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

void DishItem::on_btnName_clicked()
{
    fFocus = true;
    setChanges();
    emit focused(fIndex);
}

void DishItem::on_btnPlus1_clicked()
{
    emit changeQty(1);
}

void DishItem::on_btnMinus1_clicked()
{
    emit changeQty(-1);
}

void DishItem::on_btnAnyqty_clicked()
{
    if (fOrder->dishesValue("d_package", fIndex).toInt() > 0) {
        C5Message::error(tr("You cannot change the quantity of items of package"));
        return;
    }
    if (fOrder->dishesValue("f_hourlypayment", fIndex).toInt() > 0) {
        C5Message::error(tr("This is hourly payment item"));
        return;
    }
    double q = fOrder->dishesValue("f_qty1", fIndex).toDouble();
    double max = 999;
    if (!DlgQty::getQty(max, fOrder->dishesValue("f_name", fIndex).toString())) {
        return;
    }
    emit changeQty(max);
}
