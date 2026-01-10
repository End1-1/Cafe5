#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include "c5widget.h"
#include "struct_table.h"

namespace Ui
{
class TableWidget;
}

class QLabel;
class QFrame;

class TableWidget : public C5Widget
{
    Q_OBJECT

public:
    explicit TableWidget(QWidget *parent = nullptr);

    ~TableWidget();

    void setTable(const TableItem &t);

    void updateTable(const TableItem &t);

    TableItem mTable;

protected:
    virtual void mouseReleaseEvent(QMouseEvent *me) override;

private:
    Ui::TableWidget* ui;

signals:
    void clicked(int);
};

#endif // TABLEWIDGET_H
