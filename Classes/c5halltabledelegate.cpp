#include "c5halltabledelegate.h"
#include <QJsonObject>
#include <QPainter>

C5HallTableDelegate::C5HallTableDelegate()
{

}

void C5HallTableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid()) {
        return;
    }
<<<<<<< HEAD
    painter->save();
    QJsonObject o = index.data(Qt::UserRole).toJsonObject();
    QBrush bgBrush(QColor::fromRgb(217, 217, 217), Qt::SolidPattern);
    if (o["f_header"].toString().toInt()) {
        bgBrush.setColor(QColor::fromRgb(130, 255, 100));
    }
    if (o["f_lock"].toString().toInt() > 0) {
        bgBrush.setColor(QColor::fromRgb(255, 151, 151));
    }
    QFont font(painter->font());
=======
    QJsonObject o = index.data(Qt::UserRole).toJsonObject();
    QBrush bgBrush(Qt::white, Qt::SolidPattern);
    if (o["f_lock"].toString().toInt() > 0) {
        bgBrush.setColor(QColor::fromRgb(255, 151, 151));
    }
>>>>>>> 838f31771d5f7dd82bf2f9d4a1b63c78fc2269eb
    QRect fillRect = option.rect;
    fillRect.adjust(2, 2, -2, -2);
    painter->fillRect(fillRect, bgBrush);
    QRect tableRect = fillRect;
<<<<<<< HEAD
    tableRect.adjust(4, 2, -2, -2);
    QString amount = o["f_amount"].toString() == "0" ? "" : " / " + o["f_amount"].toString();
    painter->drawText(tableRect, o["f_name"].toString() + amount);
    QRect staffRect = option.rect;
    font.setBold(true);
    font.setPointSize(8);
    painter->setFont(font);
    QFontMetrics fm(font);
    staffRect.adjust(4, staffRect.height() - fm.height() - 5, -2, -2);
    painter->drawText(staffRect, o["f_staffname"].toString());
    font.setBold(false);
    painter->setFont(font);
    QTextOption op;
    op.setAlignment(Qt::AlignRight);
    QRect dateRect = option.rect;
    dateRect.adjust(dateRect.width() - 100, 2, -5, -2);
    painter->drawText(dateRect, o["f_dateopen"].toString(), op);
    QRect timeRect = option.rect;
    fm = QFontMetrics(font);
    timeRect.adjust(timeRect.width() - 100, fm.height(), -5, -2);
    painter->drawText(timeRect, o["f_timeopen"].toString(), op);
    painter->restore();
=======
    tableRect.adjust(2, 2, -2, -2);
    painter->drawText(tableRect, o["f_name"].toString());
>>>>>>> 838f31771d5f7dd82bf2f9d4a1b63c78fc2269eb
}
