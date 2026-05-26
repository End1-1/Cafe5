#pragma once

#include "c5widget.h"
#include <QDate>
#include <QShowEvent>

namespace Ui {
class C5SalaryEditor;
}

class C5SalaryEditor : public C5Widget
{
    Q_OBJECT

public:
    explicit C5SalaryEditor(QWidget *parent = nullptr);
    ~C5SalaryEditor() override;

    QToolBar *toolBar() override;

    /** Opens salary accrual document (f_type = 1). */
    void open(const QDate &date);

private slots:
    void on_btnAddStaff_clicked();
    void on_btnRemoveStaff_clicked();
    void on_btnChangePosition_clicked();
    void calculateDocument();
    void saveDocument();
    void removeDocument();

private:
    enum Columns {
        colDbId = 0,
        colPositionId,
        colStaffId,
        colNum,
        colPosition,
        colName,
        colFixed,
        colDishBase,
        colCalculated,
        colBonus,
        colTotal
    };

    Ui::C5SalaryEditor *ui;

    void initTable();
    void recalcTotal();
    void updateRowTotal(int row);
    void appendStaffRow(int staffId, const QString &staffName, int positionId, const QString &positionName);
    void applyCalculatedRow(int row, double fixed, double dishBase, double calculated, double total);
    QString buildPrintHtml() const;

protected slots:
    void print() override;

protected:
    void showEvent(QShowEvent *e) override;

private:
    bool mLoadOnShow = false;
    QDate mOpenDate;
};
