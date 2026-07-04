#pragma once

#include "c5widget.h"
#include <QDate>
#include <QShowEvent>

namespace Ui {
class C5SalaryPaymentEditor;
}

class C5SalaryPaymentEditor : public C5Widget
{
    Q_OBJECT

public:
    explicit C5SalaryPaymentEditor(QWidget *parent = nullptr);
    ~C5SalaryPaymentEditor() override;

    QToolBar *toolBar() override;

    void open(const QDate &date);

private slots:
    void on_btnAddStaff_clicked();
    void on_btnRemoveStaff_clicked();
    void on_btnChangePosition_clicked();
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
        colDebt,
        colAmount
    };

    Ui::C5SalaryPaymentEditor *ui;

    void initTable();
    void recalcTotal();
    void appendStaffRow(int staffId, const QString &staffName, int positionId, const QString &positionName, double debt);
    void offerAutofillFromAccrual(const QDate &date);
    void fillFromAccrual(const QDate &date);
    QString buildPrintHtml() const;

protected slots:
    void print() override;

protected:
    void showEvent(QShowEvent *e) override;

private:
    bool mLoadOnShow = false;
    QDate mOpenDate;
};
