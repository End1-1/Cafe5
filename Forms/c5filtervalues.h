#ifndef C5FILTERVALUES_H
#define C5FILTERVALUES_H

#include "c5dialog.h"

namespace Ui {
class C5FilterValues;
}

class C5FilterValues : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5FilterValues(C5User *user);

    ~C5FilterValues();

    static bool filterValues(QStringList &values, C5User *user);

private slots:
    void on_leFilter_textChanged(const QString &arg1);

    void on_buttonBox_rejected();

    void on_buttonBox_accepted();

    void on_lst_itemSelectionChanged();

    void on_chAll_clicked(bool checked);

    void on_chSelected_clicked(bool checked);

private:
    Ui::C5FilterValues *ui;
};

#endif // C5FILTERVALUES_H
