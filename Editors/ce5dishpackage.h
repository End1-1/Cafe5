#ifndef CE5DISHPACKAGE_H
#define CE5DISHPACKAGE_H

#include "ce5editor.h"

namespace Ui {
class CE5DishPackage;
}

class CE5DishPackage : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5DishPackage(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CE5DishPackage();

    virtual QString title() override;

    virtual QString table() override;

private:
    Ui::CE5DishPackage *ui;
};

#endif // CE5DISHPACKAGE_H
