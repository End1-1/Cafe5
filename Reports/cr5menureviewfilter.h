#ifndef CR5MENUREVIEWFILTER_H
#define CR5MENUREVIEWFILTER_H

#include "c5filterwidget.h"

namespace Ui {
class CR5MenuReviewFilter;
}

class CR5MenuReviewFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5MenuReviewFilter(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CR5MenuReviewFilter();

    virtual QString condition() override;

private:
    Ui::CR5MenuReviewFilter *ui;
};

#endif // CR5MENUREVIEWFILTER_H
