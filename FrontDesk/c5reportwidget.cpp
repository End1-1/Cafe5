#include "c5reportwidget.h"
#include "c5cache.h"
#include "c5tablemodel.h"
#include <QIcon>
#include <QLayout>
#include <QLabel>

C5ReportWidget::C5ReportWidget(const QStringList &dbParams, QWidget *parent) :
    C5Grid(dbParams, parent)
{
    fIcon = ":/configure.png";
    fLabel = "C5ReportWidget";
    fToolBar = 0;
    QLabel *l = new QLabel(tr("Global search"), widget());
    fFilterLineEdit = new C5LineEdit(widget());
    fFilterLineEdit->setPlaceholderText(tr("Use \"|\" for separator multiple values"));
    widget()->layout()->addWidget(l);
    widget()->layout()->addWidget(fFilterLineEdit);
    widget()->setVisible(false);
    connect(fFilterLineEdit, &C5LineEdit::textChanged, [this](const QString &arg1) {
        fModel->setFilter(-1, arg1);
        sumColumnsData();
    });
    connect(this, SIGNAL(refreshed()), this, SLOT(completeRefresh()));
}

C5ReportWidget::~C5ReportWidget()
{
    if (fToolBar) {
        delete fToolBar;
    }
}

bool C5ReportWidget::hasDataUpdates()
{
    return static_cast<C5TableModel*>(fTableView->model())->hasUpdates();
}

void C5ReportWidget::hotKey(const QString &key)
{
    if (key.toLower() == "ctrl+f") {
        widget()->setVisible(true);
        fFilterLineEdit->setFocus();
        fFilterLineEdit->setSelection(0, fFilterLineEdit->text().length());
    } else if (key.toLower() == "esc") {
        widget()->setVisible(false);
    } else {
        C5Grid::hotKey(key);
    }
}

C5Cache *C5ReportWidget::createCache(int cacheId)
{
    return C5Cache::cache(fDb.dbParams(), cacheId);
}

void C5ReportWidget::clearFilter()
{
    fFilterLineEdit->clear();
    widget()->setVisible(false);
    C5Grid::clearFilter();
}

void C5ReportWidget::completeRefresh()
{
    fModel->setFilter(-1, fFilterLineEdit->text());
    sumColumnsData();
}
