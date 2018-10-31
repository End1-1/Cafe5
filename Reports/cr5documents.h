#ifndef CR5DOCUMENTS_H
#define CR5DOCUMENTS_H

#include "c5reportwidget.h"

class CR5Documents : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5Documents(const QStringList &dbParams, QWidget *parent = 0);

    virtual QToolBar *toolBar();

protected slots:
    virtual void tblDoubleClicked(int row, int column, const QList<QVariant> &values);

    virtual void callEditor(int id);

    virtual void removeWithId(int id);

private:
    void openDoc(int id);

    int docType(int id);
};

#endif // CR5DOCUMENTS_H
