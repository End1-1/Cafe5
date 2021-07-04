#ifndef QDISHPART2BUTTON_H
#define QDISHPART2BUTTON_H

#include <QPushButton>

class QDishPart2Button : public QPushButton
{
    Q_OBJECT
public:
    QDishPart2Button(QWidget *parent = nullptr);

protected:
    virtual void paintEvent(QPaintEvent *pe) override;
};

#endif // QDISHPART2BUTTON_H
