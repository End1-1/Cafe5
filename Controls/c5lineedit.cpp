#include "c5lineedit.h"

C5LineEdit::C5LineEdit(QWidget *parent) :
    QLineEdit(parent)
{

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
