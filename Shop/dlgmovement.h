#ifndef DLGMOVEMENT_H
#define DLGMOVEMENT_H

#include "c5dialog.h"

namespace Ui
{
class DlgMovement;
}

class DlgMovement : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgMovement();
    ~DlgMovement();
    bool openDoc(const QString &doc);

private slots:
    void setupResponse(const QJsonObject &jdoc);
    void searchResponse(const QJsonObject &jdoc);
    void on_leSearch_returnPressed();
    void on_btnSave_clicked();
    void on_btnExit_clicked();
    void on_leFilter_textChanged(const QString &arg1);
    void on_btnSaveAndAccept_clicked();
    void on_btnMinimize_clicked();

private:
    Ui::DlgMovement *ui;
    QString fUuid;
    QString fPendingDoc;
    bool mSetupComplete;
    int fState;
    int fStoreOut;
    int newRow();
    void countQty();
    bool saveDoc(int state);
    void countTotals();
};

#endif // DLGMOVEMENT_H
