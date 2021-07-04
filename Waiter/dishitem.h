#ifndef DISHITEM_H
#define DISHITEM_H

#include <QWidget>
#include <QProxyStyle>

namespace Ui {
class DishItem;
}

class C5OrderDriver;

class DishItem : public QWidget
{
    Q_OBJECT

public:
    explicit DishItem(C5OrderDriver *od, int index, QWidget *parent = nullptr);

    ~DishItem();

    void clearFocus(int index);

    void setChanges();

    bool isFocused();

    int index();

    void setReadyOnly(bool v);

private slots:
    void on_btnName_clicked();

    void on_btnPlus1_clicked();

    void on_btnMinus1_clicked();

    void on_btnAnyqty_clicked();

private:
    Ui::DishItem *ui;

    C5OrderDriver *fOrder;

    int fIndex;

    bool fFocus;

    bool fReadOnly;

signals:
    void focused(int);

    void changeQty(double);
};

#endif // DISHITEM_H
