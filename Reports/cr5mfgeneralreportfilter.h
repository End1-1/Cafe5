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
    explicit CR5MFGeneralReportFilter(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CR5MFGeneralReportFilter();

    virtual QString condition();

private:
    Ui::CR5MFGeneralReportFilter *ui;
};

#endif // CR5MFGENERALREPORTFILTER_H
