#ifndef DLGIMPORTFROMAS_H
#define DLGIMPORTFROMAS_H

#include "c5dialog.h"

namespace Ui
{
class DlgImportFromAS;
}

class DlgImportFromAS : public C5Dialog
{
    Q_OBJECT
public:
    explicit DlgImportFromAS(QWidget *parent = nullptr);
    ~DlgImportFromAS();

private slots:
    void on_btnEnableEdit_clicked(bool checked);

    void on_leAddressString_textChanged(const QString &arg1);

    void on_btnCheckConnection_clicked();

private:
    Ui::DlgImportFromAS* ui;
};

#endif // DLGIMPORTFROMAS_H
