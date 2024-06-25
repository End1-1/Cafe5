#ifndef DLGVIEWSTOPLIST_H
#define DLGVIEWSTOPLIST_H

#include "c5dialog.h"

namespace Ui
{
class DlgViewStopList;
}

class DlgViewStopList : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgViewStopList();
    ~DlgViewStopList();

private:
    Ui::DlgViewStopList *ui;

private slots:
    void getStopListResponse(const QJsonObject &jdoc);
    void handleStopListQty(const QJsonObject &obj);
    void handleStopListRemoveOne(const QJsonObject &obj);
    void on_btnBack_clicked();
    void on_btnChangeQty_clicked();
    void on_btnRemoveFromStoplist_clicked();
};

#endif // DLGVIEWSTOPLIST_H
