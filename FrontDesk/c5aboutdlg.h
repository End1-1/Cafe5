#ifndef C5ABOUTDLG_H
#define C5ABOUTDLG_H

#include <QDialog>

namespace Ui
{
class c5aboutdlg;
}

class c5aboutdlg : public QDialog
{
    Q_OBJECT

public:
    explicit c5aboutdlg(QWidget *parent = nullptr);
    ~c5aboutdlg();

private slots:
    void on_btnClose_clicked();

    void on_openWeb_clicked();

private:
    Ui::c5aboutdlg *ui;
};

#endif // C5ABOUTDLG_H
