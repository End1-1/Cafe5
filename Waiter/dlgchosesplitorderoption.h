#ifndef DLGCHOSESPLITORDEROPTION_H
#define DLGCHOSESPLITORDEROPTION_H

#include "c5dialog.h"

#define JOIN_CANCEL 0
#define JOIN_JOIN 10
#define JOIN_APPEND 20

namespace Ui {
class DlgChoseSplitOrderOption;
}

class DlgChoseSplitOrderOption : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgChoseSplitOrderOption();

    ~DlgChoseSplitOrderOption();

    static int getOption();

private slots:
    void on_btnCancel_clicked();

    void on_btnSplit_clicked();

    void on_btnAppend_clicked();

private:
    Ui::DlgChoseSplitOrderOption *ui;
};

#endif // DLGCHOSESPLITORDEROPTION_H
