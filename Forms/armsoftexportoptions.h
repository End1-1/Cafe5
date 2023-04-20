#ifndef ARMSOFTEXPORTOPTIONS_H
#define ARMSOFTEXPORTOPTIONS_H

#include "c5dialog.h"

namespace Ui {
class ArmSoftExportOptions;
}

class ArmSoftExportOptions : public C5Dialog
{
    Q_OBJECT

public:
    explicit ArmSoftExportOptions(const QStringList &dbParams, QWidget *parent = nullptr);
    ~ArmSoftExportOptions();

private slots:
    void on_btnCancel_clicked();

    void on_btnOk_clicked();

private:
    Ui::ArmSoftExportOptions *ui;
};

#endif // ARMSOFTEXPORTOPTIONS_H
