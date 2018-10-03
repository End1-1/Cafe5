#ifndef C5ORDERTABLEDELEGATE_H
#define C5ORDERTABLEDELEGATE_H

#include <QItemDelegate>

class C5OrderTableDelegate : public QItemDelegate
{
public:
    C5OrderTableDelegate();

protected:
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // C5ORDERTABLEDELEGATE_H
