#ifndef CE5DISCOUNTCARD_H
#define CE5DISCOUNTCARD_H

#include "ce5editor.h"

namespace Ui
{
class CE5DiscountCard;
}

class CE5DiscountCard : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5DiscountCard(QWidget *parent = nullptr);

    ~CE5DiscountCard();

    virtual void setId(int id) override;

    virtual QString title() override {return tr("Discount card");}

    virtual QString table() override {return "b_cards_discount";}

    bool save(QString &err, QList<QMap<QString, QVariant> >& data) override;

private slots:
    void on_btnNewClient_clicked();

    void on_leFirstName_textChanged(const QString &arg1);

    void on_leCard_returnPressed();

    void on_btnSetDays_clicked();

private:
    Ui::CE5DiscountCard* ui;
};

#endif // CE5DISCOUNTCARD_H
