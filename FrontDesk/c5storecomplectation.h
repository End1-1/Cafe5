#pragma once

#include "c5widget.h"
#include "struct_doc_store_input.h"

namespace Ui
{
class C5StoreComplectation;
}

class C5StoreComplectation : public C5Widget
{
    Q_OBJECT

public:
    explicit C5StoreComplectation(C5User *user, const QString &title, QIcon icon, QWidget *parent = nullptr);
    ~C5StoreComplectation() override;

    void setDocument(const QJsonObject &docJo);
    virtual QToolBar *toolBar() override;

private:
    enum Columns {
        col_goods_id = 0,
        col_goods_name,
        col_base_qty,
        col_goods_qty,
        col_goods_unit
    };

    Ui::C5StoreComplectation *ui;
    StoreInputDocument mDocData;
    QString mComplectRowId;
    int mComplectGoodsId = 0;
    QAction *mActionSave = nullptr;
    QAction *mActionDraft = nullptr;

    bool buildDoc(QJsonObject &outDoc);
    void setState();
    void countTotal();
    void scaleMaterials();
    void loadRecipe(int complectId);
    int addMaterial(int goodsId, const QString &name, double baseQty, double qty, const QString &unit);
    void applySaveResult(const QJsonObject &jo, int status);

private slots:
    void saveDocument();
    void draftDocument();
    void removeDocument();
    void tblQtyChanged(const QString &arg1);
    void on_btnSelectComplect_clicked();
    void on_btnAddGoods_clicked();
    void on_btnRemoveGoods_clicked();
    void on_leComplectationQty_textEdited(const QString &arg1);
};
