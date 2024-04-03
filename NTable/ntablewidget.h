#ifndef NTABLEWIDGET_H
#define NTABLEWIDGET_H

#include "c5widget.h"

namespace Ui {
class NTableWidget;
}

class C5MainWindow;
class NLoadingDlg;

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

private:
    Ui::NTableWidget *ui;
    QString mRoute;
    NLoadingDlg *mLoadingDlg;

private slots:
    void queryStarted();
    void queryError(const QString &error);
    void queryFinished(int elapsed, const QByteArray &ba);
    void exportToExcel();
    void refreshData();
    void clearFilter();

};

#endif // NTABLEWIDGET_H
