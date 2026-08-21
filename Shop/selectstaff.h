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
    explicit SelectStaff(Working *w, C5User *user);

    ~SelectStaff();

    int mUserId = 0;

    /** Show staff cards; on success writes staffId. */
    static bool select(Working *w, C5User *user, int &staffId);

private slots:
    void on_leNum_returnPressed();

private:
    Ui::SelectStaff* ui;

    Working* fWorking;
};

#endif // SELECTSTAFF_H
