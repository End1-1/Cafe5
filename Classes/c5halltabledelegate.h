#ifndef C5HALLTABLEDELEGATE_H
#define C5HALLTABLEDELEGATE_H

#include <QItemDelegate>

class C5HallTableDelegate : public QItemDelegate
{
public:
    C5HallTableDelegate();

protected:
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // C5HALLTABLEDELEGATE_H
