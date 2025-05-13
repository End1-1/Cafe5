#ifndef C5SALARYDOC_H
#define C5SALARYDOC_H

#include "c5document.h"
#include "c5database.h"

namespace Ui
{
class C5SalaryDoc;
}

class C5SalaryDoc : public C5Document
{
    Q_OBJECT

public:
    explicit C5SalaryDoc(const QStringList &dbParams, QWidget *parent = nullptr);

    ~C5SalaryDoc();

    bool openDoc();

    virtual QToolBar *toolBar() override;

public slots:
    void save();

private slots:
    void countAmounts(const QString &arg1);

    void createCashDocument();

    void countSalaryResponse(const QJsonObject &jdoc);

    void getEmployesInOutList();

    void on_btnAddEmployee_clicked();

    void on_btnRemoveEmploye_clicked();

    void on_toolButton_clicked();

    void on_toolButton_2_clicked();

    void on_deDate_returnPressed();

    void on_cbShift_currentIndexChanged(int index);

private:
    Ui::C5SalaryDoc *ui;

    int newRow();

    QDate fDate;

    void getSessions();

    void countSalary();

    void removeSalaryOfShift();

    double salaryOfPosition(C5Database &db, int pos, int worker);
};

#endif // C5SALARYDOC_H
