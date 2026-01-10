#ifndef DLGGUEST_H
#define DLGGUEST_H

#include "c5dialog.h"

namespace Ui {
class DlgGuest;
}

class DlgGuest : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgGuest(C5User *user);

    ~DlgGuest();

    static bool getGuest(C5User *user, QString &res, QString &inv, QString &room, QString &guest);

private:
    Ui::DlgGuest *ui;

    int fRow;

private slots:
    void searchGuest(const QString &txt);

    void kbdAccept();

    void on_tblGuest_doubleClicked(const QModelIndex &index);
};

#endif // DLGGUEST_H
