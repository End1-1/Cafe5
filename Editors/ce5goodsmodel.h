#ifndef CE5GOODSMODEL_H
#define CE5GOODSMODEL_H

#include "ce5editor.h"

namespace Ui {
class CE5GoodsModel;
}

class CE5GoodsModel : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5GoodsModel(const QStringList &dbParams, QWidget *parent = nullptr);
    ~CE5GoodsModel();
    virtual QString title();

    virtual QString table();

private:
    Ui::CE5GoodsModel *ui;
};

#endif // CE5GOODSMODEL_H
