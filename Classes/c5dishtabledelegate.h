#ifndef C5DISHTABLEDELEGATE_H
#define C5DISHTABLEDELEGATE_H

#include <QItemDelegate>

class C5DishTableDelegate : public QItemDelegate
{
public:
    C5DishTableDelegate();

protected:
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // C5DISHTABLEDELEGATE_H
