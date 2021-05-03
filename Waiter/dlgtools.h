#ifndef DLGTOOLS_H
#define DLGTOOLS_H

#include "c5dialog.h"

namespace Ui {
class DlgTools;
}

class DlgTools : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgTools(const QStringList &dbParams);

    ~DlgTools();

private:
    Ui::DlgTools *ui;
};

#endif // DLGTOOLS_H
