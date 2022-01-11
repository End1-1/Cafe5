#ifndef TABLEWIDGETV2_H
#define TABLEWIDGETV2_H

#include "tablewidget.h"

namespace Ui {
class TableWidgetV2;
}

class TableWidgetV2 : public TableWidget
{
    Q_OBJECT

public:
    explicit TableWidgetV2(QWidget *parent = nullptr);

    ~TableWidgetV2();

    QLabel *labelTable();

    QLabel *labelStaff();

    QLabel *labelTime();

    QLabel *labelAmount();

    QLabel *labelComment();

    QFrame *frame();

    virtual void configOrder(const QString &orderid);

private:
    Ui::TableWidgetV2 *ui;
};

#endif // TABLEWIDGETV2_H
