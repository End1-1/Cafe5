#ifndef CE5MFWORKSHOP_H
#define CE5MFWORKSHOP_H

#include "ce5editor.h"

namespace Ui {
class CE5MFWorkshop;
}

class CE5MFWorkshop : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5MFWorkshop(QWidget *parent = nullptr);

    ~CE5MFWorkshop();

    virtual QString title();

    virtual QString table();

private:
    Ui::CE5MFWorkshop *ui;
};

#endif // CE5MFWORKSHOP_H
