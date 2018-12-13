#ifndef CE5CREDITCARD_H
#define CE5CREDITCARD_H

#include "ce5editor.h"

namespace Ui {
class CE5CreditCard;
}

class CE5CreditCard : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5CreditCard(const QStringList &dbParams, QWidget *parent = 0);

    ~CE5CreditCard();

    virtual QString title() {return tr("Credit card");}

    virtual QString table() {return "o_credit_card";}

protected:
    virtual void setId(int id);

    virtual void clear();

    bool save(QString &err, QList<QMap<QString, QVariant> > &data);

private slots:
    void on_pushButton_clicked();

private:
    Ui::CE5CreditCard *ui;
};

#endif // CE5CREDITCARD_H
