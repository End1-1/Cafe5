#include "c5lineedit.h"
#include "c5utils.h"
#include <QValidator>
#include <QKeyEvent>

static QLocale mLocale;

C5LineEdit::C5LineEdit(QWidget *parent) :
    QLineEdit(parent)
{
    fColor = -1;
    fDecimalPlaces = 2;
    connect(this, SIGNAL(textEdited(QString)), this, SLOT(editText(QString)));
}

void C5LineEdit::setText(const QString &arg)
{
    QString t = arg;
    QLineEdit::setText(t);
}

void C5LineEdit::appendText(const QString &arg)
{
    setText(text() + arg);
}

void C5LineEdit::setInteger(int i)
{
    setText(QString::number(i));
}

int C5LineEdit::getInteger()
{
    return text().toInt();
}

void C5LineEdit::setDouble(double i)
{
    QString si = float_str(i, fDecimalPlaces);
    setText(si);
}

double C5LineEdit::getDouble()
{
    QString t = text();
    t.replace(mLocale.groupSeparator(), "");
    double d = mLocale.toDouble(t);
    return d;
}

void C5LineEdit::setData(const QVariant &data)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    switch (data.type()) {
        case QVariant::Double:
            setDouble(data.toDouble());
            break;
        default:
            setText(data.toString());
            break;
    }
#else
    switch (data.typeId()) {
        case QMetaType::Double:
            setDouble(data.toDouble());
            break;
        default:
            setText(data.toString());
            break;
    }
#endif
}

int C5LineEdit::getTag()
{
    return fTag;
}

void C5LineEdit::setTag(int tag)
{
    fTag = tag;
}

void C5LineEdit::setColor(int c)
{
    fColor = c;
    QPalette palette;
    QColor color = QColor::fromRgb(c);
    palette.setColor(QPalette::Base, color);
    palette.setColor(QPalette::Text, color);
    setPalette(palette);
}

int C5LineEdit::color()
{
    return fColor;
}

void C5LineEdit::addEventKeys(const QString &keys)
{
    fEventKeys.append(keys);
}

void C5LineEdit::fixValue()
{
    setProperty("old", text());
}

QString C5LineEdit::old()
{
    return property("old").toString();
}

void C5LineEdit::setBgColor(const QColor &color)
{
    QPalette palette;
    palette.setColor(QPalette::Base, color);
    setPalette(palette);
}

void C5LineEdit::keyPressEvent(QKeyEvent *e)
{
    const QValidator *v = validator();
    if (v) {
        if (!strcmp(v->metaObject()->className(), "QDoubleValidator")) {
            const QDoubleValidator *dv = static_cast<const QDoubleValidator *>(v);
            switch (e->key()) {
                case 43:
                    //e->accept();
                    //e->ignore();
                    //break;
                    return;
                case 45:
                    if (dv->bottom() > 0.0000001) {
                        return;
                    }
                    break;
                case 46:
                case 44:
                case 8228:
                    e = new QKeyEvent (QEvent::KeyRelease, Qt::Key_A, Qt::NoModifier, QLocale().decimalPoint());
                    break;
                case 16777219:
                    //backspace ;)
                    break;
                case 16777220:
                case 16777221:
                    if (isEmpty()) {
                        emit returnPressed();
                    }
                    break;
            }
        }
    }
    if (!fEventKeys.isEmpty()) {
        int key = e->key();
        if (key >= 0x20 && key <= 0x10FFFF) {
            QChar ch(static_cast<char32_t>(key));
            if (fEventKeys.contains(ch)) {
                emit keyPressed(ch);
                e->ignore();
                return;
            }
        }
    }
    QLineEdit::keyPressEvent(e);
}

void C5LineEdit::keyReleaseEvent(QKeyEvent *event)
{
    if (!fEventKeys.isEmpty()) {
        int key = event->key();
        if (key >= 0x20 && key <= 0x10FFFF) {
            if (fEventKeys.contains(QChar(event->key()))) {
                emit keyPressed(QChar(event->key()));
                event->ignore();
            }
        }
    }
    QLineEdit::keyReleaseEvent(event);
}

void C5LineEdit::focusOutEvent(QFocusEvent *event)
{
    QLineEdit::focusOutEvent(event);
    emit focusOut();
}

void C5LineEdit::focusInEvent(QFocusEvent *event)
{
    QLineEdit::focusInEvent(event);
    emit focusIn();
}

void C5LineEdit::editText(const QString &arg)
{
    int cursPos = cursorPosition();
    setText(arg);
    setCursorPosition(cursPos);
    if (property("old").isValid()) {
        if (property("old").toString().compare(text(), Qt::CaseInsensitive) == 0) {
            setBgColor(Qt::white);
        } else {
            setBgColor(Qt::yellow);
        }
    }
}

C5LineEdit *isLineEdit(QObject *o)
{
    C5LineEdit *le = dynamic_cast<C5LineEdit *>(o);
    return le;
}
