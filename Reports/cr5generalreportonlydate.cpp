#include "cr5generalreportonlydate.h"
#include "xlsxsheet.h"
#include "cr5mfgeneralreportfilter.h"
#include "c5tablemodel.h"

CR5GeneralReportOnlyDate::CR5GeneralReportOnlyDate(const QStringList &dbParams) :
    C5ReportWidget(dbParams)
{
    fIcon = ":/manufacturing.png";
    fLabel = tr("Manufacture general report only date");
    fFilterWidget = new CR5MFGeneralReportFilter(dbParams);
    fFilter = static_cast<CR5MFGeneralReportFilter*>(fFilterWidget);
    fTranslation["f_date"] = tr("Date");
    fTranslation["f_workername"] = tr("Worker");
    fTranslation["f_counted"] = tr("Counted");
    fTranslation["f_days"] = tr("Days");
    fTranslation["f_average"] = tr("Average");

    fSimpleQuery = true;
}

void CR5GeneralReportOnlyDate::buildQuery()
{
    fSqlQuery = "select concat(w.f_last, ' ', w.f_first) as f_workername, date_format(p.f_date, '%d/%m/%Y') as f_date, -1 as f_counted, -1 as f_days, -1 as f_average "
                "from mf_daily_process p "
                "inner join s_user w on w.f_id=p.f_worker "
                "where p.f_product=34 and %where% "
                "union "
                "select concat(w.f_last, ' ', w.f_first), '', floor(sum(p.f_qty*p.f_price)), "
                "count(distinct(p.f_date)), floor(sum(p.f_qty*p.f_price) / count(distinct(p.f_date))) "
                "from mf_daily_process p "
                "inner join s_user w on w.f_id=p.f_worker "
                "where p.f_product<>34 and %where% "
                "group by 1 "
                "order by 1, 2 desc ";
    C5ReportWidget::buildQuery();
}

QToolBar *CR5GeneralReportOnlyDate::toolBar()
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

void CR5GeneralReportOnlyDate::completeRefresh()
{
    QString name;
    for (int i = fModel->rowCount() - 1; i > -1; i--) {
        if (fModel->data(i, 2, Qt::EditRole).toInt() == -1) {
            fModel->setData(i, 2, tr("Absent"));
        }
        if (fModel->data(i, 3, Qt::EditRole).toInt() == -1) {
            fModel->setData(i, 3, "");
            fModel->setData(i, 4, "");
        }
        if (name.isEmpty()) {
            name = fModel->data(i, 0, Qt::EditRole).toString();
        }
        if (i == fModel->rowCount() - 1) {
            continue;
        }
        if (name != fModel->data(i, 0, Qt::EditRole).toString()) {
            name = QString("%1 %2 - %3").arg(name, fFilter->d1s(), fFilter->d2s());
            fModel->insertRow(i);
            fModel->setData(i + 1, 0, name);
            fModel->setData(i + 1, 2, -1);
            name = fModel->data(i, 0, Qt::EditRole).toString();
        }
    }
    if (!name.isEmpty()) {
        name = QString("%1 %2 - %3").arg(name, fFilter->d1s(), fFilter->d2s());
        fModel->insertRow(-1);
        fModel->setData(0, 0, name);
        fModel->setData(0, 2, -1);
        fTableView->setSpan(0, 0, 1, 3);
    }
    for (int i = fModel->rowCount() - 1; i > -1; i--) {
        if (fModel->data(i, 2, Qt::EditRole).toInt() == -1) {
            fTableView->setSpan(i, 0, 1, 3);
        }
    }
    double total = 0;
    for (int i = fModel->rowCount() - 1; i > 0; i--) {
        if (fTableView->columnSpan(i, 0) > 1) {
            fModel->insertRow(i - 1);
            fTableView->setSpan(i, 0, 1, 3);
        }
        total += fModel->data(i, 2, Qt::EditRole).toDouble();
    }
    fModel->insertRow(fModel->rowCount() - 1);
    fModel->setData(fModel->rowCount() - 1, 0, tr("Total"));
    fModel->setData(fModel->rowCount() - 1, 2, total);
}
