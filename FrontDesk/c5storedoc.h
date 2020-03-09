#ifndef C5STOREDOC_H
#define C5STOREDOC_H

#include "c5widget.h"
#include <QLabel>

namespace Ui {
class C5StoreDoc;
}

class QTableWidgetItem;

class TableCell : public QLabel {
public:
    QTableWidgetItem *fOldItem;

    TableCell(QWidget *parent, QTableWidgetItem *item);
};

class C5StoreDoc : public C5Widget
{
    Q_OBJECT

public:
    enum STORE_DOC {sdInput = 1, sdOutput, sdMovement, sdInventory, sdCash, sdComplectation};

    explicit C5StoreDoc(const QStringList &dbParams, QWidget *parent = nullptr);

    ~C5StoreDoc() override;

    bool openDoc(QString id);

    void setMode(STORE_DOC sd);

    void setLastInputPrices();

    virtual QToolBar *toolBar() override;

    static bool removeDoc(const QStringList &dbParams, QString id, bool showmessage = true);

    bool saveDraft(C5Database &db, int state, QString &err, bool showMsg);

    virtual bool allowChangeDatabase() override;
    
protected:
    virtual bool eventFilter(QObject *o, QEvent *e) override;

    virtual void nextChild() override;

private:
    Ui::C5StoreDoc *ui;

    int fDocType;

    int fDocState;

    bool fCanChangeFocus;

    QString fInternalId;

    QString fCashUuid;

    TableCell *fGroupTableCell;

    QMap<int, double> fBaseQtyOfComplectation;

    bool fGroupTableCellMove;

    QString fComplectationId;

    int fBasedOnSale;

    void setUserID(bool withUpdate, int value);

    void countTotal();

    void countQtyOfComplectation();

    bool docCheck(QString &err);

    void rowsCheck(QString &err);

    bool saveDraft(C5Database &db, int state, QString &err);

    bool insertDraftRow(C5Database &db, int row);

    bool updateDraftRow(C5Database &db, int row);

    void writeInput(C5Database &db);

    bool writeOutput(C5Database &db, const QDate &date, QString docNum, int store, double &amount, QStringList &outId, QString &err);

    int addGoodsRow();

    void setDocEnabled(bool v);

    void loadGroupsInput();

    void loadGoodsInput();

    void loadGoodsOutput();

    void loadGoods(int store);

    void setGoodsPanelHidden(bool v);

    void markGoodsComplited();

    void updateCashDoc();

private slots:
    void newDoc();

    void getInput();

    void getOutput();

    void saveDoc();

    void draftDoc();

    void removeDocument();

    void printDoc();

    void checkInvoiceDuplicate();

    void filterGoodsPanel(int group);

    void showHideGoodsList();

    void changeGoodsGroupOrder();

    void saveGoodsGroupOrder();

    void cancelGoodsGroupOrder();

    void tblQtyChanged(const QString &arg1);

    void tblPriceChanged(const QString &arg1);

    void tblTotalChanged(const QString &arg1);

    void tblAddChanged(const QString &arg1);

    void on_btnAddGoods_clicked();

    void on_btnRemoveGoods_clicked();

    void on_leStoreInput_textChanged(const QString &arg1);

    void on_leStoreOutput_textChanged(const QString &arg1);

    void on_tblGoodsGroup_itemClicked(QTableWidgetItem *item);

    void on_tblGoodsStore_itemDoubleClicked(QTableWidgetItem *item);

    void on_btnNewPartner_clicked();

    void on_btnNewGoods_clicked();

    void on_leScancode_returnPressed();

    void on_tblGoodsGroup_customContextMenuRequested(const QPoint &pos);

    void on_btnCreateDoc_clicked();

    void on_leComplectationName_textChanged(const QString &arg1);

    void on_leComplectationQty_textChanged(const QString &arg1);

    void on_chPaid_clicked(bool checked);

    void on_btnAddAdd_clicked();

    void on_btnRemoveAdd_clicked();
};

#endif // C5STOREDOC_H
