#ifndef C5SALARYDOC_H
#define C5SALARYDOC_H

#include "c5document.h"

namespace Ui {
class C5SalaryDoc;
}

class C5SalaryDoc : public C5Document
{
    Q_OBJECT

public:
    explicit C5SalaryDoc(const QStringList &dbParams, QWidget *parent = nullptr);

    ~C5SalaryDoc();

    bool openDoc(const QDate &date);

    virtual QToolBar *toolBar() override;

public slots:
    void save();

private slots:

    void getEmployesInOutList();

    void on_btnAddEmployee_clicked();

    void on_btnRemoveEmploye_clicked();

    void on_toolButton_clicked();

    void on_toolButton_2_clicked();

    void on_deDate_returnPressed();

private:
    Ui::C5SalaryDoc *ui;

    int newRow();

    void countSalary();

    double salaryOfPosition(C5Database &db, int pos, int worker);
};

#endif // C5SALARYDOC_H
