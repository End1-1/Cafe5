#ifndef DISHITEMDELEGATE_H
#define DISHITEMDELEGATE_H

#include <QItemDelegate>

class DishItemDelegate : public QItemDelegate
{
public:
    DishItemDelegate();

    // QAbstractItemDelegate interface
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // DISHITEMDELEGATE_H
