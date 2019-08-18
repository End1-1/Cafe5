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
