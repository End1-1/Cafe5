#include "c5textdelegate.h"
#include "c5tablemodel.h"
#include "c5lineedit.h"

C5TextDelegate::C5TextDelegate(QObject *parent) :
    QItemDelegate(parent)
{
    fValidator = 0;
}

QWidget *C5TextDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    C5LineEdit *l = new C5LineEdit(parent);
    if (fValidator) {
        l->setValidator(fValidator);
    }
    return l;
}

void C5TextDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    C5LineEdit *l = static_cast<C5LineEdit*>(editor);
    l->setText(index.data(Qt::DisplayRole).toString());
    l->fOldValue = l->text();
}

void C5TextDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    C5LineEdit *l = static_cast<C5LineEdit*>(editor);
    C5TableModel *m = static_cast<C5TableModel*>(model);
    m->setData(index, l->text());
    if (l->text() != l->fOldValue) {
        m->setRowToUpdate(index.row());
        l->fOldValue = l->text();
    }
}

void C5TextDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}

void C5TextDelegate::setValidator(QValidator *v)
{
    fValidator = v;
}
