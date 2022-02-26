#ifndef C5REPORTWIDGET_H
#define C5REPORTWIDGET_H

#include "c5grid.h"
#include "c5cache.h"
#include "c5lineedit.h"
#include <QToolBar>

class C5Cache;
class CE5Editor;

class C5ReportWidget : public C5Grid
{
    Q_OBJECT
public:
    C5ReportWidget(const QStringList &dbParams, QWidget *parent = nullptr);

    ~C5ReportWidget();

    virtual bool hasDataUpdates();

    virtual void hotKey(const QString &key);

    virtual void changeDatabase(const QStringList &dbParams) override;

public slots:
    virtual bool on_tblView_doubleClicked(const QModelIndex &index);

protected:
    C5Cache *createCache(int cacheId);

    virtual void clearFilter();

    C5LineEdit *fFilterLineEdit;

    CE5Editor *fEditor;

protected slots:
    void completeRefresh();

    virtual int newRow();
};

#endif // C5REPORTWIDGET_H
