#include "c5lineedit.h"
#include <QValidator>


C5LineEdit::C5LineEdit(QWidget *parent) :
    QLineEdit(parent)
{
    fColor = -1;
}

void C5LineEdit::setText(const QString &arg)
{
    QString t = arg;
    const QValidator *v = validator();
    if (v) {
        if (!strcmp(v->metaObject()->className(), "QDoubleValidator")) {
            QLocale l;
            t.replace(".", l.decimalPoint());
            t.replace(",", l.decimalPoint());
            t.replace("․", l.decimalPoint());
        }
    }
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
    setText(QString::number(i, 'f', 2).remove(QRegExp("\\.0+$")).remove(QRegExp("\\.$")));
}

double C5LineEdit::getDouble()
{
    return text().toDouble();
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
