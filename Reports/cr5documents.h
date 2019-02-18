#ifndef CR5DOCUMENTS_H
#define CR5DOCUMENTS_H

#include "c5reportwidget.h"

class CR5Documents : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5Documents(const QStringList &dbParams, QWidget *parent = 0);

    virtual QToolBar *toolBar();

protected:
    virtual QMenu *buildTableViewContextMenu(const QPoint &point);

protected slots:
    virtual void tblDoubleClicked(int row, int column, const QList<QVariant> &values);

    virtual void callEditor(QString id);

private:
    void openDoc(QString id);

    int docType(QString id);

private slots:
    void saveDocs();

    void draftDocs();

    void removeDocs();

    void copySelectedDocs();
};

#endif // CR5DOCUMENTS_H
