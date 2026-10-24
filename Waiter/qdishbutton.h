#ifndef QDISHBUTTON_H
#define QDISHBUTTON_H

#include "struct_dish.h"
#include <QFrame>
#include <QPixmap>

class QDishButton : public QFrame
{
    Q_OBJECT

public:
    QDishButton(DishAItem *dish, int minWidth, QWidget *parent = nullptr);

protected:
    virtual void paintEvent(QPaintEvent *pe) override;

    virtual void mouseReleaseEvent(QMouseEvent *e) override;

private:
    void paintNewLook(QPaintEvent *pe);

    DishAItem* mDish;

    QPixmap mCardPhoto;

    bool mCardPhotoDecoded = false;

signals:
    void clicked();
};

#endif // QDISHBUTTON_H
