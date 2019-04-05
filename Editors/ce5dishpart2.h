#ifndef CE5DISHPART2_H
#define CE5DISHPART2_H

#include "ce5editor.h"

namespace Ui {
class CE5DishPart2;
}

class CE5DishPart2 : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5DishPart2(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CE5DishPart2();

    virtual QString title();

    virtual QString table();

private:
    Ui::CE5DishPart2 *ui;

private slots:
    void selectColor();

    void on_btnNewPart_clicked();
};

#endif // CE5DISHPART2_H
