#pragma once

#include "c5dialog.h"

namespace Ui
{
class DlgSimleOptions;
}

class C5User;

class DlgSimleOptions : public C5Dialog
{
    Q_OBJECT
public:
    explicit DlgSimleOptions(const QStringList &title, const QList<int>& values);

    /** Waiter: pass session user and parent so the dialog stacks correctly over fullscreen. */
    explicit DlgSimleOptions(const QStringList &title, const QList<int> &values, C5User *user, QWidget *parent = nullptr);

    /** Button grid columns count (1..3 recommended for touchscreen). */
    explicit DlgSimleOptions(const QStringList &title, const QList<int> &values, int columns, C5User *user, QWidget *parent = nullptr);

    ~DlgSimleOptions();

    virtual int exec() override;

protected:
    virtual void showEvent(QShowEvent *e) override;

private:
    Ui::DlgSimleOptions* ui;

    int mResult = 0;
};
