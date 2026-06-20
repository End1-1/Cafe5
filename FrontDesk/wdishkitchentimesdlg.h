#pragma once

#include <QDialog>
#include <QJsonObject>
#include "struct_waiter_dish.h"

class C5User;

namespace Ui
{
class WDishKitchenTimesDlg;
}

class WDishKitchenTimesDlg : public QDialog
{
    Q_OBJECT
public:
    explicit WDishKitchenTimesDlg(QWidget *parent = nullptr);
    ~WDishKitchenTimesDlg() override;

    void setUser(C5User *user);
    void setDish(const WaiterDish &dish);

    /** Filled after successful save (order from API). */
    QJsonObject savedOrder() const { return mSavedOrder; }

private slots:
    void onSaveClicked();

private:
    static QString normalizeTimeText(const QString &raw);
    bool collectAndSave();

    Ui::WDishKitchenTimesDlg *ui = nullptr;
    C5User *mUser = nullptr;
    QString mDishId;
    QString mHeaderId;
    QJsonObject mSavedOrder;
};
