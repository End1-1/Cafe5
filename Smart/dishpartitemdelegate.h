#ifndef DISHPARTITEMDELEGATE_H
#define DISHPARTITEMDELEGATE_H

#include <QItemDelegate>

class DishPartItemDelegate : public QItemDelegate
{
public:
    DishPartItemDelegate();

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // DISHPARTITEMDELEGATE_H
