#ifndef CR5SALEFROMSTORETOTALFILTER_H
#define CR5SALEFROMSTORETOTALFILTER_H

#include "c5filterwidget.h"

namespace Ui {
class CR5SaleFromStoreTotalFilter;
}

class CR5SaleFromStoreTotalFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5SaleFromStoreTotalFilter(QWidget *parent = nullptr);

    ~CR5SaleFromStoreTotalFilter();

    virtual QString condition() override;

    virtual QString filterText() override;

    int store();

    QDate start() const;

    QDate end() const;

    bool showQty();

    bool showAmount();

private:
    Ui::CR5SaleFromStoreTotalFilter *ui;
};

#endif // CR5SALEFROMSTORETOTALFILTER_H
