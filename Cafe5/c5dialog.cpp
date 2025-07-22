#include "c5dialog.h"
#include "c5message.h"
#include "c5lineedit.h"
#include "c5config.h"
#include <QKeyEvent>
#include <QApplication>
#include <QScreen>

static QWidget* __mainWindow = nullptr;

C5Dialog::C5Dialog() :
    QDialog(__mainWindow, C5Config::isAppFullScreen() ? Qt::FramelessWindowHint : Qt::Window)
{
#ifdef WAITER
#ifndef QT_DEBUG
    QScreen *screen = QGuiApplication::primaryScreen();
    setMaximumSize(screen->geometry().size());
    //setWindowFlags(Qt::WindowStaysOnTopHint);
#endif
#endif

    if(__mainWindow == nullptr) {
        __mainWindow = this;
    }

    fHttp = new NInterface(this);
}

C5Dialog::C5Dialog(bool noparent) :
    QDialog(noparent ? nullptr : __mainWindow,  C5Config::isAppFullScreen() ? Qt::FramelessWindowHint : Qt::Window)
{
#ifdef WAITER
    QScreen *screen = QGuiApplication::primaryScreen();
    setMaximumSize(screen->geometry().size());
#endif

    if(!noparent && __mainWindow == nullptr) {
        __mainWindow = this;
    }

    fHttp = new NInterface(this);
}

C5Dialog::~C5Dialog()
{
    if(__mainWindow == this) {
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

void C5Dialog::showFullScreen()
{
#ifdef WAITER

    if(C5Config::isAppFullScreen()) {
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

void C5Dialog::setFieldValue(const QString &name, const QString &value)
{
    auto l  = this->findChild<C5LineEdit*>(name);

    if(l) {
        l->setText(value);
        return;
    }

    Q_ASSERT(false);
}

void C5Dialog::setFieldValue(const QString &name, double value)
{
    auto l  = this->findChild<C5LineEdit*>(name);

    if(l) {
        l->setDouble(value);
        return;
    }

    Q_ASSERT(false);
}

void C5Dialog::setFieldValue(const QString &name, int value)
{
    auto l  = this->findChild<C5LineEdit*>(name);

    if(l) {
        l->setInteger(value);
        return;
    }

    Q_ASSERT(false);
}

int C5Dialog::getFieldIntValue(const QString &name)
{
    auto l  = this->findChild<C5LineEdit*>(name);

    if(l) {
        return l->getInteger();
    }

    Q_ASSERT(false);
    return 0;
}

double C5Dialog::getFieldDoubleValue(const QString &name)
{
    auto l  = this->findChild<C5LineEdit*>(name);

    if(l) {
        return l->getDouble();
    }

    Q_ASSERT(false);
    return 0;
}

QString C5Dialog::getFieldStringValue(const QString &name)
{
    auto l  = this->findChild<C5LineEdit*>(name);

    if(l) {
        return l->text();
    }

    Q_ASSERT(false);
    return "";
}

void C5Dialog::keyPressEvent(QKeyEvent *e)
{
    switch(e->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return:
        if(e->modifiers() &Qt::ControlModifier) {
            keyControlPlusEnter();
        }

        if(e->modifiers() &Qt::AltModifier) {
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
