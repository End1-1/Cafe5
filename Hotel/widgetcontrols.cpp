#include "widgetcontrols.h"
#include "widgetcontainer.h"

WidgetControls::WidgetControls()
{
    fCtrl = nullptr;
}

const QString WidgetControls::strw(const QString &key) const
{
    return fCtrl->getString(key);
}

void WidgetControls::setStrw(const QString &key, const QString &value)
{
    fCtrl->setString(key, value);
}
