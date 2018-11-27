#ifndef C5STOREDOC_H
#define C5STOREDOC_H

#include "c5widget.h"

namespace Ui {
class C5StoreDoc;
}

class QTableWidgetItem;

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

    static bool removeDoc(const QStringList &dbParams, int id, bool showmessage = true);

    bool save(int state, QString &err, bool showMsg);

private:
    Ui::C5StoreDoc *ui;

    int fDocType;

    int fDocState;

    void countTotal();

    bool docCheck(QString &err);

    bool save(int state, QString &err);

    void writeInput();

    bool writeOutput(const QDate &date, int docNum, int store, double &amount, QString &err);

    int addGoodsRow();

    void setDocEnabled(bool v);

    void loadGroupsInput();

    void loadGoodsInput();

    void loadGoodsOutput();

    void loadGoods(int store);

    void setGoodsPanelHidden(bool v);

private slots:
    void newDoc();

    void getInput();

    void getOutput();

    void saveDoc();

    void draftDoc();

    void removeDocument();

    void printDoc();

    void filterGoodsPanel(int group);

    void showHideGoodsList();

    void tblQtyChanged(const QString &arg1);

    void tblPriceChanged(const QString &arg1);

    void tblTotalChanged(const QString &arg1);

    void on_btnAddGoods_clicked();

    void on_btnRemoveGoods_clicked();

    void on_leStoreInput_textChanged(const QString &arg1);

    void on_leStoreOutput_textChanged(const QString &arg1);

    void on_tblGoodsGroup_itemClicked(QTableWidgetItem *item);

    void on_tblGoodsStore_itemDoubleClicked(QTableWidgetItem *item);

    void on_btnNewPartner_clicked();

    void on_btnNewGoods_clicked();
};

#endif // C5STOREDOC_H
