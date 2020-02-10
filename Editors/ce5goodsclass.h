#ifndef CE5GOODSCLASS_H
#define CE5GOODSCLASS_H

#include "ce5editor.h"

namespace Ui {
class CE5GoodsClass;
}

class CE5GoodsClass : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5GoodsClass(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CE5GoodsClass();

    virtual QString title();

    virtual QString table();

private:
    Ui::CE5GoodsClass *ui;
};

#endif // CE5GOODSCLASS_H
