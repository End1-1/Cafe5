#ifndef NLOADINGDLG_H
#define NLOADINGDLG_H

#include <QDialog>

namespace Ui
{
class NLoadingDlg;
}

class NLoadingDlg : public QDialog
{
    Q_OBJECT

public:
    explicit NLoadingDlg(QWidget *parent = nullptr);
    ~NLoadingDlg();
    int mSecond;
    virtual void open() override;
    virtual void reject() override;
    virtual void accept() override;

private:
    Ui::NLoadingDlg *ui;

private slots:
    void timeout();
};

#endif // NLOADINGDLG_H
