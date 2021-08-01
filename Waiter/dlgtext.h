#ifndef DLGTEXT_H
#define DLGTEXT_H

#include "c5dialog.h"

namespace Ui {
class DlgText;
}

class DlgText : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgText();

    ~DlgText();

    static bool getText(const QString &title, QString &defaultText);

private:
    Ui::DlgText *ui;
};

#endif // DLGTEXT_H
