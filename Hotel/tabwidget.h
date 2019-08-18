#ifndef TABWIDGET_H
#define TABWIDGET_H

#include "widgetcontrols.h"
#include "widgetcontainer.h"

class TabWidget : public WidgetContainer, public WidgetControls
{
    Q_OBJECT
public:
    explicit TabWidget();
    virtual const QString title() = 0;
    virtual const QString icon() = 0;
    virtual void setWidgetContainer();
};

#endif // TABWIDGET_H
