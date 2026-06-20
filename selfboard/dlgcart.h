#ifndef DLGCART_H
#define DLGCART_H

#include "ordercart.h"

#include <QWidget>

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

private:
    void setupAppearance();
    void setupTouchScroll();
    void rebuildCartList();
    void updateSummary();
    QString modifiersText(const MenuDish &dish) const;
    void clearLayout(QLayout *layout);

    Ui::DlgCart *ui;
    OrderCart *m_cart;
};

#endif // DLGCART_H
