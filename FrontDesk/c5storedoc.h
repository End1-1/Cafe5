#ifndef C5STOREDOC_H
#define C5STOREDOC_H

#include "c5document.h"
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

class C5StoreDoc : public C5Document
{
    Q_OBJECT

public:
    enum STORE_DOC {sdInput = 1, sdOutput, sdMovement, sdInventory, sdCash, sdComplectation, sdDeComplectation};

    explicit C5StoreDoc(const QStringList &dbParams, QWidget *parent = nullptr);

    ~C5StoreDoc() override;

    bool openDoc(QString id, QString &err);

    void setMode(STORE_DOC sd);

    void setLastInputPrices();

    void setStore(int store1, int store2);

    void setReason(int reason);

    void setComment(const QString comment);

    void setFlag(const QString &name, const QVariant &value);

    virtual QToolBar *toolBar() override;

    bool writeDocument(int state, QString &err);

    static bool removeDoc(const QStringList &dbParams, QString id, bool showmessage = true);

    virtual bool allowChangeDatabase() override;

    void addByScancode(const QString &code, const QString &qty, QString price);

    double total();

public slots:
    void saveDoc();
    
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

    QString fCashRowId;

    QString fCashUserId;

    TableCell *fGroupTableCell;

    QMap<QString, QVariant> fFlags;

    QMap<int, double> fBaseQtyOfComplectation;

    bool fGroupTableCellMove;

    QString fComplectationId;

    int fBasedOnSale;

    void setUserId(bool withUpdate, int value);

    void correctDishesRows(int row, int count);

    void countTotal();

    void countQtyOfComplectation();

    bool docCheck(QString &err);

    void rowsCheck(QString &err);

    void writeDocumentWithState(int state);

    int addGoodsRow();

    void addGoods(int goods, const QString &name, double qty, const QString &unit, double price, double total, const QString &comment);

    void setDocEnabled(bool v);

    void loadGroupsInput();

    void loadGoodsInput();

    void loadGoodsOutput();

    void loadGoods(int store);

    void setGoodsPanelHidden(bool v);

    void markGoodsComplited();

    void updateCashDoc();

private slots:
    void lineEditKeyPressed(const QChar &key);

    void focusNextChildren();

    void newDoc();

    void getInput();

    void getOutput();

    void draftDoc();

    void removeDocument();

    void createStoreInput();

    void printDoc();

    void printBarcode();

    void checkInvoiceDuplicate();

    void filterGoodsPanel(int group);

    void showHideGoodsList();

    void changeGoodsGroupOrder();

    void saveGoodsGroupOrder();

    void cancelGoodsGroupOrder();

    void tblDishQtyChanged(const QString &arg1);

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

    void on_leComplectationName_textChanged(const QString &arg1);

    void on_leComplectationQty_textChanged(const QString &arg1);

    void on_chPaid_clicked(bool checked);

    void on_btnAddAdd_clicked();

    void on_btnRemoveAdd_clicked();

    void on_btnInsertLast_clicked();

    void on_btnEditGoods_clicked();

    void on_btnCalculator_clicked();

    void inputOfService();

    void outputOfService();

    void on_btnAddDish_clicked();

    void on_btnRemoveRows_clicked();

    void on_btnRememberStoreIn_clicked(bool checked);
};

#endif // C5STOREDOC_H
