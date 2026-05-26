#pragma once

#include "c5widget.h"
#include <QJsonArray>
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

class RAbstractEditorReport : public C5Widget
{
    Q_OBJECT
public:
    explicit RAbstractEditorReport(const QString &title, QIcon icon, const QString &editorName);

    ~RAbstractEditorReport();

    virtual QToolBar* toolBar() override;

protected:
    virtual void showEvent(QShowEvent *e) override;

    virtual void newData();

    void getData();

    QJsonObject filterObject(const QString &name) const;

    QModelIndex reportMapViewIndexToSource(const QModelIndex &viewIndex) const;

    QVariant reportSourceCellData(int sourceRow, int column, int role = Qt::DisplayRole) const;

    virtual void applyFilter();

    Ui::RAbstractEditorReport *ui = nullptr;

protected slots:
    virtual void on_tbl_doubleClicked(const QModelIndex &index);

    void on_leFilter_textChanged(const QString &arg1);

private:

    bool mFirstLoad = true;

    QString mEditorName;

    QJsonArray mFilterWidget;

    QJsonArray mFilterValues;

    RAbstractEditorTableModel* mModel;

    RFilterProxyModel* mProxyModel;

    QSet<int> mReportDefaultHiddenColumns;

    void removeAction();

    void showColumnVisibilityDialog();

    void applyColumnVisibility();

    void showColumnValueFilterDialog(int column);

    void exportToExcel();

    RAbstractEditorDialog* createEditorDialog(const QString &editorName);
};
