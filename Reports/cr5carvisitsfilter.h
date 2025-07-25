#ifndef CR5CARVISITSFILTER_H
#define CR5CARVISITSFILTER_H

#include "c5filterwidget.h"

namespace Ui {
class CR5CarVisitsFilter;
}

class CR5CarVisitsFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5CarVisitsFilter(QWidget *parent = nullptr);

    ~CR5CarVisitsFilter();

    virtual QString condition() override;

    virtual QString filterText() override;

private:
    Ui::CR5CarVisitsFilter *ui;
};

#endif // CR5CARVISITSFILTER_H
