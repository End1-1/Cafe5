#pragma once
#include <QTableView>

class C5TableView : public QTableView
{
    Q_OBJECT
public:
    C5TableView(QWidget *parent = nullptr);

    void setItemDelegateForColumn(int column, QAbstractItemDelegate *delegate);

};
