#include "c5waiterdialog.h"
#include <QDebug>
#include <QShowEvent>
#include <QStringList>
#include <QWindow>
#include "c5user.h"

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
