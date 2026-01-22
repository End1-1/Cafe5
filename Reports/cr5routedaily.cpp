#include "cr5routedaily.h"
#include "cr5routedailyfilter.h"
#include "c5tablemodel.h"
#include "c5saledoc.h"
#include "c5mainwindow.h"

CR5RouteDaily::CR5RouteDaily(QWidget *parent) :
    C5ReportWidget( parent)
{
    fLabel = tr("Edit route");
    fIconName = ":/route.png";
    fFilterWidget = new CR5RouteDailyFilter();
    fSimpleQuery = true;
    fSqlQuery = "SELECT ro.f_id, p.f_address, p.f_taxname, ro.f_date, "
                "CONCAT_WS(' ', d.f_last, d.f_first) AS f_driver, ro.f_action_comment, ro.f_action_datetime, "
                "f_saleuuid "
                "FROM o_route_exec ro "
                "LEFT JOIN c_partners p ON p.f_id=ro.f_partner "
                "LEFT JOIN s_user d ON d.f_id=ro.f_driver ";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_address"] = tr("Address");
    fTranslation["f_taxname"] = tr("Tax name");
    fTranslation["f_date"] = tr("Date");
    fTranslation["f_driver"] = tr("Driver");
    fTranslation["f_action_comment"] = tr("Comment");
    fTranslation["f_action_datetime"] = tr("Action on");
}

QToolBar *CR5RouteDaily::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbNew;
        btn << ToolBarButtons::tbRefresh;
        btn << ToolBarButtons::tbFilter;
        btn << ToolBarButtons::tbClearFilter;
        btn << ToolBarButtons::tbExcel;
        fToolBar = createStandartToolbar(btn);
    }
    return fToolBar;
}

void CR5RouteDaily::completeRefresh()
{
    C5ReportWidget::completeRefresh();
    fTableView->setColumnWidth(7, 0);
}

bool CR5RouteDaily::on_tblView_doubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return true;
    }
    if (!fModel->data(index.row(), 7, Qt::EditRole).toString().isEmpty()) {
        auto *s = __mainWindow->createTab<C5SaleDoc>();
        s->openDoc(fModel->data(index.row(), 7, Qt::EditRole).toString());
    }
    return true;
}
