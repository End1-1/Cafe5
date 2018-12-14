#include "c5lineedit.h"
#include "c5utils.h"
#include <QValidator>
#include <QKeyEvent>

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
    return QLocale().toDouble(text());
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

void C5LineEdit::keyPressEvent(QKeyEvent *e)
{
    const QValidator *v = validator();
    if (v) {
        if (!strcmp(v->metaObject()->className(), "QDoubleValidator")) {
            switch (e->key()) {
            case 46:
            case 44:
            case 8228:
                e = new QKeyEvent (QEvent::KeyRelease,Qt::Key_A,Qt::NoModifier, QLocale().decimalPoint());
                break;
            }
        }
    }
    QLineEdit::keyPressEvent(e);
}

void C5LineEdit::editText(const QString &arg)
{
    int cursPos = cursorPosition();
    setText(arg);
    setCursorPosition(cursPos);
}

C5LineEdit *isLineEdit(QObject *o)
{
    C5LineEdit *le = dynamic_cast<C5LineEdit*>(o);
    return le;
}
