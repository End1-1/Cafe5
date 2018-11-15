#ifndef C5DATEDELEGATE_H
#define C5DATEDELEGATE_H

#include <QItemDelegate>

class C5DateDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    C5DateDelegate(QObject *parent = 0);

protected:
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

};

#endif // C5DATEDELEGATE_H
