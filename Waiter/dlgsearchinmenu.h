#ifndef DLGSEARCHINMENU_H
#define DLGSEARCHINMENU_H

#include "c5dialog.h"
#include "struct_dish.h"

namespace Ui
{
class DlgSearchInMenu;
}

class QTableWidgetItem;

class DlgSearchInMenu : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgSearchInMenu(const QVector<DishAItem*>* dishes, int menu, C5User *user);

    ~DlgSearchInMenu();

    DishAItem* mDish = nullptr;

protected:
    virtual void showEvent(QShowEvent *e) override;

private:
    Ui::DlgSearchInMenu* ui;

    const QVector<DishAItem*>* mDishes;

    int mMenu;

private slots:
    void searchDish(const QString &name);

    void kbdAccept();

    void on_tbl_itemClicked(QTableWidgetItem *item);

signals:
    void dish(int, QString);
};

#endif // DLGSEARCHINMENU_H
