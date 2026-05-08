#pragma once

#include "c5widget.h"
#include <QDate>
#include <QShowEvent>

namespace Ui
{
class C5SalaryEditor;
}

class C5SalaryEditor : public C5Widget
{
    Q_OBJECT

public:
    explicit C5SalaryEditor(QWidget *parent = nullptr);
    ~C5SalaryEditor() override;

    QToolBar *toolBar() override;

    // Sets initial state and triggers lazy load on showEvent.
    void open(const QDate &date, int type);

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
        colAmount
    };

    Ui::C5SalaryEditor *ui;

    void initTable();
    void recalcTotal();
    QString buildPrintHtml() const;

protected slots:
    void print() override;

protected:
    void showEvent(QShowEvent *e) override;

private:
    bool mLoadOnShow = false;
    QDate mOpenDate;
    int mOpenType = 1;
};
