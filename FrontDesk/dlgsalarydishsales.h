#pragma once

#include <QDate>
#include <QDialog>
#include <QJsonObject>

class C5User;

namespace Ui {
class DlgSalaryDishSales;
}

class DlgSalaryDishSales : public QDialog
{
    Q_OBJECT

public:
    explicit DlgSalaryDishSales(C5User *user,
                                const QDate &date,
                                int staffId,
                                const QString &staffName,
                                int positionId,
                                int positionCount,
                                QWidget *parent = nullptr);
    ~DlgSalaryDishSales() override;

private:
    void loadData();
    void fillTable(const QJsonObject &response);

    Ui::DlgSalaryDishSales *ui = nullptr;
    C5User *mUser = nullptr;
    QDate mDate;
    int mStaffId = 0;
    QString mStaffName;
    int mPositionId = 0;
    int mPositionCount = 1;
};
