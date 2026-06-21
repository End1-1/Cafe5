#ifndef SELFBOARDBOTTOMCHROME_H
#define SELFBOARDBOTTOMCHROME_H

#include <QWidget>

class QLabel;
class QPushButton;
class QToolButton;

class SelfboardBottomChrome : public QWidget
{
    Q_OBJECT

public:
    explicit SelfboardBottomChrome(QWidget *parent = nullptr);

    void setCartCount(int count);
    void setCartTotal(double total);
    void setGoToCartEnabled(bool enabled);

    QWidget *cartSummaryWidget() const;

signals:
    void cancelOrderClicked();
    void goToCartClicked();
    void cartSummaryClicked();

protected:
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void buildUi();
    void positionQrPanel();

    QWidget *m_upperHost = nullptr;
    QLabel *m_qrPanel = nullptr;
    QWidget *m_cartSummary = nullptr;
    QLabel *m_lblCartBadge = nullptr;
    QLabel *m_lblCartAmount = nullptr;
    QPushButton *m_btnCancel = nullptr;
    QPushButton *m_btnGoToCart = nullptr;
};

#endif // SELFBOARDBOTTOMCHROME_H
