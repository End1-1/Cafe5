#ifndef DLGRECEIPTLANGUAGE_H
#define DLGRECEIPTLANGUAGE_H

#include "c5dialog.h"

namespace Ui {
class DlgReceiptLanguage;
}

class DlgReceiptLanguage : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgReceiptLanguage();

    ~DlgReceiptLanguage();

    static int receipLanguage();

private slots:
    void on_lst_clicked(const QModelIndex &index);

private:
    Ui::DlgReceiptLanguage *ui;

    int fResult;
};

#endif // DLGRECEIPTLANGUAGE_H
