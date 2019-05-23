#ifndef DISHTABLEITEMDELEGATE_H
#define DISHTABLEITEMDELEGATE_H

#include <QItemDelegate>

class DishTableItemDelegate : public QItemDelegate
{
public:
    DishTableItemDelegate();

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // DISHTABLEITEMDELEGATE_H
