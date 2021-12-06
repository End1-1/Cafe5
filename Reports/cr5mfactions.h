#ifndef CR5MFACTIONS_H
#define CR5MFACTIONS_H

#include <c5reportwidget.h>

class CR5MfActions : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5MfActions(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar() override;
};

#endif // CR5MFACTIONS_H
