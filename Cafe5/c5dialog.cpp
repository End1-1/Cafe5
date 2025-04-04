#include "c5dialog.h"
#include "c5message.h"
#include <QKeyEvent>
#include <QApplication>
#include <QScreen>

static QWidget *__mainWindow = nullptr;

C5Dialog::C5Dialog(const QStringList &dbParams) :
#ifdef WAITER
    QDialog(__mainWindow, Qt::FramelessWindowHint),
    fDBParams(dbParams)
#else
    QDialog(__mainWindow),
    fDBParams(dbParams)
#endif
{
#ifdef WAITER
    QScreen *screen = QGuiApplication::primaryScreen();
    setMaximumSize(screen->geometry().size());
    //setWindowFlags(Qt::WindowStaysOnTopHint);
#endif
    if (__mainWindow == nullptr) {
        __mainWindow = this;
    }
    fHttp = new NInterface(this);
}

C5Dialog::C5Dialog(const QStringList &dbParams, bool noparent) :
#ifdef WAITER
    QDialog(noparent ? nullptr : __mainWindow, Qt::FramelessWindowHint),
    fDBParams(dbParams)
#else
    QDialog(__mainWindow),
    fDBParams(dbParams)
#endif
{
#ifdef WAITER
    QScreen *screen = QGuiApplication::primaryScreen();
    setMaximumSize(screen->geometry().size());
#endif
    if (!noparent && __mainWindow == nullptr) {
        __mainWindow = this;
    }
    fHttp = new NInterface(this);
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
    connect(s, SIGNAL(handleError(int, QString)), this, SLOT(handleError(int, QString)));
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

void C5Dialog::selectorCallback(int row, const QJsonArray &values)
{
    Q_UNUSED(row);
    Q_UNUSED(values);
}

void C5Dialog::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
            if (e->modifiers() &Qt::ControlModifier) {
                keyControlPlusEnter();
            }
            if (e->modifiers() &Qt::AltModifier) {
                keyAlterPlusEnter();
            } else {
                focusNextChild();
                keyEnter();
            }
            e->ignore();
            return;
    }
    QDialog::keyPressEvent(e);
}

void C5Dialog::keyEnter()
{
}

void C5Dialog::keyControlPlusEnter()
{
}

void C5Dialog::keyAlterPlusEnter()
{
}

void C5Dialog::handleError(int err, const QString &msg)
{
    Q_UNUSED(err);
    C5Message::error(msg);
}
