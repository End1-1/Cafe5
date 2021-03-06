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
    painter->save();
    QJsonObject o = index.data(Qt::UserRole).toJsonObject();
    QBrush bgBrush(QColor::fromRgb(217, 217, 217), Qt::SolidPattern);
    if (!o["f_header"].toString().isEmpty()) {
        bgBrush.setColor(QColor::fromRgb(130, 255, 100));
    }
    if (o["f_print"].toString().toInt() > 0) {
        bgBrush.setColor(Qt::yellow);
    }
    if (o["f_lock"].toString().toInt() > 0) {
        bgBrush.setColor(QColor::fromRgb(255, 151, 151));
    }
    QFont font(painter->font());
    if (o["f_lock"].toString().toInt() > 0) {
        bgBrush.setColor(QColor::fromRgb(255, 151, 151));
    }
    QRect fillRect = option.rect;
    fillRect.adjust(2, 2, -2, -2);
    painter->fillRect(fillRect, bgBrush);
    QRect tableRect = fillRect;
    tableRect.adjust(4, 2, -2, -2);
    QString amount = o["f_amount"].toString().toDouble() == 0.00 ? "" : " / " + o["f_amount"].toString();
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
    if (!o["f_govnumber"].toString().isEmpty()) {
        QRect govRect = option.rect;
        govRect.adjust(0, govRect.height() - fm.height() - 10, -2, -2);
        op.setAlignment(Qt::AlignRight);
        font.setBold(true);
        font.setPointSize(12);
        painter->setFont(font);
        painter->drawText(govRect, o["f_govnumber"].toString(), op);
    }
    painter->restore();
}
