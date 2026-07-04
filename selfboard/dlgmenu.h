#ifndef DLGMENU_H
#define DLGMENU_H

#include <QDialog>
#include <QVector>

#include "webmenuclient.h"
#include "ordercart.h"

class SelfboardBottomChrome;

class DlgCart;
class DlgDishDetails;
class DlgOrderDone;
class DlgPackagePick;
class DlgPayment;
class DishCardAnimHost;
class DishCardWidget;
class QButtonGroup;
class QAnimationGroup;
class QPropertyAnimation;
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

    ServiceMode serviceMode() const { return m_serviceMode; }
    void setServiceMode(ServiceMode mode);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void changeEvent(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onGroupClicked(int groupId);
    void onAddToCart(int dishId, DishCardWidget *card = nullptr);
    void onDishInfo(int dishId);
    void onCancelOrder();
    void onGoToCart();
    void onServiceModeClicked();

private:
    void setupAppearance();
    void setupServiceModeButton();
    void setupTouchScroll();
    void buildSidebarGroups();
    void setupDishPanelHost();
    void syncDishPanelGeometry();
    void startGroupReveal(bool animatePanel);
    void selectGroup(int groupId, bool animatePanel = false);
    void rebuildDishGrid(bool prepareForAnimation = false);
    void clearLayout(QLayout *layout);
    void stopContentAnimations();
    void finalizeCardAppearance();
    void animatePanelSlideIn();
    void animateDishCardsStagger();
    void updateServiceModeLabel();
    void updateCartSummary();
    void playFlyToCart(DishCardWidget *card);
    QVector<MenuDish> filteredDishes() const;
    void showPackagePicker(const MenuDish &package);
    void closePackagePicker();
    void closeDishDetails();
    void showCartOverlay();
    void closeCartOverlay();
    void showPaymentOverlay();
    void closePaymentOverlay();
    void onCartOverlayFinished(int result);
    void onPaymentOverlayFinished(int result);
    void submitPaidOrder();
    void showOrderDone(const QString &orderNumber);
    void closeOrderDone();

    Ui::DlgMenu *ui;
    ServiceMode m_serviceMode;
    WebMenuClient m_menuClient;
    OrderCart m_cart;
    int m_currentGroupId = 0;
    QButtonGroup *m_groupButtons = nullptr;
    QVBoxLayout *m_groupsLayout = nullptr;
    QGridLayout *m_dishGridLayout = nullptr;
    QWidget *m_dishPanelHost = nullptr;
    QWidget *m_dishSlidePanel = nullptr;
    QVector<DishCardAnimHost *> m_lastBuiltCardHosts;
    QPropertyAnimation *m_panelSlideAnim = nullptr;
    QAnimationGroup *m_cardStaggerGroup = nullptr;
    DlgPackagePick *m_packagePick = nullptr;
    DlgDishDetails *m_dishDetails = nullptr;
    DlgCart *m_cartOverlay = nullptr;
    DlgPayment *m_paymentOverlay = nullptr;
    DlgOrderDone *m_orderDoneOverlay = nullptr;
    SelfboardBottomChrome *m_bottomChrome = nullptr;
    bool m_orderSubmitInProgress = false;
};

#endif // DLGMENU_H
