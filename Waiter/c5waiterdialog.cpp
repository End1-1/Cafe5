#include "c5waiterdialog.h"
#include "c5user.h"
#include <QShowEvent>
#include <QWindow>

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

    if(mScreen > -1) {
        QScreen *screen = qApp->screens().at(mScreen);

        if(screen) {
            windowHandle()->setScreen(screen);
        }
    }

    setWindowState(windowState() | Qt::WindowFullScreen);
}
