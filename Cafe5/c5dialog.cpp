#include "c5dialog.h"
#include "c5database.h"
#include <QKeyEvent>

C5Dialog::C5Dialog(const QStringList &dbParams, QWidget *parent) :
#ifdef WAITER
    QDialog(parent, Qt::FramelessWindowHint),
    fDBParams(dbParams)
#else
    QDialog(parent),
    fDBParams(dbParams)
#endif
{
}

bool C5Dialog::preambule()
{
    return true;
}

C5SocketHandler *C5Dialog::createSocketHandler(const char *slot)
{
    C5SocketHandler *s = new C5SocketHandler(0, this);
    connect(s, SIGNAL(handleCommand(QJsonObject)), this, slot);
    connect(s, SIGNAL(handleError(int,QString)), this, SLOT(handleError(int,QString)));
    return s;
}

QStringList C5Dialog::getDbParams()
{
    return fDBParams;
}

void C5Dialog::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return:
        if (e->modifiers() & Qt::ControlModifier) {
            QDialog::keyPressEvent(e);
        } else {
            focusNextChild();
        }
        e->ignore();
        return;
    }
    QDialog::keyPressEvent(e);
}

void C5Dialog::handleError(int err, const QString &msg)
{
    Q_UNUSED(err);
    C5Message::error(msg);
}
