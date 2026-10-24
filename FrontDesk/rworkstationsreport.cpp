#include "rworkstationsreport.h"
#include "ui_rworkstationsreport.h"
#include "rfiscalmachinesreport.h"
#include "rservicevaluesreport.h"
#include "rcashboxesreport.h"
#include "rpaymenttypesreport.h"
#include "rapikeysreport.h"
#include "rprintersreport.h"
#include "rprintaliasesreport.h"
#include "cr5hall.h"
#include "cr5tables.h"
#include "cr5discountsystem.h"
#include "c5grid.h"
#include "dict_workstation.h"
#include "rabstracteditordialog.h"
#include "rabstracteditorreport.h"
#include "ui_rabstracteditorreport.h"
#include "c5message.h"
#include <QDialog>
#include <QShowEvent>
#include <QTabWidget>
#include <QTableView>
#include <QToolBar>

namespace {

class RWorkstationsListReport : public RAbstractEditorReport
{
public:
    explicit RWorkstationsListReport(const QString &title, QIcon icon, const QString &editorName, QWidget *parent = nullptr)
        : RAbstractEditorReport(title, icon, editorName, parent)
    {
        setDeferredLoad(true);
        connect(ui->tbl, &QTableView::doubleClicked, this, [this](const QModelIndex &index) {
            handleDoubleClick(index);
        });
    }

private:
    void handleDoubleClick(const QModelIndex &index)
    {
        if(!index.isValid()) {
            return;
        }

        const QModelIndex srcIndex = reportMapViewIndexToSource(index);
        if(!srcIndex.isValid()) {
            return;
        }

        const int type = reportSourceCellData(srcIndex.row(), 1).toInt();

        switch(type) {
        case WORKSTATION_WAITER:
        case WORKSTATION_SHOP:
        case WORKSTATION_COMMON: {
            RAbstractEditorDialog *dialog = createEditorDialog(mEditorName);
            const QJsonValue id = reportSourceCellData(srcIndex.row(), 0).toJsonValue();
            dialog->setId(id);
            dialog->exec();
            dialog->deleteLater();
            break;
        }
        default:
            C5Message::error(tr("Settings editor is not available for workstation type %1").arg(type));
            break;
        }
    }
};

} // namespace

RWorkstationsReport::RWorkstationsReport(const QString &title, QIcon icon, const QString &editorName)
    : C5Widget()
    , ui(new Ui::RWorkstationsReport)
    , mEditorName(editorName)
{
    ui->setupUi(this);
    fLabel = title;
    fIcon = icon;
    setupTabs();
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &RWorkstationsReport::onTabChanged);
}

RWorkstationsReport::~RWorkstationsReport()
{
    delete ui;
}

void RWorkstationsReport::setupTabs()
{
    mWorkstationsListReport = new RWorkstationsListReport(fLabel, fIcon, mEditorName, this);
    ui->tabWidget->addTab(mWorkstationsListReport, tr("Workstations"));

    mFiscalMachinesReport = new RFiscalMachinesReport(tr("Fiscal machines"), fIcon,
                                                      QStringLiteral("form_fiscal_machines"), this);
    ui->tabWidget->addTab(mFiscalMachinesReport, tr("Fiscal machines"));

    mServiceValuesReport = new RServiceValuesReport(tr("Service values"), fIcon, QStringLiteral("form_service_values"), this);
    ui->tabWidget->addTab(mServiceValuesReport, tr("Service values"));

    mCashboxesReport = new RCashboxesReport(tr("Cashboxes"), fIcon, QStringLiteral("form_cashboxes"), this);
    ui->tabWidget->addTab(mCashboxesReport, tr("Cashboxes"));

    mPaymentTypesReport = new RPaymentTypesReport(tr("Payment types"), fIcon, QStringLiteral("form_payment_types"), this);
    ui->tabWidget->addTab(mPaymentTypesReport, tr("Payment types"));

    mApiKeysReport = new RApiKeysReport(tr("API keys"), fIcon, QStringLiteral("form_api_keys"), this);
    ui->tabWidget->addTab(mApiKeysReport, tr("API keys"));

    mPrintersReport = new RPrintersReport(tr("Printers"), fIcon, QStringLiteral("form_printers"), this);
    ui->tabWidget->addTab(mPrintersReport, tr("Printers"));

    mPrintAliasesReport = new RPrintAliasesReport(tr("Print aliases"), fIcon, QStringLiteral("form_print_aliases"), this);
    ui->tabWidget->addTab(mPrintAliasesReport, tr("Print aliases"));

    mDiscountReport = new CR5DiscountSystem(this);
    ui->tabWidget->addTab(mDiscountReport, tr("Discount and bonuses"));

    mHallsReport = new CR5Hall(this);
    ui->tabWidget->addTab(mHallsReport, tr("Halls"));

    mTablesReport = new CR5Tables(this);
    ui->tabWidget->addTab(mTablesReport, tr("Tables"));

    propagateUserToReports();
}

void RWorkstationsReport::propagateUserToReports()
{
    for(int i = 0; i < ui->tabWidget->count(); ++i) {
        auto *child = qobject_cast<C5Widget *>(ui->tabWidget->widget(i));
        if(child) {
            child->mUser = mUser;
        }
    }
}

C5Widget *RWorkstationsReport::activeChildWidget() const
{
    return qobject_cast<C5Widget *>(ui->tabWidget->currentWidget());
}

RAbstractEditorReport *RWorkstationsReport::activeReport() const
{
    return qobject_cast<RAbstractEditorReport *>(ui->tabWidget->currentWidget());
}

void RWorkstationsReport::ensureTabLoaded(int index)
{
    if(index < 0 || mLoadedTabIndexes.contains(index)) {
        return;
    }

    propagateUserToReports();

    QWidget *w = ui->tabWidget->widget(index);
    if(auto *report = qobject_cast<RAbstractEditorReport *>(w)) {
        report->reloadReport();
    } else if(auto *grid = qobject_cast<C5Grid *>(w)) {
        grid->postProcess();
    } else {
        return;
    }

    mLoadedTabIndexes.insert(index);
}

void RWorkstationsReport::onTabChanged(int index)
{
    ensureTabLoaded(index);
    syncToolbarFromActiveReport();
}

void RWorkstationsReport::showEvent(QShowEvent *e)
{
    C5Widget::showEvent(e);
    propagateUserToReports();
    ensureTabLoaded(ui->tabWidget->currentIndex());
}

QToolBar *RWorkstationsReport::toolBar()
{
    if(!fToolBar) {
        fToolBar = new QToolBar();
        fToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    }

    syncToolbarFromActiveReport();
    return fToolBar;
}

void RWorkstationsReport::syncToolbarFromActiveReport()
{
    if(!fToolBar) {
        return;
    }

    fToolBar->clear();

    C5Widget *child = activeChildWidget();
    if(!child) {
        return;
    }

    QToolBar *innerToolbar = child->toolBar();
    if(!innerToolbar) {
        return;
    }

    for(QAction *src : innerToolbar->actions()) {
        QAction *action = fToolBar->addAction(src->icon(), src->text());
        action->setToolTip(src->toolTip());
        action->setEnabled(src->isEnabled());
        connect(action, &QAction::triggered, src, &QAction::trigger);
    }
}
