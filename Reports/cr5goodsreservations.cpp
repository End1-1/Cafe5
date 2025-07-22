#include "cr5goodsreservations.h"
#include "cr5goodsreservationsfilter.h"
#include "dlgreservgoods.h"

CR5GoodsReservations::CR5GoodsReservations(QWidget *parent) :
    C5ReportWidget(parent)
{
    fIcon = ":/calendar.png";
    fLabel = tr("Goods reservation");
    setProperty("sql", "select rs.f_id, arn.f_name as f_statename, rs.f_date, "
                "ss.f_name as f_sourcestore, sd.f_name as f_storename, "
                "g.f_name as f_goodsname, g.f_scancode, "
                "rs.f_qty, rs.f_message "
                "from a_store_reserve rs "
                "left join c_storages ss on ss.f_id=rs.f_source "
                "left join c_storages sd on sd.f_id=rs.f_store "
                "left join c_goods g on g.f_id=rs.f_goods "
                "left join a_store_reserve_state arn on arn.f_id=rs.f_state "
                "%where% "
                "order by rs.f_date ");
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_statename"] = tr("State");
    fTranslation["f_date"] = tr("Date");
    fTranslation["f_sourcestore"] = tr("Source");
    fTranslation["f_storename"] = tr("Store");
    fTranslation["f_goodsname"] = tr("Goods");
    fTranslation["f_scancode"] = tr("Scancode");
    fTranslation["f_qty"] = tr("Qty");
    fTranslation["f_message"] = tr("Message");
    fFilterWidget = new CR5GoodsReservationsFilter();
}

void CR5GoodsReservations::buildQuery()
{
    CR5GoodsReservationsFilter *f = filter<CR5GoodsReservationsFilter>();
    fSqlQuery = property("sql").toString();
    fSqlQuery.replace("%where%", f->condition());
    C5ReportWidget::buildQuery();
}

QToolBar* CR5GoodsReservations::toolBar()
{
    if(!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbNew
            << ToolBarButtons::tbFilter
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        createStandartToolbar(btn);
    }

    return fToolBar;
}

int CR5GoodsReservations::newRow()
{
    DlgReservGoods d;
    d.exec();
    return -1;
}

bool CR5GoodsReservations::tblDoubleClicked(int row, int column, const QJsonArray &values)
{
    Q_UNUSED(column);

    if(row < 0 || values.count() == 0) {
        return true;
    }

    DlgReservGoods d(values.at(0).toInt());
    d.exec();
    return true;
}
