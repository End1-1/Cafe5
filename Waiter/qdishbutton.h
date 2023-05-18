#ifndef QDISHBUTTON_H
#define QDISHBUTTON_H

#include <QPushButton>

class QDishButton : public QPushButton
{
public:
    QDishButton(int minWidth, QWidget *parent = nullptr);

protected:
    virtual void paintEvent(QPaintEvent *pe) override;
};

#endif // QDISHBUTTON_H
