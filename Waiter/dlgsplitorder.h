#ifndef DLGSPLITORDER_H
#define DLGSPLITORDER_H

#include "c5dialog.h"
#include "c5user.h"

namespace Ui
{
class DlgSplitOrder;
}

class C5OrderDriver;

class DlgSplitOrder : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgSplitOrder(C5User *user);

    ~DlgSplitOrder();

    void configOrder(int table);

private slots:
    void openO1Response(const QJsonObject &jdoc);

    void openO2Response(const QJsonObject &jdoc);

    void on_btnChoseTable_clicked();

    void on_btnExit_clicked();

    void on_btnLeftToRight_clicked();

    void on_btnAllLeftToRigh_clicked();

    void on_btnRightToLeft_clicked();

    void on_btnAllRightToLeft_clicked();

    void on_btnSave_clicked();

    void on_btnScrollUp1_clicked();

    void on_btnScrollDown1_clicked();

    void on_btnScrollUp2_clicked();

    void on_btnScrollDown2_clicked();

private:
    Ui::DlgSplitOrder *ui;

    C5User *fUser;

    bool moveItem(C5OrderDriver *or1, C5OrderDriver *or2, int row);

    QMultiMap<QString, QString> fHistory;
};

#endif // DLGSPLITORDER_H
