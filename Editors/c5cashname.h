#ifndef C5CASHNAME_H
#define C5CASHNAME_H

#include "ce5editor.h"

namespace Ui {
class C5CashName;
}

class C5CashName : public CE5Editor
{
    Q_OBJECT

public:
    explicit C5CashName(QWidget *parent = nullptr);

    ~C5CashName();

    virtual QString title();

    virtual QString table();

private:
    Ui::C5CashName *ui;
};

#endif // C5CASHNAME_H
