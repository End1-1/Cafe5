#ifndef RWORKSTATIONSREPORT_H
#define RWORKSTATIONSREPORT_H

#include "c5widget.h"
#include <QSet>

namespace Ui
{
class RWorkstationsReport;
}

class RAbstractEditorReport;
class CR5Hall;
class CR5Tables;
class CR5DiscountSystem;

class RWorkstationsReport : public C5Widget
{
    Q_OBJECT

public:
    RWorkstationsReport(const QString &title, QIcon icon, const QString &editorName);

    ~RWorkstationsReport() override;

    QToolBar *toolBar() override;

protected:
    void showEvent(QShowEvent *e) override;

private slots:
    void onTabChanged(int index);

private:
    void setupTabs();
    void propagateUserToReports();
    void ensureTabLoaded(int index);
    void syncToolbarFromActiveReport();

    C5Widget *activeChildWidget() const;
    RAbstractEditorReport *activeReport() const;

    Ui::RWorkstationsReport *ui = nullptr;
    QString mEditorName;
    RAbstractEditorReport *mWorkstationsListReport = nullptr;
    RAbstractEditorReport *mFiscalMachinesReport = nullptr;
    RAbstractEditorReport *mServiceValuesReport = nullptr;
    RAbstractEditorReport *mCashboxesReport = nullptr;
    RAbstractEditorReport *mPaymentTypesReport = nullptr;
    RAbstractEditorReport *mApiKeysReport = nullptr;
    RAbstractEditorReport *mPrintersReport = nullptr;
    RAbstractEditorReport *mPrintAliasesReport = nullptr;
    CR5Hall *mHallsReport = nullptr;
    CR5Tables *mTablesReport = nullptr;
    CR5DiscountSystem *mDiscountReport = nullptr;
    QSet<int> mLoadedTabIndexes;
};

#endif // RWORKSTATIONSREPORT_H
