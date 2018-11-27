#ifndef CE5GOODSWASTE_H
#define CE5GOODSWASTE_H

#include "ce5editor.h"

namespace Ui {
class CE5GoodsWaste;
}

class CE5GoodsWaste : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5GoodsWaste(const QStringList &dbParams, QWidget *parent = 0);

    ~CE5GoodsWaste();

    virtual QString title() {return tr("Autowaste"); }

    virtual QString table() {return "c_goods_waste"; }

private:
    Ui::CE5GoodsWaste *ui;
};

#endif // CE5GOODSWASTE_H
