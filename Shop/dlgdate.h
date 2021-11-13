#ifndef DLGDATE_H
#define DLGDATE_H

#include "c5dialog.h"

namespace Ui {
class DlgDate;
}

class DlgDate : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgDate();

    ~DlgDate();

    static bool getDate(QDate &date);

private slots:
    void on_btnCancel_clicked();

    void on_btnOK_clicked();

private:
    Ui::DlgDate *ui;
};

#endif // DLGDATE_H
