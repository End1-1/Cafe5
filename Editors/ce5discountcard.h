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
    explicit CE5DiscountCard(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CE5DiscountCard();

    virtual QString title() {return tr("Discount card");}

    virtual QString table() {return "b_cards_discount";}

    bool save(QString &err, QList<QMap<QString, QVariant> > &data);

private slots:
    void on_btnNewClient_clicked();

    void on_leFirstName_textChanged(const QString &arg1);

private:
    Ui::CE5DiscountCard *ui;
};

#endif // CE5DISCOUNTCARD_H
