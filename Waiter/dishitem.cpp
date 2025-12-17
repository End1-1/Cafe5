#include "dishitem.h"
#include "ui_dishitem.h"
#include "dlgqty.h"
#include "c5utils.h"
#include <QPaintEvent>
#include <QPainter>
#include <QPushButton>

DishItem::DishItem(const QMap<QString, QVariant>& d, int index, QWidget *parent) :
    QWidget(parent),
    fData(d),
    ui(new Ui::DishItem)
{
    ui->setupUi(this);
    ui->wmodificators->setVisible(false);
    fFocus = false;
    fReadOnly = false;
    ui->btnChecked->setVisible(false);
    QString name = fData["f_dish_name"].toString();

    if(fData["f_fromtable"].toInt() > 0) {
        //TODO
        //name = QString("(%1) %2").arg(dbtable->name(fData["f_fromtable"].toInt()), name);
    }

    ui->btnDish->setText(name);
    setChanges();
    adjustSize();
    QFont f(font());
    f.setBold(true);
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

void DishItem::setChanges()
{
    if(fData["f_state"].toInt() == DISH_STATE_NONE) {
        setVisible(false);
    } else {
        ui->lbEmarks->setVisible(!fData["f_emarks"].toString().isEmpty());
        ui->lbQty1->setText(float_str(fData["f_qty1"].toDouble(), 2));
        ui->lbComment->setText(fData["f_comment2"].toString() + " " + fData["f_comment"].toString());
        ui->lbComment->setVisible(!ui->lbComment->text().isEmpty());
        ui->lbTotal->setText(float_str(fData["f_qty2"].toDouble() * fData["f_price"].toDouble(), 2));

        if(fData["f_state"] == DISH_STATE_OK) {
            ui->lbQty1->setProperty("state", fData["f_qty2"].toDouble() > 0.001 ? "1" : "0");
            ui->lbQty1->style()->polish(ui->lbQty1);
            QDateTime dt = fData["f_qty2"].toDouble() > 0.001 ?
                           str_to_datetime(fData["f_printtime"].toString()) :
                           str_to_datetime(fData["f_appendtime"].toString());
            QString dd = dt.date() == QDate::currentDate() ? dt.toString(FORMAT_TIME_TO_SHORT_STR) :
                         QString("<html><body><p align=\"center\" "
                    "style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"> "
                    "<span style=\" font-size:6pt;\">%1</span></p>\n<p align=\"center\" "
                    "style=\" margin-top:0px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"> "
                    "<span style=\" font-size:8pt;\">%2</span></p></body></html>")
                         .arg(dt.toString(FORMAT_DATE_TO_STR), dt.toString(FORMAT_TIME_TO_STR));
            ui->lbTimeOfDish->setText(fData["f_state"].toInt() == DISH_STATE_OK ? dd : "-");
            ui->orderDishFrame->setProperty("dish_focused", fFocus ? "1" : "2");
            ui->orderDishFrame->style()->polish(ui->orderDishFrame);
        } else if(fData["f_state"] == DISH_STATE_SET) {
            ui->orderDishFrame->setProperty("state", "4");
            ui->orderDishFrame->style()->polish(ui->orderDishFrame);
            ui->lbTimeOfDish->setText(str_to_datetime(fData["f_removetime"].toString()).toString(FORMAT_TIME_TO_SHORT_STR));
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
            ui->lbTimeOfDish->setText(str_to_datetime(fData["f_removetime"].toString()).toString(FORMAT_TIME_TO_SHORT_STR));
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

void DishItem::setCheckMode(bool v)
{
    if(fData["f_state"].toInt() != DISH_STATE_OK
            && fData["f_state"].toInt() != DISH_STATE_SET) {
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

void DishItem::checkFocus(const QString &id)
{
    fFocus = id == fData["f_id"].toString();
    setChanges();
}

bool DishItem::event(QEvent *event)
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

void DishItem::on_btnDish_clicked()
{
    if(ui->btnChecked->isVisible()) {
        return;
    }

    fFocus = true;
    setChanges();
    emit focused(fData["f_id"].toString());
}
