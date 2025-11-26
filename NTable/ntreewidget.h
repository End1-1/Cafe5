#pragma once

#include "c5widget.h"

namespace Ui
{
class NTreeWidget;
}

class C5MainWindow;
class NLoadingDlg;
class NFilterDlg;
class NHandler;

class NTreeWidget : public C5Widget
{
    Q_OBJECT

public:
    explicit NTreeWidget(const QString &route, QWidget *parent = nullptr);
    ~NTreeWidget();
    C5MainWindow* mMainWindow;
    static QString mHost;
    void query();
    virtual QToolBar* toolBar() override;
    virtual void hotKey(const QString &key) override;
    void initParams(const QJsonObject &o);

private:
    Ui::NTreeWidget* ui;
    int fFilterColumn;
    QJsonObject fInitParams;
    QString mRoute;
    NLoadingDlg* mLoadingDlg;
    NFilterDlg* fFilter;
    NHandler* mHandler;
    QWidget* mToolWidget;
    void sum();

private slots:
    void queryStarted();
    void filterByColumn();
    void queryError(const QString &error);
    void queryFinished(const QJsonObject &ba);
    void exportToExcel();
    void refreshData();
    void clearFilter();
    void setSearchParameters();
    virtual void tableViewHeaderResized(int column, int oldSize, int newSize);
    void tblValueChanged(int v);
    void tblValueChanged2(int v);
    void on_leFilterLineedit_textEdited(const QString &arg1);
    virtual void tableViewHeaderContextMenuRequested(const QPoint &point);
    virtual void tableViewHeaderClicked(int index);
    void on_mTreeView_doubleClicked(const QModelIndex &index);
};
