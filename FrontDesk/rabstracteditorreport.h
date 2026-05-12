#pragma once

#include "c5widget.h"
#include <QJsonArray>
#include <QModelIndex>
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

    QJsonObject filterObject(const QString &name);

    QModelIndex reportMapViewIndexToSource(const QModelIndex &viewIndex) const;

    QVariant reportSourceCellData(int sourceRow, int column, int role = Qt::DisplayRole) const;

protected slots:
    virtual void on_tbl_doubleClicked(const QModelIndex &index);

    void on_leFilter_textChanged(const QString &arg1);

private:
    Ui::RAbstractEditorReport* ui;

    bool mFirstLoad = true;

    QString mEditorName;

    QJsonArray mFilterWidget;

    QJsonArray mFilterValues;

    RAbstractEditorTableModel* mModel;

    RFilterProxyModel* mProxyModel;

    void applyFilter();

    void removeAction();

    void showColumnVisibilityDialog();

    void applySavedColumnVisibility();

    void showColumnValueFilterDialog(int column);

    void exportToExcel();

    RAbstractEditorDialog* createEditorDialog(const QString &editorName);
};
