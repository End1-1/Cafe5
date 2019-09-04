#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QTableWidget>

class TableWidgetItem;

class TableWidget : public QTableWidget
{
public:
    TableWidget(QWidget *parent = nullptr);
    TableWidgetItem *item(int row, int column);
    int addEmptyRow();
    void setItem(int row, int column, TableWidgetItem *item);
    void setString(int row, int column, const QString &s);
    void setInteger(int row, int column, int v);
    int getInteger(int row, int column);
};

#endif // TABLEWIDGET_H
