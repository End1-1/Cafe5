#include "cr5goodsqtyreminder.h"
#include "cr5goodsqtyreminderfilter.h"

CR5GoodsQtyReminder::CR5GoodsQtyReminder(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/goods.png";
    fLabel = tr("Low quantity");
    fSqlQuery = "select s.f_name as f_storename, gr.f_name as f_groupname, g.f_name as f_goodsname, g.f_scancode, "
                "g.f_lowlevel, sum(a.f_qty*a.f_type) as f_qty "
                "from a_store a "
                "left join c_storages s on s.f_id=a.f_store "
                "left join c_goods g on g.f_id=a.f_goods "
                "left join c_groups gr on gr.f_id=g.f_group "
                "where g.f_enabled=1 %where% "
                "group by s.f_name, g.f_name, g.f_lowlevel "
                "having sum(a.f_qty*a.f_type)=0  or sum(a.f_qty*a.f_type)<g.f_lowlevel ";
    fFilterWidget = new CR5GoodsQtyReminderFilter(dbParams);

    fTranslation["f_storename"] = tr("Storage");
    fTranslation["f_groupname"] = tr("Group");
    fTranslation["f_goodsname"] = tr("Goods");
    fTranslation["f_scancode"] = tr("Scancode");
    fTranslation["f_lowlevel"] = tr("Min qty");
    fTranslation["f_qty"] = tr("Qty");
}

QToolBar *CR5GoodsQtyReminder::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbFilter
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
            createStandartToolbar(btn);
    }
    return fToolBar;
}