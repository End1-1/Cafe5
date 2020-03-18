#ifndef CR5DEBTSTOPARTNER_H
#define CR5DEBTSTOPARTNER_H

#include "c5reportwidget.h"

class CR5DebtsToPartner : public C5ReportWidget
{
    Q_OBJECT
public:
    CR5DebtsToPartner(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar();

protected:
    virtual bool tblDoubleClicked(int row, int column, const QList<QVariant> &vals) override;
};

#endif // CR5DEBTSTOPARTNER_H
