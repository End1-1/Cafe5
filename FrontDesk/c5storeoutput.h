#pragma once

#include <QLabel>
#include "c5widget.h"
#include "office_structs.h"
#include "struct_doc_store_input.h"

namespace Ui
{
class C5StoreOutput;
}

class QTableWidgetItem;
class C5TableWidget;
class C5LineEdit;

class C5StoreOutput : public C5Widget
{
    Q_OBJECT

public:
    explicit C5StoreOutput(C5User *user, const QString &title, QIcon icon, QWidget *parent = nullptr);

    ~C5StoreOutput() override;

    void setDocument(StoreInputDocument doc);

    virtual QToolBar* toolBar() override;

    static bool removeDoc(QString id, bool showmessage = true);

    virtual bool allowChangeDatabase() override;

    void addByScancode(const QString &code, const QString &qty, QString price);

    double total();

    void setStore(int id, const QString &name);

    virtual void hotKey(const QString &key) override;

    bool openDraft(const QString &id, QString &err);

    int addGoods(int goods, const QString &name, double qty, const QString &unit, double price, double total,
                 const QString &comment, const QString &adgt);

    void fillFromInventory(const QList<InventoryDiff> &surpluses);

protected:

    virtual void nextChild() override;

private:
    enum Columns {
        col_rec_in_id = 0,
        col_goods_id,   // будет 1
        col_goods_name, // будет 2
        col_adgt,       // будет 3
        col_goods_qty,  // будет 4
        col_goods_unit, // будет 5
        col_price,      // будет 6
        col_total,      // будет 7
        col_valid_date, // будет 8
        col_comment,    // будет 9
        col_remain      // будет 10
    };

    Ui::C5StoreOutput *ui;

    StoreInputDocument mDocData;

    bool fCanChangeFocus;

    QAction *mActionSave;

    QAction *mActionDraft;

    bool buildDoc();

    void setState();

    void correctDishesRows(int row, int count);

    void countTotal();

    bool docCheck(QString &err, int state);

    void rowsCheck(QString &err);

    int addGoodsRow();

    void setDocEnabled(bool v);

    QString makeGoodsTableHtml(const QStringList &headers, const QList<QStringList>& rows, const QSet<int>& rightCols);

    QString makeOtherChargesHtml(C5TableWidget *tbl, const QStringList &hdr);

    QString makeComplectationInputHtml(const C5LineEdit *code,
                                       const C5LineEdit *name,
                                       const C5LineEdit *qty,
                                       double total,
                                       double qtyVal,
                                       const QStringList &hdr);
private slots:
    void saveDocument();

    void draftDocument();

    void focusNextChildren();

    void changeCurrencyResponse(const QJsonObject &jdoc);

    void slotCheckQtyResponse(const QJsonObject &jdoc);

    void getInput();

    void removeDocument();

    void tblAddChanged(const QString &arg1);

    void tblQtyChanged(const QString &arg1);

    void tblPriceChanged(const QString &arg1);

    void tblTotalChanged(const QString &arg1);

    void on_btnAddGoods_clicked();

    void on_btnNewGoods_clicked();

    void on_leScancode_returnPressed();

    void on_btnEditGoods_clicked();

    void on_btnCalculator_clicked();

    void on_btnRememberStoreIn_clicked(bool checked);

    void on_btnCopyUUID_clicked();

    void on_leSearchInDoc_textChanged(const QString &arg1);

    void on_btnCloseSearch_clicked();

    void on_btnCopyLastAdd_clicked();

    void on_btnSaveComment_clicked();
	
    void on_btnRemoveGoods_clicked();
};
