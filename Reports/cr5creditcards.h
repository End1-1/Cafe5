#ifndef CR5CREDITCARDS_H
#define CR5CREDITCARDS_H

#include "c5reportwidget.h"

class CR5CreditCards : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5CreditCards(QWidget *parent = nullptr);


    virtual QToolBar *toolBar();
};

#endif // CR5CREDITCARDS_H
