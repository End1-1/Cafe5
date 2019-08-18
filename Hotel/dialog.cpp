#include "dialog.h"
#include "dashboard.h"

Dialog::Dialog() :
    QDialog(__dashboard, Qt::FramelessWindowHint),
    WidgetControls()
{
    setStyleSheet("QDialog {border: 1px solid blue;}");
}
