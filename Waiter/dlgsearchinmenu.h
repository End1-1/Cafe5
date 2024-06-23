#ifndef DLGSEARCHINMENU_H
#define DLGSEARCHINMENU_H

#include "c5dialog.h"

namespace Ui
{
class DlgSearchInMenu;
}

class DlgSearchInMenu : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgSearchInMenu();

    ~DlgSearchInMenu();

    void buildMenu(int menuid);

private:
    Ui::DlgSearchInMenu *ui;

    void extractDishes(int &p2);

private slots:
    void searchDish(const QString &name);

    void menuClicked();

    void kbdAccept();

signals:
    void dish(int);
};

#endif // DLGSEARCHINMENU_H
