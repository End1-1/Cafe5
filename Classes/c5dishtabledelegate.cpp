#include "c5dishtabledelegate.h"
#include "c5menu.h"
#include "c5utils.h"
#include <QJsonObject>
#include <QPainter>
#include <QApplication>

C5DishTableDelegate::C5DishTableDelegate()
{

}

void C5DishTableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid()) {
        painter->fillRect(option.rect, Qt::white);
        return;
    }
    QJsonObject o = index.data(Qt::UserRole).toJsonObject();
    if (o.count() == 0) {
        QItemDelegate::paint(painter, option, index);
        return;
    }
    bool stop = C5Menu::fStopList.contains(o["f_dish"].toString().toInt());
    bool stopreached = false;
    QString name = o["f_name"].toString();
    if (stop) {
        double qty = C5Menu::fStopList[o["f_dish"].toString().toInt()];
        QString sqty = qty > 0.01 ? float_str(qty, 1) : "x";
        if (sqty == "x") {
            stopreached = true;
        }
        name = QString("[%1] %2").arg(sqty).arg(name);
    }

    painter->save();
    QFont f(qApp->font());
    f.setBold(true);
    painter->setFont(f);
    painter->fillRect(option.rect, stopreached ? qRgba(0xee,0xee,0xee, 0) : QColor::fromRgb(o["dish_color"].toString().toInt()));
    QRect rectName = option.rect;
    rectName.adjust(2, 2, -2, -2);
    painter->drawText(rectName, name);
    QRect rectPrice = option.rect;
    QFontMetrics fm(painter->font());
    QString price = o["f_price"].toString();
    rectPrice.adjust(rectPrice.width() - (fm.width(price) + 5), rectPrice.height() - (fm.height() + 5), -2, -2);
    painter->drawText(rectPrice, price);
    painter->restore();
}
