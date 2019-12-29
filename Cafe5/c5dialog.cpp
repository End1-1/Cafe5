#include "c5dialog.h"
#include "c5database.h"
#include <QKeyEvent>

static QWidget *__mainWindow = nullptr;

C5Dialog::C5Dialog(const QStringList &dbParams) :
#ifdef WAITER
    QDialog(__mainWindow, C5Config::isAppFullScreen() ? Qt::FramelessWindowHint : Qt::WindowFlags()),
    fDBParams(dbParams)
#else
    QDialog(__mainWindow),
    fDBParams(dbParams)
#endif
{
    if (__mainWindow == nullptr) {
        __mainWindow = this;
    }
}

C5Dialog::~C5Dialog()
{
    if (__mainWindow == this) {
        __mainWindow = nullptr;
    }
}

void C5Dialog::setMainWindow(QWidget *widget)
{
    __mainWindow = widget;
}

bool C5Dialog::preambule()
{
    return true;
}

C5SocketHandler *C5Dialog::createSocketHandler(const char *slot)
{
    C5SocketHandler *s = new C5SocketHandler(nullptr, this);
    connect(s, SIGNAL(handleCommand(QJsonObject)), this, slot);
    connect(s, SIGNAL(handleError(int,QString)), this, SLOT(handleError(int,QString)));
    return s;
}

QStringList C5Dialog::getDbParams()
{
    return fDBParams;
}

void C5Dialog::showFullScreen()
{
#ifdef WAITER
    if (C5Config::isAppFullScreen()) {
        QDialog::showFullScreen();
    } else {
        QDialog::showMaximized();
    }
#else
    QDialog::showFullScreen();
#endif
}

void C5Dialog::selectorCallback(int row, const QList<QVariant> &values)
{
    Q_UNUSED(row);
    Q_UNUSED(values);
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
