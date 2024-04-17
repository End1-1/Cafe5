#ifndef NTABLEWIDGET_H
#define NTABLEWIDGET_H

#include "c5widget.h"

namespace Ui {
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
    QString mRoute;
    NLoadingDlg *mLoadingDlg;
    NFilterDlg *fFilter;
    NHandler *mHandler;
    void sum();

private slots:
    void queryStarted();
    void queryError(const QString &error);
    void queryFinished(int elapsed, const QByteArray &ba);
    void exportToExcel();
    void refreshData();
    void clearFilter();
    void setSearchParameters();
    virtual void tableViewHeaderResized(int column, int oldSize, int newSize);
    void tblValueChanged(int v);
    void on_leFilterLineedit_textEdited(const QString &arg1);
    void on_mTableView_doubleClicked(const QModelIndex &index);
};

#endif // NTABLEWIDGET_H
