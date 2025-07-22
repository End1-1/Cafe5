#ifndef CR5DISHPART2_H
#define CR5DISHPART2_H

#include "c5reportwidget.h"

class CR5DishPart2 : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5DishPart2(QWidget *parent = nullptr);

    virtual QToolBar *toolBar();

public slots:
    virtual bool on_tblView_doubleClicked(const QModelIndex &index);

protected:
    virtual void buildQuery();

    virtual void refreshData();

private:
    void setColors();

private slots:
    void deletePart2();

    void translator();
};

#endif // CR5DISHPART2_H
