#ifndef DLGVIEWSTOPLIST_H
#define DLGVIEWSTOPLIST_H

#include "c5dialog.h"

namespace Ui {
class DlgViewStopList;
}

class DlgViewStopList : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgViewStopList();
    ~DlgViewStopList();

private:
    Ui::DlgViewStopList *ui;
};

#endif // DLGVIEWSTOPLIST_H
