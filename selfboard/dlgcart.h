#ifndef DLGCART_H
#define DLGCART_H

#include "ordercart.h"

#include <QWidget>

class DlgMenu;
class QLabel;
class QPaintEvent;

namespace Ui {
class DlgCart;
}

class DlgCart : public QWidget
{
    Q_OBJECT

public:
    static constexpr int ResultGoToPay = 2;

    explicit DlgCart(OrderCart *cart, QWidget *parent = nullptr);
    ~DlgCart() override;

signals:
    void finished(int result);

protected:
    void paintEvent(QPaintEvent *event) override;
    void changeEvent(QEvent *event) override;

public slots:
    void onQuantityChanged(const QString &lineKey, int quantity);

private slots:
    void on_btnOrderMore_clicked();
    void on_btnGoToPay_clicked();
    void onServiceModeClicked();

private:
    void setupAppearance();
    void setupHeaderLayout();
    void setupServiceModeButton();
    void updateServiceModeLabel();
    void setupOrderBarSummary();
    void setupTouchScroll();
    void rebuildCartList();
    void updateSummary();
    QString lineSubtitle(const MenuDish &dish) const;
    QString modifiersText(const MenuDish &dish) const;
    void clearLayout(QLayout *layout);

    Ui::DlgCart *ui;
    OrderCart *m_cart;
    DlgMenu *m_menu = nullptr;
    QLabel *m_lblCartBadge = nullptr;
    QLabel *m_lblCartAmount = nullptr;
};

#endif // DLGCART_H
