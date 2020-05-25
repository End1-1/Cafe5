#ifndef CR5GOODS_H
#define CR5GOODS_H

#include "c5reportwidget.h"

class CR5Goods : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5Goods(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar();

public slots:
    virtual bool on_tblView_doubleClicked(const QModelIndex &index);

private slots:
    void pricing();
};

#endif // CR5GOODS_H
