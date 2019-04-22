#ifndef CR5MENUNAMES_H
#define CR5MENUNAMES_H

#include "c5reportwidget.h"

class CR5MenuNames : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5MenuNames(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar();
};

#endif // CR5MENUNAMES_H
