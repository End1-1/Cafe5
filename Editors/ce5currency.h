#ifndef CE5CURRENCY_H
#define CE5CURRENCY_H

#include "ce5editor.h"

namespace Ui {
class CE5Currency;
}

class CE5Currency : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5Currency(const QStringList &dbParams, QWidget *parent = nullptr);
    ~CE5Currency();
    virtual QString title() override;
    virtual QString table() override;

private:
    Ui::CE5Currency *ui;
};

#endif // CE5CURRENCY_H
