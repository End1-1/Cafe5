#ifndef NLOADINGDLG_H
#define NLOADINGDLG_H

#include <QDialog>

namespace Ui {
class NLoadingDlg;
}

class NLoadingDlg : public QDialog
{
    Q_OBJECT

public:
    explicit NLoadingDlg(QWidget *parent = nullptr);
    ~NLoadingDlg();

private:
    Ui::NLoadingDlg *ui;
    int mSecond;

private slots:
    void timeout();
};

#endif // NLOADINGDLG_H
