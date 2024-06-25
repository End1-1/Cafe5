#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QWidget>
#include <QJsonObject>

class QLabel;
class QFrame;

class TableWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TableWidget(QWidget *parent = nullptr);

    virtual void configOrder(const QJsonObject &jo);

    virtual QLabel *labelTable() = 0;

    virtual QLabel *labelStaff() = 0;

    virtual QLabel *labelTime() = 0;

    virtual QLabel *labelAmount() = 0;

    virtual QLabel *labelComment() = 0;

    virtual QFrame *frame() = 0;

    void config(int id);

    virtual void mouseReleaseEvent(QMouseEvent *me);

    int fTable;

signals:
    void clicked(int);

};

#endif // TABLEWIDGET_H
