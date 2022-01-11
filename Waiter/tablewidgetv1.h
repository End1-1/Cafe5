#ifndef TABLEWIDGETV1_H
#define TABLEWIDGETV1_H

#include "tablewidget.h"

namespace Ui {
class TableWidgetV1;
}

class TableWidgetV1 : public TableWidget
{
    Q_OBJECT

public:
    explicit TableWidgetV1(QWidget *parent = nullptr);

    ~TableWidgetV1();

    QLabel *labelTable();

    QLabel *labelStaff();

    QLabel *labelTime();

    QLabel *labelAmount();

    QLabel *labelComment();

    QFrame *frame();

private:
    Ui::TableWidgetV1 *ui;
};

#endif // TABLEWIDGETV1_H
