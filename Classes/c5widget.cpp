#include "c5widget.h"

C5Widget::C5Widget(const QStringList &dbParams, QWidget *parent) :
    QWidget(parent),
    fDBParams(dbParams)
{
    fToolBar = 0;
}

QIcon C5Widget::icon()
{
    return QIcon(fIcon);
}

QString C5Widget::label()
{
    return fLabel;
}

void C5Widget::postProcess()
{

}

QToolBar *C5Widget::toolBar()
{
    createToolBar();
    return fToolBar;

    if (!fToolBar) {
        fToolBar = new QToolBar();
        fToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    }
    return fToolBar;
}

bool C5Widget::createToolBar()
{
    if (!fToolBar) {
        fToolBar = new QToolBar();
        fToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        return true;
    }
    return false;
}

QToolBar *C5Widget::createStandartToolbar(const QList<ToolBarButtons> &btn)
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

void C5Widget::print()
{

}

QWidget *C5Widget::widget()
{
    return 0;
}

void C5Widget::hotKey(const QString &key)
{
    Q_UNUSED(key);
}
