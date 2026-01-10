#pragma once

#include "c5dialog.h"

namespace Ui
{
class DlgReportsFilter;
}

class DlgReportsFilter : public C5Dialog
{
    Q_OBJECT
public:
    explicit DlgReportsFilter(const QDate &d1, const QDate &d2, C5User *user = nullptr);

    ~DlgReportsFilter();

    QDate date1();

    QDate date2();
private slots:
    void on_btnOK_clicked();

    void on_btnCancel_clicked();

private:
    Ui::DlgReportsFilter* ui;
};
