#ifndef C5TABLEVIEW_H
#define C5TABLEVIEW_H

#include <QTableView>

class C5TableView : public QTableView
{
    Q_OBJECT

public:
    C5TableView(QWidget *parent);

    void setItemDelegateForColumn(int column, QAbstractItemDelegate *delegate);
};

#endif // C5TABLEVIEW_H
