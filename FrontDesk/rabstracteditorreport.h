#pragma once

#include "c5widget.h"

namespace Ui
{
class RAbstractEditorReport;
}

class RAbstractEditorTableModel;
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

private:
    Ui::RAbstractEditorReport* ui;

    bool mFirstLoad = true;

    QString mEditorName;

    RAbstractEditorTableModel* mModel;

    void getData();

    RAbstractEditorDialog* createEditorDialog(const QString &editorName);
};
