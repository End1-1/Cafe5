#include "cr5goodsgroup.h"
#include "ce5goodsgroup.h"
#include "c5cache.h"
#include "c5codenameselectorfunctions.h"
#include "c5message.h"
#include "c5officewidget.h"
#include "c5tablemodel.h"
#include "c5user.h"
#include "ninterface.h"
#include "struct_goods_group.h"

namespace {

constexpr int kColGoodsQty = 7;

} // namespace

CR5GoodsGroup::CR5GoodsGroup(QWidget *parent) :
    C5ReportWidget(parent)
{
    fIconName = ":/goods.png";
    fLabel = tr("Group of goods");
    fSqlQuery = R"(
    select gr.f_id, grp.f_name as f_parent, gr.f_name, gr.f_class, gr.f_taxdept, gr.f_adgcode, gr.f_chargevalue,
    g.qnt, gr.f_order
    from c_groups gr
    left join c_groups grp on grp.f_id=gr.f_parent
    left join (select g.f_group, count(g.f_id) as qnt from c_goods g group by 1) g on g.f_group = gr.f_id
    )";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_parent"] = tr("Parent");
    fTranslation["f_name"] = tr("Name");
    fTranslation["f_class"] = tr("Class");
    fTranslation["f_taxdept"] = tr("Tax dept");
    fTranslation["f_adgcode"] = tr("ADG code");
    fTranslation["f_chargevalue"] = tr("Charge value");
    fTranslation["f_color"] = tr("Color");
    fTranslation["qnt"] = tr("Qty");
    fTranslation["f_order"] = tr("Queue");
    fEditor = new CE5GoodsGroup();
}

QToolBar* CR5GoodsGroup::toolBar()
{
    if(!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbNew
            << ToolBarButtons::tbDelete
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        createStandartToolbar(btn);
    }

    return fToolBar;
}

void CR5GoodsGroup::removeWithId(int id, int row)
{
    if(id <= 0 || !fModel || row < 0 || row >= fModel->rowCount()) {
        return;
    }

    if(C5Message::question(tr("Delete selected goods group?")) != QDialog::Accepted) {
        return;
    }

    const int goodsQty = fModel->data(row, kColGoodsQty, Qt::EditRole).toInt();
    int moveTo = 0;

    if(goodsQty > 0) {
        if(C5Message::question(tr("Group contains %1 goods. Move them to another group and delete?").arg(goodsQty)) !=
                QDialog::Accepted) {
            return;
        }

        const auto groups = selectItem<GoodsGroupItem>(false, false);
        if(groups.isEmpty()) {
            return;
        }

        moveTo = groups.first().id;
        if(moveTo <= 0) {
            return;
        }

        if(moveTo == id) {
            C5Message::error(tr("Select another group"));
            return;
        }
    }

    QJsonObject params{
        {QStringLiteral("editor"), QStringLiteral("form_groups_of_goods")},
        {QStringLiteral("f_id"), id},
    };
    if(moveTo > 0) {
        params.insert(QStringLiteral("move_to"), moveTo);
    }

    NInterface::query(QStringLiteral("/engine/v2/officen/editors/remove"),
                      C5OfficeWidget::mUser->mSessionKey,
                      this,
                      params,
                      [this, row](const QJsonObject &) {
                          fModel->removeRow(row);
                          C5Cache::cache(cache_goods_group)->refresh();
                          C5Message::info(tr("Deleted"));
                      },
                      [](const QJsonObject &) { return false; },
                      true,
                      5000,
                      false);
}
