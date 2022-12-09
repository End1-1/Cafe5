#ifndef CE5CURRENCYCROSSRATERECORD_H
#define CE5CURRENCYCROSSRATERECORD_H

#include "ce5editor.h"

namespace Ui {
class CE5CurrencyCrossRateRecord;
}

class CE5CurrencyCrossRateRecord : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5CurrencyCrossRateRecord(const QStringList &dbParams, QWidget *parent = nullptr);
    ~CE5CurrencyCrossRateRecord();
    virtual QString title() override;
    virtual QString table() override;

public slots:
    virtual bool save(QString &err, QList<QMap<QString, QVariant> > &data) override;


private:
    Ui::CE5CurrencyCrossRateRecord *ui;
};

#endif // CE5CURRENCYCROSSRATERECORD_H
