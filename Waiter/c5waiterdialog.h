#pragma once

#include "c5dialog.h"

class C5WaiterDialog : public C5Dialog
{
    Q_OBJECT

public:
    C5WaiterDialog(C5User *user);

protected:
    virtual void showEvent(QShowEvent *e) override;
};
