#ifndef C5DATASYNCHRONIZE_H
#define C5DATASYNCHRONIZE_H

#include "c5dialog.h"

namespace Ui {
class C5DataSynchronize;
}

class C5DataSynchronize : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5DataSynchronize(const QStringList &dbParams);

    ~C5DataSynchronize();

private slots:
    void on_btnSaveSyncTables_clicked();

    void on_btnRevert_clicked();

private:
    Ui::C5DataSynchronize *ui;

    void dropTriggers(C5Database &ds);

};

#endif // C5DATASYNCHRONIZE_H