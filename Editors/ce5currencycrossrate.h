#ifndef CE5CURRENCYCROSSRATE_H
#define CE5CURRENCYCROSSRATE_H

#include "ce5editor.h"

namespace Ui {
class CE5CurrencyCrossRate;
}

class CE5CurrencyCrossRate : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5CurrencyCrossRate(const QStringList &dbParams, QWidget *parent = nullptr);
    ~CE5CurrencyCrossRate();
    virtual QString title() override;
    virtual QString table() override;

private:
    Ui::CE5CurrencyCrossRate *ui;
};

#endif // CE5CURRENCYCROSSRATE_H
