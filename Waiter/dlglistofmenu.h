#ifndef DLGLISTOFMENU_H
#define DLGLISTOFMENU_H

#include "c5dialog.h"

namespace Ui {
class DlgListOfMenu;
}

class DlgListOfMenu : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgListOfMenu(const QStringList &dbParams);

    ~DlgListOfMenu();

    static bool getMenuId(int &id, const QStringList &dbParams);

private slots:
    void on_lst_clicked(const QModelIndex &index);

private:
    Ui::DlgListOfMenu *ui;

    int fMenuId;
};

#endif // DLGLISTOFMENU_H
