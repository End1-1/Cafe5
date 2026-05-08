#ifndef DLGPREORDERDATETIME_H
#define DLGPREORDERDATETIME_H

#include "c5dialog.h"

namespace Ui
{
class DlgPreorderDateTime;
}

class DlgPreorderDateTime : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgPreorderDateTime(C5User *user, QWidget *parent = nullptr);

    ~DlgPreorderDateTime() override;

    void setInitial(const QDateTime &dt);

    bool clearRequested() const
    {
        return mClearRequested;
    }

    QDateTime selectedDateTime() const;

private slots:
    void on_btnOk_clicked();

    void on_btnClear_clicked();

    void on_btnCancel_clicked();

private:
    Ui::DlgPreorderDateTime *ui;

    bool mClearRequested = false;
};

#endif // DLGPREORDERDATETIME_H
