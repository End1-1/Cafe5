#ifndef CR5DEBTSTOPARTNERFILTER_H
#define CR5DEBTSTOPARTNERFILTER_H

#include "c5filterwidget.h"

namespace Ui {
class CR5DebtsToPartnerFilter;
}

class CR5DebtsToPartnerFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5DebtsToPartnerFilter(const QStringList &dbParams);

    ~CR5DebtsToPartnerFilter();

    virtual QString condition() override;

    QDate d1() const;

    QDate d2() const;

    QString currency() const;

private:
    Ui::CR5DebtsToPartnerFilter *ui;
};

#endif // CR5DEBTSTOPARTNERFILTER_H
