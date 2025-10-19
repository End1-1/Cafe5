#ifndef DLGSEARCHPARTNER_H
#define DLGSEARCHPARTNER_H

#include "c5dialog.h"

namespace Ui
{
class DlgSearchPartner;
}

class DlgSearchPartner : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgSearchPartner();
    ~DlgSearchPartner();
    int result;
    QMap<int, QVariant> fData;

private slots:
    void on_btnAddPartner_clicked();

    void on_btnCancel_clicked();

    void on_tbl_cellDoubleClicked(int row, int column);

    void on_btnSave_clicked();

    void on_lineEdit_textChanged(const QString &arg1);

private:
    Ui::DlgSearchPartner* ui;

    void loadPartners();
};

#endif // DLGSEARCHPARTNER_H
