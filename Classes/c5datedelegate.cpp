#include "c5datedelegate.h"
#include "c5dateedit.h"
#include "c5tablemodel.h"
#include "format_date.h"

C5DateDelegate::C5DateDelegate(QObject *parent) :
    QItemDelegate(parent)
{
}

QWidget* C5DateDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    C5DateEdit *l = new C5DateEdit(parent);
    return l;
}

void C5DateDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    C5DateEdit *l = static_cast<C5DateEdit*>(editor);
    l->setText(index.data(Qt::DisplayRole).toString());
    l->fOldValue = l->text();
}

void C5DateDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    C5DateEdit *l = static_cast<C5DateEdit*>(editor);
    C5TableModel *m = static_cast<C5TableModel*>(model);
    m->setData(index, QDate::fromString(l->toMySQLDate(false), FORMAT_DATE_TO_STR_MYSQL));

    if(l->text() != l->fOldValue) {
        m->setRowToUpdate(index.row());
        l->fOldValue = l->text();
    }
}
