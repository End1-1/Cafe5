#ifndef CR5DEBTSTOPARTNER_H
#define CR5DEBTSTOPARTNER_H

#include "c5reportwidget.h"

class CR5DebtsToPartner : public C5ReportWidget
{
    Q_OBJECT
public:
    CR5DebtsToPartner(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar();

private slots:
    void doubleClick(int row, int column, const QList<QVariant> &vals);
};

#endif // CR5DEBTSTOPARTNER_H
