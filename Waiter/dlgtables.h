#pragma once

#include "c5waiterdialog.h"
#include "struct_hall.h"
#include "struct_table.h"

namespace Ui
{
class DlgTables;
}

class QTableWidgetItem;

class DlgTables : public C5WaiterDialog
{
    Q_OBJECT

public:
    explicit DlgTables(C5User *user);

    ~DlgTables();

    virtual int exec() override;

    QString mTableName;

protected:
    virtual void showEvent(QShowEvent *e) override;

private slots:
    void hallClicked();

    void on_btnCancel_clicked();

    void on_tblTables_itemClicked(QTableWidgetItem *item);

private:
    Ui::DlgTables* ui;

    QVector<HallItem> mHall;

    QVector<TableItem> mTables;

    int mResult = 0;

};
