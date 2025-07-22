#ifndef CR5MFGENERALREPORTFILTER_H
#define CR5MFGENERALREPORTFILTER_H

#include "c5filterwidget.h"

namespace Ui {
class CR5MFGeneralReportFilter;
}

class CR5MFGeneralReportFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5MFGeneralReportFilter(QWidget *parent = nullptr);

    ~CR5MFGeneralReportFilter();

    virtual QString condition();

    virtual QString conditionText();

    QString d1ms() const;

    QString d2ms() const;

    QString d1s() const;

    QString d2s() const;

private:
    Ui::CR5MFGeneralReportFilter *ui;
};

#endif // CR5MFGENERALREPORTFILTER_H
