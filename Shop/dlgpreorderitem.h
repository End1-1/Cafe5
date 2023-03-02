#ifndef DLGPREORDERITEM_H
#define DLGPREORDERITEM_H

#include "c5dialog.h"

namespace Ui {
class DlgPreorderItem;
}

class QTableWidget;

class DlgPreorderItem : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgPreorderItem(QWidget *parent = nullptr);
    ~DlgPreorderItem();
    void configToTable(QTableWidget *t, int r);
    void configFromTable(QTableWidget *t, int r);

private slots:
    void on_btnGetGoods_clicked();

    void on_leQty_textEdited(const QString &arg1);

    void on_lePrice_textEdited(const QString &arg1);

    void on_leTotal_textEdited(const QString &arg1);

    void on_btnCancel_clicked();

    void on_btnSave_clicked();

private:
    Ui::DlgPreorderItem *ui;
};

#endif // DLGPREORDERITEM_H
