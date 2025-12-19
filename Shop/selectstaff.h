#ifndef SELECTSTAFF_H
#define SELECTSTAFF_H

#include "c5dialog.h"

namespace Ui
{
class SelectStaff;
}

class Working;
class C5User;

class SelectStaff : public C5Dialog
{
    Q_OBJECT

public:
    explicit SelectStaff(Working *w);

    ~SelectStaff();

    int mUserId = 0;

private slots:
    void on_leNum_returnPressed();

private:
    Ui::SelectStaff* ui;

    Working* fWorking;
};

#endif // SELECTSTAFF_H
