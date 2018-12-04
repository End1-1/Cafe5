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

C5CheckBox *isCheckBox(QObject *o)
{
    C5CheckBox *ce = dynamic_cast<C5CheckBox*>(o);
    return ce;
}
