#ifndef CR5CONSUMPTIONBYSALESFILTERDRAFT_H
#define CR5CONSUMPTIONBYSALESFILTERDRAFT_H

#include "c5filterwidget.h"

namespace Ui {
class CR5ConsumptionBySalesFilterDraft;
}

class CR5ConsumptionBySalesFilterDraft : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5ConsumptionBySalesFilterDraft(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CR5ConsumptionBySalesFilterDraft();

    virtual QString condition();

    int store();

    bool salesCounted();

    QDate date1();

    QDate date2();

    QString group() const;

    QString class1() const;

    QString class2() const;

    QString class3() const;

    QString class4() const;

private:
    Ui::CR5ConsumptionBySalesFilterDraft *ui;
};

#endif // CR5CONSUMPTIONBYSALESFILTERDRAFT_H
