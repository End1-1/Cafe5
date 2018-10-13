#include "c5combodelegate.h"
#include "c5cache.h"
#include "c5tablemodel.h"
#include "c5combobox.h"

C5ComboDelegate::C5ComboDelegate(const QString &field, C5Cache *cache, QObject *parent) :
    QItemDelegate(parent)
{
    fCache = cache;
    fField = field;
}

QWidget *C5ComboDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    C5ComboBox *combo = new C5ComboBox(parent);
    for (int i = 0, count = fCache->rowCount(); i < count; i++) {
        combo->addItem(fCache->getString(i, 1), fCache->getInt(i, 0));
    }
    return combo;
}

void C5ComboDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    C5ComboBox *combo = static_cast<C5ComboBox*>(editor);
    int idx = combo->findText(index.data(Qt::DisplayRole).toString());
    combo->setCurrentIndex(idx);
    combo->fOldIndex = idx;
}

void C5ComboDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    C5ComboBox *combo = static_cast<C5ComboBox*>(editor);
    C5TableModel *m = static_cast<C5TableModel*>(model);
    m->setData(index, combo->currentText());
    if (combo->currentIndex() != combo->fOldIndex) {
        m->setRowToUpdate(index.row(), fField, combo->currentData());
    }
}

void C5ComboDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}
