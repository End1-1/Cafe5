#ifndef DLGMENU_H
#define DLGMENU_H

#include <QDialog>

#include "menustubclient.h"
#include "ordercart.h"

class QButtonGroup;
class QGridLayout;
class QHBoxLayout;
class QVBoxLayout;

namespace Ui {
class DlgMenu;
}

class DlgMenu : public QDialog
{
    Q_OBJECT

public:
    enum class ServiceMode { TakeAway, DineIn };

    explicit DlgMenu(ServiceMode mode, QWidget *parent = nullptr);
    ~DlgMenu() override;

protected:
    void changeEvent(QEvent *event) override;

private slots:
    void onGroupClicked(int groupId);
    void onAddToCart(int dishId);
    void onDishInfo(int dishId);
    void onSearchTextChanged(const QString &text);
    void onCancelOrder();
    void onGoToCart();

private:
    void setupAppearance();
    void setupTouchScroll();
    void buildSidebarGroups();
    void buildGroupChips();
    void selectGroup(int groupId);
    void rebuildDishGrid();
    void clearLayout(QLayout *layout);
    void updateServiceModeLabel();
    void updateCartSummary();
    QVector<MenuDish> filteredDishes() const;

    Ui::DlgMenu *ui;
    ServiceMode m_serviceMode;
    MenuStubClient m_menuClient;
    OrderCart m_cart;
    int m_currentGroupId = 0;
    QButtonGroup *m_groupButtons = nullptr;
    QButtonGroup *m_chipButtons = nullptr;
    QVBoxLayout *m_groupsLayout = nullptr;
    QHBoxLayout *m_chipsLayout = nullptr;
    QGridLayout *m_dishGridLayout = nullptr;
};

#endif // DLGMENU_H
