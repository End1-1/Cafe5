#ifndef C5REPORTWIDGET_H
#define C5REPORTWIDGET_H

#include "c5grid.h"
#include "c5lineedit.h"
#include <QToolBar>

class C5Cache;

class C5ReportWidget : public C5Grid
{
public:
    C5ReportWidget(QWidget *parent = 0);

    ~C5ReportWidget();

    QIcon icon();

    QString label();

    virtual QToolBar *toolBar();

    virtual bool hasDataUpdates();

    void hotkey(const QString &key);

protected:
    QString fIcon;

    QString fLabel;

    QToolBar *fToolBar;

    bool createToolBar();

    C5Cache *createCache(int cacheId);

    QToolBar *createStandartToolbar(const QList<ToolBarButtons> &btn);

    virtual void clearFilter();

    C5LineEdit *fFilterLineEdit;
};

#endif // C5REPORTWIDGET_H
