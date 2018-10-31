#ifndef CR5COMMONSALESFILTER_H
#define CR5COMMONSALESFILTER_H

#include "c5filterwidget.h"

namespace Ui {
class CR5CommonSalesFilter;
}

class CR5CommonSalesFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5CommonSalesFilter(const QStringList &dbParams, QWidget *parent = 0);

    ~CR5CommonSalesFilter();

    virtual QString condition();

private:
    Ui::CR5CommonSalesFilter *ui;
};

#endif // CR5COMMONSALESFILTER_H
