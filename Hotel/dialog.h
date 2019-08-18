#ifndef DIALOG_H
#define DIALOG_H

#include "widgetcontrols.h"
#include <QDialog>

class Dialog : public QDialog, public WidgetControls
{
    Q_OBJECT
public:
    explicit Dialog();
};

#endif // DIALOG_H
