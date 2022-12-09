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
    enum STORE_DOC {sdInput = 1, sdOutput, sdMovement, sdInventory, sdCash, sdComplectation, sdSalary, sdDeComplectation};

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

    QVariant docProperty(const QString &field) const;

    virtual bool allowChangeDatabase() override;

    void addByScancode(const QString &code, const QString &qty, QString price);

    double total();

    virtual void hotKey(const QString &key);

    void setListenBroadcast(bool v);

    static void writeAStoreSale(int storei, int storeo);

    virtual bool parseBroadcastMessage(int what, const QString &msg, QString &replystr) override;

    bool parseBroadcastSuggest(QJsonObject jobj, QString &replystr);

    bool parseBroadcastAddGoods(QJsonObject jobj, QString &replystr);

    bool parseBroadcastQty(QJsonObject jobj, QString &replystr);

    bool parseBroadcastPrice(QJsonObject jobj, QString &replystr);

    bool parseBroadcastAmount(QJsonObject jobj, QString &replystr);

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

    QString fCashDocUuid;

    TableCell *fGroupTableCell;

    QMap<QString, QVariant> fFlags;

    QMap<int, double> fBaseQtyOfComplectation;

    double fBaseComplectOutput;

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

    int addGoods(int goods, const QString &name, double qty, const QString &unit, double price, double total, const QString &comment);

    void setDocEnabled(bool v);

    void loadGroupsInput();

    void loadGoodsInput();

    void loadGoodsOutput();

    void loadGoods(int store);

    void setGoodsPanelHidden(bool v);

    void markGoodsComplited();

    void addGoodsByCalculation(int goods, const QString &name, double qty);

    void printV1();

    void printV2();

    double additionalCost();

    double additionalCostForEveryGoods();

    void calcPrice2(int row);

    void calcTotalSale();

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

    void tblCalcMarginPercentChanged(const QString &arg1);

    void tblCalcMarginChanged(const QString &arg1);

    void tblCalcFinalChanged(const QString &arg1);

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

    void duplicateOutput();

    void on_btnAddDish_clicked();

    void on_btnRemoveRows_clicked();

    void on_btnRememberStoreIn_clicked(bool checked);

    void on_btnCopyUUID_clicked();

    void on_leSearchInDoc_textChanged(const QString &arg1);

    void on_btnCloseSearch_clicked();

    void on_btnFillRemote_clicked(bool checked);

    void on_btnAddPackages_clicked();

    void on_btnSetMargin_clicked();

    void on_btnSetAllMarginPercent_clicked();

    void on_leTotalSale_textChanged(const QString &arg1);

    void on_btnAutoFillSalePrice_clicked();
};

#endif // C5STOREDOC_H
