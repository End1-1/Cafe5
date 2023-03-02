#ifndef C5CASHDOC_H
#define C5CASHDOC_H

#include "c5document.h"

namespace Ui {
class C5CashDoc;
}

class C5CashDoc : public C5Document
{
    Q_OBJECT

public:
    explicit C5CashDoc(const QStringList &dbParams, QWidget *parent = nullptr);

    ~C5CashDoc() override;

    virtual QToolBar *toolBar() override;

    void setRelation(bool r);

    void setCashInput(int id);

    void setCashOutput(int id);

    void setPartner(int id);

    void setDate(const QDate &d);

    void setComment(const QString &t);

    void addRow(const QString &t, double a);

    void updateRow(int row, const QString &t, double a);

    bool openDoc(const QString &uuid);

    void setStoreDoc(const QString &uuid);

    virtual void selectorCallback(int row, const QList<QVariant> &values);

    QString uuid() const;

    QDate date() const;

    int inputCash();

    int outputCash();

    static bool removeDoc(const QStringList &dbParams, const QString &uuid);

public slots:
    void save(bool fromrelation = false);

private slots:
    void amountChanged(const QString &arg1);

    void draft();

    void removeDoc();

    void inputFromSale();

    void on_btnNewRow_clicked();

    void on_btnOpenStoreDoc_clicked();

    void on_btnRemoveRow_clicked();

private:
    Ui::C5CashDoc *ui;

    QString fUuid;
    
    QString fStoreUuid;

    bool fRelation;

    QAction *fActionFromSale;

    QStringList fRemovedRows;

    QAction *fActionSave;

    QAction *fActionDraft;

signals:
    void saved(const QString &);
};

#endif // C5CASHDOC_H
