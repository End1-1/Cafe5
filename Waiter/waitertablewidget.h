#ifndef WAITERTABLEWIDGET_H
#define WAITERTABLEWIDGET_H

#include <QTableWidget>

class WaiterTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    WaiterTableWidget(QWidget *parent = nullptr);

    int addEmptyRow();

    void setString(int row, int col, const QString &str);

    QString getString(int row, int col) const;

    void setInteger(int row, int col, int value);

    int getInteger(int row, int col);

    void setDouble(int row, int col, double value);

    void configColumns(QList<int> colWidths);

    void search(const QString &txt);
};

#endif // WAITERTABLEWIDGET_H
