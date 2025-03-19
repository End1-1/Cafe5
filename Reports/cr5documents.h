#ifndef CR5DOCUMENTS_H
#define CR5DOCUMENTS_H

#include "c5reportwidget.h"

class CR5DocumentsFilter;

class CR5Documents : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5Documents(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar() override;

protected:
    virtual QMenu *buildTableViewContextMenu(const QPoint &point) override;

    virtual void restoreColumnsWidths() override;

    virtual bool tblDoubleClicked(int row, int column, const QJsonArray &values) override;

protected slots:
    virtual void callEditor(const QString &id) override;

private:
    CR5DocumentsFilter *fFilter;

    void openDoc(QString id);

    int docType(QString id);

private slots:
    void saveDocs();

    void draftDocs();

    void removeDocs();

    void copySelectedDocs();

    void templates();

signals:
    void updateProgressValue(int);

    void updateProgressValueWithMessage(int, QString);
};

#endif // CR5DOCUMENTS_H
