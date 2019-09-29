#ifndef TABWIDGET_H
#define TABWIDGET_H

#include "widgetcontrols.h"
#include "widgetcontainer.h"

class TabWidget : public WidgetContainer, public WidgetControls
{
    Q_OBJECT
public:
    explicit TabWidget();
    virtual void setWidgetContainer();
    virtual bool allowClose();
    virtual void showMenuPanel();
};

#endif // TABWIDGET_H
