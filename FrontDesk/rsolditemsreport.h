#pragma once

#include "rabstracteditorreport.h"

class RSoldItemsReport : public RAbstractEditorReport
{
    Q_OBJECT

public:
    explicit RSoldItemsReport(const QString &title, QIcon icon, const QString &editorName,
                              QWidget *parent = nullptr);

protected:
    void showEvent(QShowEvent *e) override;
    void on_tbl_doubleClicked(const QModelIndex &index) override;

private:
    void setupContextMenu();
    void showContextMenu(const QPoint &pos);
    void openSelectedItemByOrders();

    int selectedGoodsId(QString *itemName = nullptr) const;
    void currentPeriod(QString *date1, QString *date2, int *groupId, int *fiscalReceipt) const;

    bool mContextMenuReady = false;
};
