#ifndef DLGPIN_H
#define DLGPIN_H

#include "c5dialog.h"
#include <QJsonObject>

namespace Ui
{
class DlgPin;
}

class NLoadingDlg;

class DlgPin : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgPin(QWidget *parent = nullptr);

    ~DlgPin();

    static bool getPin(QString &pin, QString &pass);

protected:
    virtual void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void loginResponse(const QJsonObject &jdoc);

    void queryLoading();

    void queryStopped(QObject *sender);

    void queryFinished(const QJsonObject &json);

    void queryError(const QString &err);

    void btnNumPressed();

    void on_btnEnter_clicked();

    void on_btnClear_clicked();

    void on_btnClose_clicked();

    void on_btn1_clicked();

    void on_btn2_clicked();

    void on_btn3_clicked();

    void on_btn4_clicked();

    void on_btn5_clicked();

    void on_btn6_clicked();

    void on_btn7_clicked();

    void on_btn8_clicked();

    void on_btn9_clicked();

    void on_btn0_clicked();

private:
    Ui::DlgPin *ui;

    bool fPinEmpty;

    NLoadingDlg *fLoadingDlg;

};

#endif // DLGPIN_H
