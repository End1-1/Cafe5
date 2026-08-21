#pragma once

#include <QDialog>
#include <QJsonArray>

namespace Ui {
class DlgInventoryBlankStorePick;
}

class DlgInventoryBlankStorePick : public QDialog
{
    Q_OBJECT

public:
    explicit DlgInventoryBlankStorePick(QWidget *parent = nullptr);
    ~DlgInventoryBlankStorePick() override;

    void setStores(const QJsonArray &stores);

    /** -1 = all stores as one common blank; >0 = specific store. */
    int selectedStoreId() const;
    bool isCommonAll() const;

    static bool pick(const QJsonArray &stores, int &storeId, bool &commonAll, QWidget *parent = nullptr);

private:
    Ui::DlgInventoryBlankStorePick *ui;
};
