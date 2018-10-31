#include "c5filterwidget.h"

C5FilterWidget::C5FilterWidget(const QStringList &dbParams, QWidget *parent) :
    QWidget(parent)
{
    fDBParams = dbParams;
}
