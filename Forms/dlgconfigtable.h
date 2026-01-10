#ifndef DLGCONFIGTABLE_H
#define DLGCONFIGTABLE_H

#include "c5dialog.h"

namespace Ui {
class DlgConfigTable;
}

class DlgConfigTable : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgConfigTable(C5User *user);
    ~DlgConfigTable();

private:
    Ui::DlgConfigTable *ui;
};

#endif // DLGCONFIGTABLE_H
