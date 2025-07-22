#ifndef C5SALARYPAYMENT_H
#define C5SALARYPAYMENT_H

#include "c5widget.h"

namespace Ui {
class C5SalaryPayment;
}

class C5SalaryPayment : public C5Widget
{
    Q_OBJECT

public:
    explicit C5SalaryPayment(QWidget *parent = nullptr);
    ~C5SalaryPayment();

    virtual QToolBar *toolBar();

public slots:
    void save();

private slots:
    void on_btnLeft_clicked();

    void on_btnRight_clicked();

    void on_deDate_returnPressed();

private:
    Ui::C5SalaryPayment *ui;

    int addRow();

    void openDoc(const QDate &date);
};

#endif // C5SALARYPAYMENT_H
