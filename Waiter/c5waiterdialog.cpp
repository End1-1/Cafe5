#include "c5waiterdialog.h"
#include "c5user.h"
#include <QShowEvent>

C5WaiterDialog::C5WaiterDialog(C5User *user)
    : C5Dialog(user)
{
}

void C5WaiterDialog::showEvent(QShowEvent *e)
{
    C5Dialog::showEvent(e);

    if(e->spontaneous()) {
        return;
    }

    setWindowState(windowState() | Qt::WindowFullScreen);
}
