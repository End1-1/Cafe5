#include "wpwidget.h"
#include "c5user.h"

WPWidget::WPWidget(C5User *user, QWidget *parent) :
    QWidget(parent),
    mUser(user)
{
}
