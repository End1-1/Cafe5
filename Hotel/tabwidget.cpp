#include "tabwidget.h"
#include "widgetcontainer.h"

TabWidget::TabWidget() :
    WidgetContainer(),
    WidgetControls()
{

}

void TabWidget::setWidgetContainer()
{
    fCtrl = this;
    fCtrl->getWatchList(fCtrl);
}

bool TabWidget::allowClose()
{
    return true;
}

void TabWidget::showMenuPanel()
{

}
