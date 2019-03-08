#ifndef C5PART2TABLEDELEGATE_H
#define C5PART2TABLEDELEGATE_H

#include <QItemDelegate>

class C5Part2TableDelegate : public QItemDelegate
{
public:
    C5Part2TableDelegate();

protected:
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // C5PART2TABLEDELEGATE_H
