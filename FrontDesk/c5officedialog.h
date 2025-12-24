#ifndef C5OFFICEDIALOG_H
#define C5OFFICEDIALOG_H

#include "c5dialog.h"
#include <QObject>

class C5User;

class C5OfficeDialog : public C5Dialog
{
public:
    C5OfficeDialog(C5User *user);
};

#endif // C5OFFICEDIALOG_H
