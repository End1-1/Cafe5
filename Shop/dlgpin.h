#ifndef DLGPIN_H
#define DLGPIN_H

#include <QDialog>

namespace Ui
{
class DlgPin;
}

class NLoadingDlg;

class DlgPin : public QDialog
{
    Q_OBJECT

public:
    explicit DlgPin(QWidget *parent = nullptr);

    ~DlgPin();

    static bool getPin(QString &pin, QString &pass);

protected:
    virtual void keyReleaseEvent(QKeyEvent *event) override;

private slots:
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
