#ifndef CR5CONSUPTIONREASONFILTER_H
#define CR5CONSUPTIONREASONFILTER_H

#include "c5filterwidget.h"

namespace Ui {
class CR5ConsuptionReasonFilter;
}

class CR5ConsuptionReasonFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5ConsuptionReasonFilter(QWidget *parent = nullptr);

    ~CR5ConsuptionReasonFilter();

    virtual QString condition() override;

    virtual QString filterText() override;

    void setDateFilter(const QDate &d1, const QDate &d2);

    void setStoreFilter(int store);

    void setGoodsFilter(int goods);

private:
    Ui::CR5ConsuptionReasonFilter *ui;
};

#endif // CR5CONSUPTIONREASONFILTER_H
