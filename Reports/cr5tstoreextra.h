#ifndef CR5TSTOREEXTRA_H
#define CR5TSTOREEXTRA_H

#include "c5reportwidget.h"

class CR5TStoreExtra : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5TStoreExtra(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar();

protected:
    virtual void buildQuery();

    virtual void refreshData();

private:
    QMap<QString, int> fColumnNameIndex;

    QString documentForInventory();

private slots:
    void tblDoubleClicked(int row, int column, const QList<QVariant> &values);
};

#endif // CR5TSTOREEXTRA_H
