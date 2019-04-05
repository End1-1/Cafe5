#ifndef DLGSEARCHINMENU_H
#define DLGSEARCHINMENU_H

#include "c5dialog.h"

namespace Ui {
class DlgSearchInMenu;
}

class DlgSearchInMenu : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgSearchInMenu();

    ~DlgSearchInMenu();

    void buildMenu(const QString &name);

private:
    Ui::DlgSearchInMenu *ui;

private slots:
    void searchDish(const QString &name);

    void menuClicked();

    void kbdAccept();

signals:
    void dish(const QJsonObject &o);
};

#endif // DLGSEARCHINMENU_H
