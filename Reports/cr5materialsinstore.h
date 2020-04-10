#ifndef CR5MATERIALSINSTORE_H
#define CR5MATERIALSINSTORE_H

#include "c5reportwidget.h"

class CR5MaterialInStoreFilter;

class CR5MaterialsInStore : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5MaterialsInStore(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar();


    virtual void buildQuery() override;

protected slots:
    virtual void refreshData() override;

private:
    CR5MaterialInStoreFilter *fFilter;

    void prepareDrafts();

    void prepareNoDrafts();

    void setColors();

private slots:
    void printBarcode();

};

#endif // CR5MATERIALSINSTORE_H
