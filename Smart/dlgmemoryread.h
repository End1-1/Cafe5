#ifndef DLGMEMORYREAD_H
#define DLGMEMORYREAD_H

#include "c5dialog.h"

namespace Ui {
class DlgMemoryRead;
}

class DlgMemoryRead : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgMemoryRead();
    ~DlgMemoryRead();
    static int getWindow();
    static int sessionHistory();

private slots:
    void on_btnCancel_clicked();

    void on_btnOpen_clicked();

    void on_tblMemory_cellClicked(int row, int column);

    void on_btnReprint_clicked();

    void on_btnOpen_2_clicked();

private:
    Ui::DlgMemoryRead *ui;
    int fResult;
    int getSessionHistory();
    int getIncompleteOrders();
};

#endif // DLGMEMORYREAD_H
