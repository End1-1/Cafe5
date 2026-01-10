#ifndef DLGLISTDISHSPECIAL_H
#define DLGLISTDISHSPECIAL_H

#include "c5dialog.h"

namespace Ui
{
class DlgListDishSpecial;
}

class QListWidgetItem;

class DlgListDishSpecial : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgListDishSpecial(C5User *user);
    ~DlgListDishSpecial();
    static bool getSpecial(C5User *user, int dish, QString &special);

private slots:
    void on_lst_itemClicked(QListWidgetItem *item);

private:
    Ui::DlgListDishSpecial *ui;
    QString fResult;
};

#endif // DLGLISTDISHSPECIAL_H
