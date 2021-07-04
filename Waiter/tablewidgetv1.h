#ifndef TABLEWIDGETV1_H
#define TABLEWIDGETV1_H

#include <QWidget>

namespace Ui {
class TableWidgetV1;
}

class TableWidgetV1 : public QWidget
{
    Q_OBJECT

public:
    explicit TableWidgetV1(QWidget *parent = nullptr);

    ~TableWidgetV1();

    int fTable;

    void config(int id);

    void configOrder(const QString &orderid);

    virtual void mouseReleaseEvent(QMouseEvent *me);

private:
    Ui::TableWidgetV1 *ui;

signals:
    void clicked(int);
};

#endif // TABLEWIDGETV1_H
