#include "c5checkbox.h"

C5CheckBox::C5CheckBox(QWidget *parent) :
    QCheckBox(parent)
{
    fTag = 0;
}

C5CheckBox::~C5CheckBox()
{

}

int C5CheckBox::getTag()
{
    return fTag;
}

void C5CheckBox::setTag(int tag)
{
    fTag = tag;
}
