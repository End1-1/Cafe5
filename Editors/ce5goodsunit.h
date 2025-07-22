#ifndef CE5GOODSUNIT_H
#define CE5GOODSUNIT_H

#include "ce5editor.h"

namespace Ui {
class CE5GoodsUnit;
}

class CE5GoodsUnit : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5GoodsUnit(QWidget *parent = nullptr);

    ~CE5GoodsUnit();

    virtual QString title();

    virtual QString table();

private:
    Ui::CE5GoodsUnit *ui;
};

#endif // CE5GOODSUNIT_H
