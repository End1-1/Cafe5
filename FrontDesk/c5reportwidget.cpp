#include "c5reportwidget.h"
#include "c5cache.h"
#include "c5tablemodel.h"
#include <QIcon>

C5ReportWidget::C5ReportWidget(QWidget *parent) :
    C5Grid(parent)
{
    fIcon = ":/configure.png";
    fLabel = "C5ReportWidget";
    fToolBar = 0;
}

C5ReportWidget::~C5ReportWidget()
{
    if (fToolBar) {
        delete fToolBar;
    }
}

QIcon C5ReportWidget::icon()
{
    return QIcon(fIcon);
}

QString C5ReportWidget::label()
{
    return fLabel;
}

QToolBar *C5ReportWidget::toolBar()
{
    createToolBar();
    return fToolBar;
}

bool C5ReportWidget::hasDataUpdates()
{
    return static_cast<C5TableModel*>(fTableView->model())->hasUpdates();
}

bool C5ReportWidget::createToolBar()
{
    if (!fToolBar) {
        fToolBar = new QToolBar();
        fToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        return true;
    }
    return false;
}

C5Cache *C5ReportWidget::createCache(int cacheId)
{
    return C5Cache::cache(fDb.host(), fDb.database(), fDb.username(), fDb.password(), cacheId);
}
