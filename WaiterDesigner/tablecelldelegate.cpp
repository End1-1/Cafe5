#include "tablecelldelegate.h"

#include <QPainter>
#include <QTableWidgetItem>
#ifdef WAITER
#include "c5utils.h"
#endif

namespace {

QColor backgroundForState(const WaiterTblTablesStyle &style, int state)
{
    switch (state) {
    case 2:
        return style.colorOrder;
    case 3:
        return style.colorPrecheck;
    default:
        return style.colorEmpty;
    }
}

} // namespace

TableCellDelegate::TableCellDelegate(const WaiterTblTablesStyle &style, QObject *parent)
    : QStyledItemDelegate(parent)
    , mStyle(style)
{
}

void TableCellDelegate::setStyle(const WaiterTblTablesStyle &style)
{
    mStyle = style;
}

void TableCellDelegate::paint(QPainter *p,
                              const QStyleOptionViewItem &opt,
                              const QModelIndex &idx) const
{
    p->save();

    const int inset = qMax(0, mStyle.borderWidth);
    QRect r = opt.rect.adjusted(inset, inset, -inset, -inset);

    const int id = idx.data(WaiterRoleId).toInt();
    const int state = idx.data(WaiterRoleState).toInt();
    const QColor bg = backgroundForState(mStyle, state);

    if (id == 0) {
        p->fillRect(r, bg);
        p->restore();
        return;
    }

    p->fillRect(r, bg);

    const QString name = idx.data(WaiterRoleName).toString();
    const QString staff = idx.data(WaiterRoleStaff).toString();
    const double amount = idx.data(WaiterRoleAmount).toDouble();
    const int pad = 6;

    QFont f = opt.font;
    f.setBold(true);
    QRect nameRect(r.left() + pad, r.top() + pad, r.width() / 2, 18);

    int fontSize = 14;
    const int minFontSize = 8;
    QFontMetrics fm(f);
    while (fontSize >= minFontSize) {
        f.setPointSize(fontSize);
        fm = QFontMetrics(f);
        if (fm.horizontalAdvance(name) <= nameRect.width()) {
            break;
        }
        --fontSize;
    }

    p->setFont(f);
    p->setPen(Qt::black);
    p->drawText(nameRect, Qt::AlignCenter | Qt::AlignVCenter, name);

    f.setPointSize(10);
    f.setBold(true);
    p->setFont(f);

    if (amount > 0.01) {
        const QString amountText =
#ifdef WAITER
            float_str(amount, 2);
#else
            QString::number(amount, 'f', 2);
#endif
        p->drawText(QRect(r.center().x(), r.top() + pad, r.width() / 2 - pad, 18),
                    Qt::AlignRight | Qt::AlignVCenter,
                    amountText);
    }

    f.setBold(false);
    p->setFont(f);
    p->drawText(QRect(r.left() + pad, r.top() + pad + 20, r.width() - pad * 2, 18),
                Qt::AlignLeft | Qt::AlignVCenter,
                staff);
    p->restore();
}

QSize TableCellDelegate::sizeHint(const QStyleOptionViewItem &opt, const QModelIndex &idx) const
{
    Q_UNUSED(opt);
    Q_UNUSED(idx);
    return QSize(200, 56);
}
