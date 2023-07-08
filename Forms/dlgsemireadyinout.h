#ifndef DLGSEMIREADYINOUT_H
#define DLGSEMIREADYINOUT_H

#include "c5dialog.h"

namespace Ui {
class DlgSemireadyInOut;
}

class DlgSemireadyInOut : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgSemireadyInOut(const QStringList &dbParams, QWidget *parent = nullptr);
    ~DlgSemireadyInOut();
    int dishId();
    double dishQty();
    int rowCount();
    int goodsId(int r);
    QString goodsName(int r);
    double goodsQty(int r);

private slots:
    void on_btnSelectGoods_clicked();

    void on_btnCancel_clicked();

    void on_leQty_textEdited(const QString &arg1);

    void on_btnOk_clicked();

private:
    Ui::DlgSemireadyInOut *ui;
};

#endif // DLGSEMIREADYINOUT_H
