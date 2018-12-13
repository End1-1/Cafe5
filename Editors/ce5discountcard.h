#ifndef CE5DISCOUNTCARD_H
#define CE5DISCOUNTCARD_H

#include "ce5editor.h"

namespace Ui {
class CE5DiscountCard;
}

class CE5DiscountCard : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5DiscountCard(const QStringList &dbParams, QWidget *parent = 0);

    ~CE5DiscountCard();

    virtual QString title() {return tr("Discount card");}

    virtual QString table() {return "b_cards_discount";}

private slots:
    void on_btnNewClient_clicked();

    void on_leFirstName_textChanged(const QString &arg1);

private:
    Ui::CE5DiscountCard *ui;
};

#endif // CE5DISCOUNTCARD_H
