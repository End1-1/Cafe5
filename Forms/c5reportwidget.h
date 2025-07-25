#ifndef C5REPORTWIDGET_H
#define C5REPORTWIDGET_H

#include "c5grid.h"
#include "c5cache.h"
#include "c5lineedit.h"
#include <QToolBar>

class C5Cache;
class CE5Editor;
class QTimer;

class C5ReportWidget : public C5Grid
{
    Q_OBJECT
public:
    C5ReportWidget(QWidget *parent = nullptr);

    ~C5ReportWidget();

    virtual bool hasDataUpdates();

    virtual void hotKey(const QString &key) override;

public slots:
    virtual bool on_tblView_doubleClicked(const QModelIndex &index) override;

protected:
    C5Cache* createCache(int cacheId);

    virtual void clearFilter() override;

    C5LineEdit* fFilterLineEdit;

    CE5Editor* fEditor;

protected slots:
    virtual void completeRefresh() override;

    virtual int newRow();

private:
    QTimer* fSearchTimer;

private slots:
    void searchText();
};

#endif // C5REPORTWIDGET_H
