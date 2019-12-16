#ifndef DLGSHIFTROTATION_H
#define DLGSHIFTROTATION_H

#include "c5dialog.h"

namespace Ui {
class DlgShiftRotation;
}

class DlgShiftRotation : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgShiftRotation(const QStringList &dbParams);

    ~DlgShiftRotation();

private slots:
    void handleShift(const QJsonObject &obj);

    void on_btnNextDate_clicked();

    void on_btnPrevDate_clicked();

    void on_btnChange_clicked();

    void on_btnCancel_clicked();

private:
    Ui::DlgShiftRotation *ui;
};

#endif // DLGSHIFTROTATION_H
