#include "c5guicontrols.h"
#include <QToolButton>

C5GuiControls __guic;

C5GuiControls::C5GuiControls()
{

}

bool C5GuiControls::isToolButton(QToolButton *&b, QObject *o)
{
    b = dynamic_cast<QToolButton*>(o);
    return b;
}
