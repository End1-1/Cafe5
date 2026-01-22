#pragma once

#include "c5dialog.h"
#include <QJsonObject>

namespace Ui
{
class DlgReportParams;
}

class WPWidget;

class DlgReportParams : public C5Dialog
{
    Q_OBJECT
public:
    explicit DlgReportParams(C5User *user, const QString &params);

    ~DlgReportParams();

    QJsonObject getParams();

protected:
    virtual void showEvent(QShowEvent *e) override;

private slots:
    void on_btnBack_clicked();

    void on_btnPrint_clicked();

private:
    Ui::DlgReportParams* ui;

    WPWidget* mWPWidget = nullptr;
};
