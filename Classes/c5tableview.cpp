#include "c5tableview.h"
#include "c5tablemodel.h"

C5TableView::C5TableView(QWidget *parent)  :
    QTableView(parent)
{
}

void C5TableView::setItemDelegateForColumn(int column, QAbstractItemDelegate *delegate)
{
    auto *m = static_cast<C5TableModel*>(model());
    m->setEditableFlag(column, Qt::ItemIsEditable);
    QTableView::setItemDelegateForColumn(column, delegate);
}
