#ifndef WIDGETCONTROLS_H
#define WIDGETCONTROLS_H

#include <QString>

class WidgetContainer;

class WidgetControls
{
public:
    WidgetControls();
    virtual void setWidgetContainer() = 0;
    const QString strw(const QString &key) const;
    void setStrw(const QString &key, const QString &value);

protected:
    WidgetContainer *fCtrl;
};

#endif // WIDGETCONTROLS_H
