#ifndef RWORKSTATIONSREPORT_H
#define RWORKSTATIONSREPORT_H

#include "c5widget.h"
#include <QSet>

namespace Ui
{
class RWorkstationsReport;
}

class RAbstractEditorReport;

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

    RAbstractEditorReport *activeReport() const;

    Ui::RWorkstationsReport *ui = nullptr;
    QString mEditorName;
    RAbstractEditorReport *mWorkstationsListReport = nullptr;
    RAbstractEditorReport *mServiceValuesReport = nullptr;
    QSet<int> mLoadedTabIndexes;
};

#endif // RWORKSTATIONSREPORT_H
