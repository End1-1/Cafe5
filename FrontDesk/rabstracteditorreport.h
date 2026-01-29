#pragma once

#include "c5widget.h"
#include <QJsonArray>

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

private slots:
    void on_tbl_doubleClicked(const QModelIndex &index);

    void on_leFilter_textChanged(const QString &arg1);

private:
    Ui::RAbstractEditorReport* ui;

    bool mFirstLoad = true;

    QString mEditorName;

    QJsonArray mFilterWidget;

    QJsonArray mFilterValues;

    RAbstractEditorTableModel* mModel;

    RFilterProxyModel* mProxyModel;

    void getData();

    void applyFilter();

    RAbstractEditorDialog* createEditorDialog(const QString &editorName);
};
