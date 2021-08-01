#ifndef DLGSTOPLISTOPTION_H
#define DLGSTOPLISTOPTION_H

#include "c5dialog.h"

namespace Ui {
class DlgStopListOption;
}

class DlgOrder;

class DlgStopListOption : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgStopListOption(DlgOrder *o);

    ~DlgStopListOption();

private slots:
    void handleStopList(const QJsonObject &o);

    void handlePrintStopList(const QJsonObject &obj);

    void on_btnCancel_clicked();

    void on_btnClearStopList_clicked();

    void on_btnSetStoplist_clicked();

    void on_btnViewStopList_clicked();

    void on_btnPrintStoplist_clicked();

private:
    Ui::DlgStopListOption *ui;

    DlgOrder *fDlgOrder;
};

#endif // DLGSTOPLISTOPTION_H
