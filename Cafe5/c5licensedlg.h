#ifndef C5LICENSEDLG_H
#define C5LICENSEDLG_H

#include <QDialog>

namespace Ui {
class C5LicenseDlg;
}

class C5LicenseDlg : public QDialog
{
    Q_OBJECT

public:
    explicit C5LicenseDlg(QWidget *parent = nullptr);
    ~C5LicenseDlg();

private:
    Ui::C5LicenseDlg *ui;
};

#endif // C5LICENSEDLG_H
