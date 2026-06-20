#ifndef DLGPAYMENT_H
#define DLGPAYMENT_H

#include "ordercart.h"

#include <QWidget>

class QPaintEvent;
class QShowEvent;

namespace Ui {
class DlgPayment;
}

class DlgPayment : public QWidget
{
    Q_OBJECT

public:
    explicit DlgPayment(OrderCart *cart, QWidget *parent = nullptr);
    ~DlgPayment() override;

    bool backToCartRequested() const { return m_backToCart; }

signals:
    void finished(int result);

protected:
    void paintEvent(QPaintEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void changeEvent(QEvent *event) override;

private slots:
    void on_btnBackToCart_clicked();
    void startPayment();

private:
    void setupAppearance();
    void updateTotal();
    void showPaymentError(const QString &message);
    void runPaymentStub();

    Ui::DlgPayment *ui;
    OrderCart *m_cart;
    bool m_backToCart = false;
    bool m_paymentStarted = false;
};

#endif // DLGPAYMENT_H
