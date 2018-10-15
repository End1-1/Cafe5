#include "c5reportwidget.h"
#include "c5cache.h"
#include "c5tablemodel.h"
#include <QIcon>
#include <QLayout>
#include <QLabel>

C5ReportWidget::C5ReportWidget(QWidget *parent) :
    C5Grid(parent)
{
    fIcon = ":/configure.png";
    fLabel = "C5ReportWidget";
    fToolBar = 0;
    QLabel *l = new QLabel(tr("Global search"), widget());
    fFilterLineEdit = new C5LineEdit(widget());
    widget()->layout()->addWidget(l);
    widget()->layout()->addWidget(fFilterLineEdit);
    widget()->setVisible(false);
    connect(fFilterLineEdit, &C5LineEdit::textChanged, [this](const QString &arg1) {
        fModel->setFilter(-1, arg1);
    });
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

void C5ReportWidget::hotkey(const QString &key)
{
    if (key == "F3") {
        widget()->setVisible(true);
        fFilterLineEdit->setFocus();
        fFilterLineEdit->setSelection(0, fFilterLineEdit->text().length());
    } else if (key == "esc") {
        widget()->setVisible(false);
    }
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

QToolBar *C5ReportWidget::createStandartToolbar(const QList<C5Grid::ToolBarButtons> &btn)
{
    if (createToolBar()) {
        if (btn.contains(ToolBarButtons::tbNew)) {
            fToolBar->addAction(QIcon(":/new.png"), tr("New"), this, SLOT(newRow()));
        }
        if (btn.contains(ToolBarButtons::tbEdit)) {
            fToolBar->addAction(QIcon(":/edit.png"), tr("Edit"), this, SLOT(editRow()));
        }
        if (btn.contains(ToolBarButtons::tbSave)) {
            fToolBar->addAction(QIcon(":/save.png"), tr("Save\nchanges"), this, SLOT(saveDataChanges()));
        }
        if (btn.contains(ToolBarButtons::tbFilter)) {
            fToolBar->addAction(QIcon(":/filter_set.png"), tr("Search by\nparameters"), this, SLOT(setSearchParameters()));
        }
        if (btn.contains(ToolBarButtons::tbClearFilter)) {
            fToolBar->addAction(QIcon(":/filter_clear.png"), tr("Clear\nfilter"), this, SLOT(clearFilter()));
        }
        if (btn.contains(ToolBarButtons::tbRefresh)) {
            fToolBar->addAction(QIcon(":/refresh.png"), tr("Refresh"), this, SLOT(refreshData()));
        }
        if (btn.contains(ToolBarButtons::tbPrint)) {
            fToolBar->addAction(QIcon(":/print.png"), tr("Print"), this, SLOT(print()));
        }
        if (btn.contains(ToolBarButtons::tbExcel)) {
            fToolBar->addAction(QIcon(":/excel.png"), tr("Export to\nMS Excel"), this, SLOT(exportToExcel()));
        }
    }
    return fToolBar;
}

void C5ReportWidget::clearFilter()
{
    fFilterLineEdit->clear();
    widget()->setVisible(false);
    C5Grid::clearFilter();
}
