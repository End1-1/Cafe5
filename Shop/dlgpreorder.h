#ifndef DLGPREORDER_H
#define DLGPREORDER_H

#include "c5dialog.h"

namespace Ui {
class DlgPreorder;
}

class Working;

class DlgPreorder : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgPreorder(QWidget *parent = nullptr);
    ~DlgPreorder();

private slots:

    void on_btnNew_clicked();

    void on_btnSave_clicked();

    void on_btnClose_clicked();

    void on_btnPrintFiscal_clicked();

    void on_btnEdit_clicked();

    void on_tbl_cellClicked(int row, int column);


    void on_btnAddGoods_clicked();

    void on_btnEditGoods_clicked();

    void on_btnCreateSale_clicked();

private:
    Ui::DlgPreorder *ui;
    Working *fWorking;
    void loadRows(const QString &id);
    void loadPreorder(const QString &id);
};

#endif // DLGPREORDER_H
