#pragma once

#include "ce5editor.h"

namespace Ui
{
class CE5GoodsGroup;
}

class CE5GoodsGroup : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5GoodsGroup(QWidget *parent = nullptr);

    ~CE5GoodsGroup();

    virtual QString title() {return tr("Goods group"); }

    virtual QString table() {return "c_groups"; }

private:
    Ui::CE5GoodsGroup* ui;

private slots:
    void setColor();
};
