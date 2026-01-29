#pragma once

#include "c5widget.h"
#include "struct_goods_item.h"

namespace Ui
{
class C5StoreDecompilation;
}

class C5StoreDecompilation : public C5Widget
{
    Q_OBJECT
public:
    explicit C5StoreDecompilation(QWidget *parent = nullptr);

    ~C5StoreDecompilation();

    virtual QToolBar* toolBar() override;

private slots:
    void rowTextEdited(const QString &text);

    void saveDoc();

    void bMessageReceived(const QJsonObject &jo);

    void on_btnAddComplect_clicked();

    void on_leBarcode_returnPressed();

    void on_btnFillComplect_clicked();

    void on_btnRemove_clicked();

private:
    Ui::C5StoreDecompilation* ui;

    QString mLastQuery;

    QString mInternalId;

    int addGoods1Row(GoodsItem g);

};
