#ifndef C5CASHDOC_H
#define C5CASHDOC_H

#include "c5document.h"
#include "c5database.h"

namespace Ui
{
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

    void loadSuggest();

    void addRow(const QString &t, double a);

    void updateRow(int row, const QString &t, double a);

    bool openDoc(const QString &uuid);

    void setStoreDoc(const QString &uuid);

    virtual void selectorCallback(int row, const QJsonArray &values) override;

    QString uuid() const;

    QDate date() const;

    int inputCash();

    int outputCash();

    QString fUuid;

    int fBClientDebtId;

    int fDebtSource;

    int fDebtFlag = 0;

    bool fNoSavedMessage = false;

    static bool removeDoc(const QStringList &dbParams, const QString &uuid);

    static bool removeDoc(C5Database &db, const QString &uuid);

    bool save(bool writedebt, bool fromrelation = false);

public slots:
    void saveDoc();

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
