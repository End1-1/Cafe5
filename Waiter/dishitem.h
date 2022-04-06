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

    void setCheckMode(bool v);

    void setChecked(bool v);

    bool isChecked();

    void setReadyOnly(bool v);

protected:
    virtual bool event(QEvent *event);

private slots:
    void on_btnDish_clicked();

private:
    Ui::DishItem *ui;

    C5OrderDriver *fOrder;

    int fIndex;

    bool fFocus;

    bool fReadOnly;

signals:
    void focused(int);
};

#endif // DISHITEM_H
