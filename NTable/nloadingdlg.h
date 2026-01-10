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
    explicit NLoadingDlg(const QString &title, QWidget *parent = nullptr);

    ~NLoadingDlg();

    virtual void open() override;

    virtual void reject() override;

    virtual void accept() override;

    void resetSeconds();

public Q_SLOTS:
    void hide();

private:
    Ui::NLoadingDlg* ui;

    int mSecond;

private slots:
    void timeout();
};

#endif // NLOADINGDLG_H
