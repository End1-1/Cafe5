#ifndef CE5CURRENCYRATE_H
#define CE5CURRENCYRATE_H

#include "ce5editor.h"

namespace Ui {
class CE5CurrencyRate;
}

class CE5CurrencyRate : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5CurrencyRate(QWidget *parent = nullptr);
    ~CE5CurrencyRate();
    virtual QString title();
    virtual QString table();
    virtual bool save(QString &err, QList<QMap<QString, QVariant> > &data) override;

private:
    Ui::CE5CurrencyRate *ui;
};

#endif // CE5CURRENCYRATE_H
