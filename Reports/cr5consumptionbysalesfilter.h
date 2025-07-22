#ifndef CR5CONSUMPTIONBYSALESFILTER_H
#define CR5CONSUMPTIONBYSALESFILTER_H

#include "c5filterwidget.h"

namespace Ui
{
class CR5ConsumptionBySalesFilter;
}

static const int REPORTTYPE_QUANTITEIS = 1;
static const int REPORTTYPE_AMOUNTS = 2;

class CR5ConsumptionBySalesFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5ConsumptionBySalesFilter(QWidget *parent = nullptr);

    ~CR5ConsumptionBySalesFilter();

    virtual QString condition();

    int store();

    bool salesCounted();

    bool draft();

    QDate date1();

    QDate date2();

    QString group() const;

    int reportType();

private slots:

    void on_chDraft_clicked(bool checked);

private:
    Ui::CR5ConsumptionBySalesFilter *ui;
};

#endif // CR5CONSUMPTIONBYSALESFILTER_H
