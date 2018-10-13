#ifndef C5COMBODELEGATE_H
#define C5COMBODELEGATE_H

#include <QItemDelegate>

class C5Cache;

class C5ComboDelegate : public QItemDelegate
{
public:
    C5ComboDelegate(const QString &field, C5Cache *cache, QObject *parent);

    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;

    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    C5Cache *fCache;

    QString fField;
};

#endif // C5COMBODELEGATE_H
