#ifndef C5WAITERDIALOG_H
#define C5WAITERDIALOG_H

#include "c5dialog.h"

class C5User;

class C5WaiterDialog : public C5Dialog
{
public:
    C5WaiterDialog(C5User *user);

protected:
    C5User* mUser;
};

#endif // C5WAITERDIALOG_H
