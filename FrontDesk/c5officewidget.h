#ifndef C5OFFICEWIDGET_H
#define C5OFFICEWIDGET_H

#include <c5widget.h>
#include <QObject>

class C5User;

class C5OfficeWidget : public C5Widget
{
public:
    C5OfficeWidget(QWidget *parent = nullptr);
    static C5User* mUser;
protected:

};

#endif // C5OFFICEWIDGET_H
