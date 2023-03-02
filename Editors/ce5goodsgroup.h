#ifndef CE5GOODSGROUP_H
#define CE5GOODSGROUP_H

#include "ce5editor.h"

namespace Ui {
class CE5GoodsGroup;
}

class CE5GoodsGroup : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5GoodsGroup(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CE5GoodsGroup();

    virtual QString title() {return tr("Goods group"); }

    virtual QString table() {return "c_groups"; }

private slots:
    void on_btnSetStoreReminderQty_clicked();

private:
    Ui::CE5GoodsGroup *ui;
};

#endif // CE5GOODSGROUP_H
