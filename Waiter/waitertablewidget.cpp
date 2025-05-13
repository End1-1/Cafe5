#include "waitertablewidget.h"
#include "c5utils.h"

WaiterTableWidget::WaiterTableWidget(QWidget *parent) :
    QTableWidget(parent) {}

int WaiterTableWidget::addEmptyRow()
{
    int row = rowCount();
    setRowCount(row + 1);
    for (int i = 0, count = columnCount(); i < count; i++) {
        setItem(row, i, new QTableWidgetItem());
    }
    return row;
}

void WaiterTableWidget::setString(int row, int col, const QString &str)
{
    auto i = item(row, col);
    if (!i) {
        i = new QTableWidgetItem();
        setItem(row, col, i);
    }
    i->setText(str);
}

QString WaiterTableWidget::getString(int row, int col) const
{
    auto  i = item(row, col);
    if (i) {
        return i->text();
    }
    return "INVALID ITEM";
}

void WaiterTableWidget::setInteger(int row, int col, int value)
{
    setString(row, col, QString::number(value));
}

int WaiterTableWidget::getInteger(int row, int col)
{
    return getString(row, col).toInt();
}

void WaiterTableWidget::setDouble(int row, int col, double value)
{
    setString(row, col, float_str(value, 2));
}

void WaiterTableWidget::configColumns(QList<int> colWidths)
{
    for (int i = 0; i < colWidths.size(); i++) {
        setColumnWidth(i, colWidths.at(i));
    }
}

void WaiterTableWidget::search(const QString &txt)
{
    for (int i = 0; i < rowCount(); i++) {
        bool hidden = true;
        for (int j = 0; j < columnCount(); j++) {
            if (getString(i, j).contains(txt, Qt::CaseInsensitive)) {
                hidden = false;
            }
        }
        setRowHidden(i, hidden);
    }
}
