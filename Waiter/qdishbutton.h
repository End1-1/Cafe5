#ifndef QDISHBUTTON_H
#define QDISHBUTTON_H

#include "struct_dish.h"
#include <QFrame>

class QDishButton : public QFrame
{
    Q_OBJECT

public:
    QDishButton(DishAItem *dish, int minWidth, QWidget *parent = nullptr);

protected:
    virtual void paintEvent(QPaintEvent *pe) override;

    virtual void mouseReleaseEvent(QMouseEvent *e) override;

private:
    DishAItem* mDish;

signals:
    void clicked();
};

#endif // QDISHBUTTON_H
