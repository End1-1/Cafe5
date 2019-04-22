#ifndef CR5GOODSSTORAGES_H
#define CR5GOODSSTORAGES_H

#include "c5reportwidget.h"

class CR5GoodsStorages : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5GoodsStorages(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar();
};

#endif // CR5GOODSSTORAGES_H
