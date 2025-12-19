#include "c5shopdialog.h"
#include "c5user.h"

C5ShopDialog::C5ShopDialog(C5User *user) :
    C5Dialog(),
    mUser(user)
{
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
}

C5ShopDialog::~C5ShopDialog()
{
}
