#ifndef C5GUICONTROLS_H
#define C5GUICONTROLS_H

class QToolButton;
class QObject;

class C5GuiControls
{
public:
    C5GuiControls();

    bool isToolButton(QToolButton *&b, QObject *o);
};

extern C5GuiControls __guic;

#endif // C5GUICONTROLS_H
