#pragma once
#include <QTableView>

class QAbstractItemView;

class C5TableView : public QTableView
{
    Q_OBJECT
public:
    C5TableView(QWidget *parent = nullptr);

    void setItemDelegateForColumn(int column, QAbstractItemDelegate *delegate);

    void copySelection();

    static void copySelectionFrom(QAbstractItemView *view);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
};
