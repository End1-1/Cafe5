#ifndef C5TEXTDELEGATE_H
#define C5TEXTDELEGATE_H

#include <QItemDelegate>

class C5TextDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    C5TextDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // C5TEXTDELEGATE_H
