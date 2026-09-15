#pragma once

#include "c5widget.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QModelIndex>
#include <QSet>
#include <QVariant>

namespace Ui
{
class RAbstractEditorReport;
}

class RAbstractEditorTableModel;
class RFilterProxyModel;
class RAbstractEditorDialog;
class QButtonGroup;
class QToolButton;

class RAbstractEditorReport : public C5Widget
{
    Q_OBJECT
public:
    explicit RAbstractEditorReport(const QString &title, QIcon icon, const QString &editorName, QWidget *parent = nullptr);

    ~RAbstractEditorReport();

    virtual QToolBar* toolBar() override;

    /** Load or refresh report data from the server. */
    void reloadReport();

    /** When true, data is not fetched in showEvent (use reloadReport / tab activation). */
    void setDeferredLoad(bool deferred);

    QString mEditorName;

protected:
    virtual void showEvent(QShowEvent *e) override;

    virtual void resizeEvent(QResizeEvent *e) override;

    virtual bool eventFilter(QObject *watched, QEvent *event) override;

    virtual void newData();

    virtual void removeAction();

    QJsonObject filterObject(const QString &name) const;

    QJsonArray reportFilterValues() const { return mFilterValues; }

    QModelIndex reportMapViewIndexToSource(const QModelIndex &viewIndex) const;

    QVariant reportSourceCellData(int sourceRow, int column, int role = Qt::DisplayRole) const;

    int reportColumnFromEnd(int offsetFromEnd) const;

    virtual void applyFilter();

    RAbstractEditorDialog* createEditorDialog(const QString &editorName);

    void getData();

    Ui::RAbstractEditorReport *ui = nullptr;

protected slots:
    virtual void on_tbl_doubleClicked(const QModelIndex &index);

    void on_leFilter_textChanged(const QString &arg1);

private:

    bool mFirstLoad = true;

    bool mDeferredLoad = false;

    QJsonArray mFilterWidget;

    QJsonArray mFilterValues;

    RAbstractEditorTableModel* mModel;

    RFilterProxyModel* mProxyModel;

    QSet<int> mReportDefaultHiddenColumns;

    QJsonObject mFooterValues;

    QButtonGroup *mViewModeGroup = nullptr;

    QString mViewModeFilterName;

    bool mUpdatingViewModeScroll = false;

    void showColumnVisibilityDialog();

    void applyColumnVisibility();

    void showColumnValueFilterDialog(int column);

    void exportToExcel();

    void printStockInventoryBlank();

    void setupViewModeBar();

    void clearViewModeButtons();

    void setViewModeFilterValue(int value);

    void updateViewModeScrollButtons();

    void syncViewModeScrollRange();

    void scrollViewModeBy(int delta);
};
