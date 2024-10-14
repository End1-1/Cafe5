#ifndef NTABLEWIDGET_H
#define NTABLEWIDGET_H

#include "c5widget.h"

namespace Ui
{
class NTableWidget;
}

class C5MainWindow;
class NLoadingDlg;
class NFilterDlg;
class NHandler;

class NTableWidget : public C5Widget
{
    Q_OBJECT

public:
    explicit NTableWidget(const QString &route, QWidget *parent = nullptr);
    ~NTableWidget();
    C5MainWindow *mMainWindow;
    static QString mHost;
    void query();
    virtual QToolBar *toolBar() override;
    virtual void hotKey(const QString &key) override;

private:
    Ui::NTableWidget *ui;
    int fFilterColumn;
    QString mRoute;
    NLoadingDlg *mLoadingDlg;
    NFilterDlg *fFilter;
    NHandler *mHandler;
    QWidget *mToolWidget;
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
    void on_mTableView_doubleClicked(const QModelIndex &index);
    virtual void tableViewHeaderContextMenuRequested(const QPoint &point);
    virtual void tableViewHeaderClicked(int index);
};

#endif // NTABLEWIDGET_H
