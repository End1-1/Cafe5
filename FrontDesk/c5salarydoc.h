#ifndef C5SALARYDOC_H
#define C5SALARYDOC_H

#include "c5widget.h"

namespace Ui {
class C5SalaryDoc;
}

class C5SalaryDoc : public C5Widget
{
    Q_OBJECT

public:
    explicit C5SalaryDoc(const QStringList &dbParams, QWidget *parent = nullptr);

    ~C5SalaryDoc();

    virtual QToolBar *toolBar() override;

public slots:
    void save();

private slots:
    void countAmounts(const QString &arg1);

    void on_btnAddEmployee_clicked();

    void on_btnRemoveEmploye_clicked();

private:
    Ui::C5SalaryDoc *ui;

    QString fUUID;

    QString fCashUUID;
};

#endif // C5SALARYDOC_H
