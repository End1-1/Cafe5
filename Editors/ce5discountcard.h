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

    void prepareLoad(int mode);

    virtual void setId(int id) override;

    virtual void clear() override;

    virtual QString title() override {return tr("Discount card");}

    virtual QString table() override;

    bool save(QString &err, QList<QMap<QString, QVariant> >& data) override;

private slots:
    void on_btnNewClient_clicked();

    void on_leFirstName_textChanged(const QString &arg1);

    void on_leCard_returnPressed();

    void on_btnSetDays_clicked();

private:
    Ui::CE5DiscountCard* ui;

    int mPendingMode = 0;

    QString mCardTable;

    bool isAccumulateMode(int mode) const;

    void applyTableForMode(int mode);
};

#endif // CE5DISCOUNTCARD_H
