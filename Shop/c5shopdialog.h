#ifndef C5SHOPDIALOG_H
#define C5SHOPDIALOG_H

#include "c5dialog.h"
#include <QObject>

class C5User;

class C5ShopDialog : public C5Dialog
{
public:
    C5ShopDialog(C5User *user);

protected:
    C5User* mUser;
};

#endif // C5SHOPDIALOG_H
