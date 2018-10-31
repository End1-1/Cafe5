#ifndef C5STOREDOC_H
#define C5STOREDOC_H

#include "c5widget.h"

namespace Ui {
class C5StoreDoc;
}

class C5StoreDoc : public C5Widget
{
    Q_OBJECT

public:
    enum STORE_DOC {sdInput = 1, sdOutput, sdMovement};

    explicit C5StoreDoc(const QStringList &dbParams, QWidget *parent = 0);

    ~C5StoreDoc();

    bool openDoc(int id);

    void setMode(STORE_DOC sd);

    virtual QToolBar *toolBar();

    static bool removeDoc(int id);

private:
    Ui::C5StoreDoc *ui;

    int fDocType;

    int fDocState;

    void countTotal();

    bool docCheck();

    void save(int state);

    void writeInput();

    bool writeOutput(const QDate &date, int docNum, int store, double &amount, QString &err);

    int addGoodsRow();

    void setDocEnabled(bool v);

private slots:
    void saveDoc();

    void draftDoc();

    void tblQtyChanged(const QString &arg1);

    void tblPriceChanged(const QString &arg1);

    void tblTotalChanged(const QString &arg1);

    void on_btnAddGoods_clicked();

    void on_btnRemoveGoods_clicked();
};

#endif // C5STOREDOC_H
