#ifndef DLGSTOPLISTOPTION_H
#define DLGSTOPLISTOPTION_H

#include "c5dialog.h"

namespace Ui
{
class DlgStopListOption;
}

class DlgOrder;
class C5User;

class DlgStopListOption : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgStopListOption(DlgOrder *o, C5User *u);

    ~DlgStopListOption();

private slots:
    void removeStopListResponse(const QJsonObject &jdoc);

    void printStopListResponse(const QJsonObject &jdoc);

    void on_btnCancel_clicked();

    void on_btnClearStopList_clicked();

    void on_btnSetStoplist_clicked();

    void on_btnViewStopList_clicked();

    void on_btnPrintStoplist_clicked();

private:
    Ui::DlgStopListOption* ui;

    DlgOrder* fDlgOrder;
};

#endif // DLGSTOPLISTOPTION_H
