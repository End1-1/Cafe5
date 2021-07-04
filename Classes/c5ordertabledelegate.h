#ifndef C5ORDERTABLEDELEGATE_H
#define C5ORDERTABLEDELEGATE_H

#include <QItemDelegate>

class C5OrderDriver;

class C5OrderTableDelegate : public QItemDelegate
{
public:
    C5OrderTableDelegate(C5OrderDriver *od);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    C5OrderDriver *fOrder;
};

#endif // C5ORDERTABLEDELEGATE_H
