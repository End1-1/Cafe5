#pragma once

#include <QList>
#include <QString>
#include <QWidget>

namespace Ui
{
class DlgCustDisplay;
}

struct CustDisplayLine {
    QString name;
    QString qtyText;
    QString priceText;
    QString amountText;
};

struct CustDisplayTotals {
    QString subtotalText;
    QString serviceCaption;
    QString serviceText;
    bool showService = false;
    QString discountCaption;
    QString discountText;
    bool showDiscount = false;
    QString totalDueText;
};

/** Customer-facing screen: dish lines and totals. */
class DlgCustDisplay : public QWidget
{
    Q_OBJECT

public:
    explicit DlgCustDisplay(QWidget *parent = nullptr);
    ~DlgCustDisplay() override;

    void setContent(const QList<CustDisplayLine> &lines, const CustDisplayTotals &totals);

    void showOnSecondScreen();

private:
    QWidget *createDishRowWidget(const CustDisplayLine &line);

    Ui::DlgCustDisplay *ui;
};
