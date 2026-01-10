#pragma once

#include "c5dialog.h"

namespace Ui
{
class DlgSimleOptions;
}

class DlgSimleOptions : public C5Dialog
{
    Q_OBJECT
public:
    explicit DlgSimleOptions(const QStringList &title, const QList<int>& values);

    ~DlgSimleOptions();

    virtual int exec() override;

protected:
    virtual void showEvent(QShowEvent *e) override;

private:
    Ui::DlgSimleOptions* ui;

    int mResult = 0;
};
