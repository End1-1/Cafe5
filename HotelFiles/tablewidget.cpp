#include "tablewidget.h"
#include "tablewidgetitem.h"
#include "tablewidgetitem.h"

TableWidget::TableWidget(QWidget *parent) :
    QTableWidget(parent)
{
    setEditTriggers(EditTrigger::NoEditTriggers);
}

TableWidgetItem *TableWidget::item(int row, int column)
{
    TableWidgetItem *i = static_cast<TableWidgetItem*>(QTableWidget::item(row, column));
    if (!i) {
        i = new TableWidgetItem();
        setItem(row, column, i);
    }
    return i;
}

int TableWidget::addEmptyRow()
{
    setRowCount(rowCount() + 1);
    return rowCount() - 1;
}

void TableWidget::setItem(int row, int column, TableWidgetItem *item)
{
    QTableWidget::setItem(row, column, item);
}

void TableWidget::setString(int row, int column, const QString &s)
{
    item(row, column)->setData(Qt::EditRole, s);
}

void TableWidget::setInteger(int row, int column, int v)
{
    item(row, column)->setData(Qt::EditRole, v);
}

int TableWidget::getInteger(int row, int column)
{
    return item(row, column)->data(Qt::EditRole).toInt();
}
