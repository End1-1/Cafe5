#include "c5dialog.h"
#include "c5message.h"
#include "c5lineedit.h"
#include "ninterface.h"
#include <QKeyEvent>
#include <QApplication>
#include <QScreen>
#include <QWindow>
#include <QApplication>
#include <QPainter>
#include <QTimer>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

static QWidget* __mainWindow = nullptr;
int C5Dialog::mScreen = -1;

C5Dialog::C5Dialog(C5User *user) :
    QDialog(__mainWindow),
    mUser(user)
{
#ifdef BORDERLESSDIALOGS
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
#endif

    if(__mainWindow == nullptr) {
        __mainWindow = this;
    }

    if(mScreen > -1) {
        QScreen *screen = qApp->screens().at(mScreen);
        this->create();

        if(windowHandle()) {
            windowHandle()->setScreen(screen);
        }
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

void C5Dialog::updateRequired(const QString &msg, const QString &appName, const QString &newVersion)
{
    Q_UNUSED(appName);
    Q_UNUSED(newVersion);
    C5Message::info(msg);
    qApp->exit(0);
}

void C5Dialog::resizeEvent(QResizeEvent *e)
{
    QDialog::resizeEvent(e);
    updateBackgroundCache();
}

void C5Dialog::moveEvent(QMoveEvent *e)
{
    QDialog::moveEvent(e);
    updateBackgroundCache();
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

void C5Dialog::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);
    QTimer::singleShot(0, this, &C5Dialog::updateBackgroundCache);

    if(e->spontaneous()) {
        return;
    }

    QScreen *screen = qApp->screens().first();

    if(mScreen > -1) {
        screen = qApp->screens().at(mScreen);
    }

    QRect geo = screen->availableGeometry();
    QPoint center = geo.center();
    QSize size = this->size();

    if(mInitialPos == QPoint(-1, -1)) {
        mInitialPos = QPoint(center.x() - size.width() / 2, center.y() - size.height() / 2);
    }

    move(mInitialPos);
}

QImage C5Dialog::grabDesktopArea(const QRect &r)
{
#ifdef Q_OS_WIN
    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, r.width(), r.height());
    SelectObject(hDC, hBitmap);
    BitBlt(hDC, 0, 0, r.width(), r.height(),
           hScreen, r.x(), r.y(), SRCCOPY);
    BITMAP bmp;
    GetObject(hBitmap, sizeof(BITMAP), &bmp);
    QImage img(bmp.bmWidth, bmp.bmHeight, QImage::Format_RGB32);
    GetBitmapBits(hBitmap, bmp.bmHeight * bmp.bmWidthBytes, img.bits());
    DeleteObject(hBitmap);
    DeleteDC(hDC);
    ReleaseDC(NULL, hScreen);
    return img;
#endif
}

QColor C5Dialog::contrast(const QColor & c)
{
    int brightness = (c.red() * 299 +
                      c.green() * 587 +
                      c.blue() * 114) / 1000;
    return (brightness < 128)
           ? QColor("#6b7a84")   // светлая рамка на тёмном фоне
           : QColor("#495963");
};

void C5Dialog::paintEvent(QPaintEvent *e)
{
    QDialog::paintEvent(e);
    QPainter p(this);
    int step = 4;

    for(int i = 0; i < mTopBorder.size(); i++) {
        p.setPen(mTopBorder[i]);
        p.drawLine(i * step, 0, i * step + step, 0);
    }

    for(int i = 0; i < mBottomBorder.size(); i++) {
        p.setPen(mBottomBorder[i]);
        p.drawLine(i * step, height() - 1, i * step + step, height() - 1);
    }

    for(int i = 0; i < mLeftBorder.size(); i++) {
        p.setPen(mLeftBorder[i]);
        p.drawLine(0, i * step, 0, i * step + step);
    }

    for(int i = 0; i < mRightBorder.size(); i++) {
        p.setPen(mRightBorder[i]);
        p.drawLine(width() - 1, i * step, width() - 1, i * step + step);
    }
}

void C5Dialog::handleError(int err, const QString &msg)
{
    Q_UNUSED(err);
    C5Message::error(msg);
}

void C5Dialog::buildAdaptiveBorder()
{
    const int step = 4;
    const int offset = 2;
    mTopBorder.clear();

    for(int x = 0; x < width(); x += step)
        mTopBorder << contrast(mBgCache.pixelColor(x + offset, 1));

    mBottomBorder.clear();

    for(int x = 0; x < width(); x += step)
        mBottomBorder << contrast(mBgCache.pixelColor(x + offset, offset + height()));

    mLeftBorder.clear();

    for(int y = 0; y < height(); y += step)
        mLeftBorder << contrast(mBgCache.pixelColor(1, y + offset));

    mRightBorder.clear();

    for(int y = 0; y < height(); y += step)
        mRightBorder << contrast(mBgCache.pixelColor(offset + width(), y + offset));
}

void C5Dialog::updateBackgroundCache()
{
    QRect g = frameGeometry();
    mBgCache = grabDesktopArea(g.adjusted(-2, -2, 2, 2));
    buildAdaptiveBorder();
}
