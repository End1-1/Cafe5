#ifndef C5CASHDOC_H
#define C5CASHDOC_H

#include "c5widget.h"

namespace Ui {
class C5CashDoc;
}

class C5CashDoc : public C5Widget
{
    Q_OBJECT

public:
    explicit C5CashDoc(const QStringList &dbParams, QWidget *parent = nullptr);

    ~C5CashDoc() override;

    virtual QToolBar *toolBar() override;

    bool openDoc(const QString &uuid);

    void setStoreDoc(const QString &uuid);

    static bool removeDoc(const QStringList &dbParams, const QString &uuid);

private slots:
    void amountChanged(const QString &arg1);

    void save();

    void removeDoc();

    void inputFromSale();

    void on_btnNewRow_clicked();

    void on_btnOpenStoreDoc_clicked();

private:
    Ui::C5CashDoc *ui;

    QString fUuid;
    
    QString fStoreUuid;
};

#endif // C5CASHDOC_H
