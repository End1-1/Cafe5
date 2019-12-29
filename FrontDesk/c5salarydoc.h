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

    bool openDoc(const QString &id);

    virtual QToolBar *toolBar() override;

    static bool removeDocument(const QStringList &dbParams, const QString &uuid);

public slots:
    void save();

private slots:
    void countSalary();

    void countAmounts(const QString &arg1);

    void createCashDocument();

    void removeDoc(bool showmessage = true);

    void on_btnAddEmployee_clicked();

    void on_btnRemoveEmploye_clicked();

private:
    Ui::C5SalaryDoc *ui;

    QString fUUID;

    QString fCashUUID;

    int newRow();

    void setPaid();
};

#endif // C5SALARYDOC_H
