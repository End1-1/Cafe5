#include "cr5materialmoveuncomplect.h"
#include "cr5materialmoveuncomplectfilter.h"
#include "c5tablemodel.h"

CR5MaterialMoveUncomplect::CR5MaterialMoveUncomplect(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/goods.png";
    fLabel = tr("Storage movement, uncomplect");
    fSimpleQuery = true;
    fFilter = new CR5MaterialmoveUncomplectFilter(dbParams);
    fFilterWidget = fFilter;

    fTranslation["f_doctype"] = tr("Document type");
    fTranslation["f_action"] = tr("Action");
    fTranslation["f_code"] = tr("Code");
    fTranslation["f_storage"] = tr("Storage");
    fTranslation["f_group"] = tr("Group");
    fTranslation["f_goods"] = tr("Goods");
    fTranslation["f_scancode"] = tr("Scancode");
    fTranslation["f_qty"] = tr("Qty");
    fTranslation["f_unit"] = tr("Unit");
    fTranslation["f_total"] = tr("Total");
    fTranslation["f_totalsale"] = tr("Total retail");
    fTranslation["f_totalsale2"] = tr("Total whosale");

    fColumnsSum.append("f_qty");
    fColumnsSum.append("f_total");
    fColumnsSum.append("f_totalsale");
    fColumnsSum.append("f_totalsale2");
}

QToolBar *CR5MaterialMoveUncomplect::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbFilter
        << ToolBarButtons::tbClearFilter
        << ToolBarButtons::tbRefresh
        << ToolBarButtons::tbExcel
        << ToolBarButtons::tbPrint;
        fToolBar = createStandartToolbar(btn);
    }
    return fToolBar;
}

void CR5MaterialMoveUncomplect::buildQuery()
{
    if (fFilter->unit().isEmpty()) {
        C5Message::error(tr("Unit of complected goods must be defined"));
        return;
    }
    QString fields;
    switch (fFilter->viewMode()) {
    case 0:
        fields = "f_doctype, f_action, f_code, f_storage, f_group, f_goods, f_scancode, f_qty, f_unit,  f_total, f_totalsale, f_totalsale2 ";
        break;
    case 1:
        fields = "f_doctype, f_action, f_storage, f_group, f_qty, f_total, f_totalsale, f_totalsale2 ";
        break;
    }


    fSqlQuery = QString("select %3 from ( "

    "select 'A', dt.f_name as f_doctype, st.f_name as f_action, g.f_id as f_code,ss.f_name as f_storage,gg.f_name as f_group,g.f_name as f_goods, "
    "g.f_scancode,s.f_qty as f_qty, "
    "u.f_name as f_unit, s.f_total as f_total, s.f_qty*gpr.f_price1 as f_totalsale, "
    "s.f_qty*gpr.f_price2 as f_totalsale2  "
    "from a_store s "
    "inner join c_goods g on g.f_id=s.f_goods "
    "inner join c_storages ss on ss.f_id=s.f_store "
    "inner join c_groups gg on gg.f_id=g.f_group "
    "inner join c_units u on u.f_id=g.f_unit and u.f_id not in(%2) "
    "inner join a_header h on h.f_id=s.f_document  "
    "left join a_type_sign st on st.f_id=s.f_type "
    "left join a_type dt on dt.f_id=h.f_type "
    "left join c_goods_prices gpr on gpr.f_goods=g.f_id "
    " %1  "

    "union all "

    "select 'B', dt.f_name as f_doctype, st.f_name as f_action, gc.f_goods as f_code,ss.f_name as f_storage,gg.f_name as f_group,g.f_name as f_goods, "
    "g.f_scancode,s.f_qty*gc.f_qty as f_qty, "
    //"u.f_name as f_unit,s.f_qty*gc.f_qty*g.f_lastinputprice as f_total,s.f_qty*gc.f_qty*gpr.f_price1 as f_totalsale, "
    "u.f_name as f_unit,s.f_qty*gc.f_qty*s.f_price as f_total,s.f_qty*gc.f_qty*gpr.f_price1 as f_totalsale, "
    "s.f_qty*gc.f_qty*gpr.f_price2 as f_totalsale2  "
    "from a_store s  "
    "inner join c_goods_complectation gc on gc.f_base=s.f_goods "
    "inner join c_goods g on g.f_id=gc.f_goods "
    "inner join c_goods gu on gu.f_id=s.f_goods "
    "inner join c_storages ss on ss.f_id=s.f_store "
    "inner join c_groups gg on gg.f_id=g.f_group  "
    "inner join c_units u on u.f_id=g.f_unit "
    "inner join a_header h on h.f_id=s.f_document  "
    "left join a_type_sign st on st.f_id=s.f_type "
    "left join a_type dt on dt.f_id=h.f_type "
    "left join c_goods_prices gpr on gpr.f_goods=g.f_id "
    " %1 and gu.f_unit in(%2)  "

    ") k "
    )
            .arg(fFilter->cond())
            .arg(fFilter->unit())
            .arg(fields);
    fModel->translate(fTranslation);
    refreshData();
    emit refreshed();
}
