#include "cr5mfgeneralreport.h"
#include "cr5mfgeneralreportfilter.h"
#include "c5tablemodel.h"
#include "xlsxsheet.h"
#include <QApplication>

CR5MFGeneralReport::CR5MFGeneralReport(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/manufacturing.png";
    fLabel = tr("Manufacture general report");
    fFilterWidget = new CR5MFGeneralReportFilter(dbParams);

    fSimpleQuery = false;
    fMainTable = "mf_daily_process p";

    fLeftJoinTables << "left join mf_actions_group pr on pr.f_id=p.f_product [pr]"
                    << "left join mf_actions ac on ac.f_id=p.f_process [ac]"
                    << "left join s_user w on w.f_id=p.f_worker [w]"
                    << "left join s_user_group wg on wg.f_id=w.f_group [wg]";

    fColumnsFields << "wg.f_name as f_positionname"
                    << "concat(w.f_last, ' ', w.f_first) as f_workername"
                    << "p.f_date"
                    << "pr.f_name as f_productname"
                    << "ac.f_name as f_processname"
                    << "sum(p.f_qty) as f_qty"
                    << "p.f_price"
                    << "sum(p.f_qty*p.f_price) as f_total";

    fColumnsGroup << "p.f_date"
                    << "wg.f_name as f_positionname"
                    << "concat(w.f_last, ' ', w.f_first) as f_workername"
                    << "pr.f_name as f_productname"
                    << "ac.f_name as f_processname"
                    << "p.f_qty"
                    << "p.f_price";

    fColumnsSum << "f_qty"
                << "f_total";

    fColumnsOrder << "concat(w.f_last, ' ', w.f_first) as f_workername"
                  << "p.f_date";


    fTranslation["f_date"] = tr("Date");
    fTranslation["f_positionname"] = tr("Position");
    fTranslation["f_workername"] = tr("Worker");
    fTranslation["f_productname"] = tr("Product");
    fTranslation["f_processname"] = tr("Process");
    fTranslation["f_qty"] = tr("Qty");
    fTranslation["f_price"] = tr("Price");
    fTranslation["f_total"] = tr("Total");

    fColumnsVisible["p.f_date"] = true;
    fColumnsVisible["wg.f_name as f_positionname"] = false;
    fColumnsVisible["concat(w.f_last, ' ', w.f_first) as f_workername"] = true;
    fColumnsVisible["pr.f_name as f_productname"] = true;
    fColumnsVisible["ac.f_name as f_processname"] = true;
    fColumnsVisible["sum(p.f_qty) as f_qty"] = true;
    fColumnsVisible["p.f_price"] = true;
    fColumnsVisible["sum(p.f_qty*p.f_price) as f_total"] = true;

    fXlsxFitToPage = 1;
    fXlsxPageOrientation = xls_page_orientation_landscape;
}

QToolBar *CR5MFGeneralReport::toolBar()
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

void CR5MFGeneralReport::completeRefresh()
{
    C5ReportWidget::completeRefresh();

    int subtotalcol1 = fModel->indexForColumnName("f_workername");
    int subtotalcol2 = fModel->indexForColumnName("f_date");
    if (subtotalcol1 > -1 && subtotalcol2 > -1) {
        QList<int> totals;
        QList<int> subtotalcols;
        subtotalcols.append(subtotalcol1);
        subtotalcols.append(subtotalcol2);
        if (fModel->indexForColumnName("f_total") > -1) {
            totals.append(fModel->indexForColumnName("f_total"));
        }
        fModel->insertSubTotals(subtotalcols, totals, false, false);
    }


    subtotalcol1 = fModel->indexForColumnName("f_workername");
    if (subtotalcol1 > -1) {
        QList<int> totals;
        QList<int> subtotalcols;
        subtotalcols.append(subtotalcol1);
        if (fModel->indexForColumnName("f_total") > -1) {
            totals.append(fModel->indexForColumnName("f_total"));
        }
        fModel->insertSubTotals(subtotalcols, totals, false, false);
    }

    QString name;
    int fi = 0;
    for (int i = 0; i < fModel->rowCount(); i++) {
        if (fModel->data(i, 0, Qt::EditRole).toString().isEmpty()) {
            continue;
        }
        if (name.isEmpty()) {
           fi = i;
           name = fModel->data(i, 0, Qt::EditRole).toString();
        }
        if (name != fModel->data(i, 0, Qt::EditRole).toString()) {
            fTableView->setSpan(fi, 0, i - fi - 1, 1);
            name = fModel->data(i, 0, Qt::EditRole).toString();
            fi = i;
        }
    }
    if (!name.isEmpty()) {
        fTableView->setSpan(fi, 0, fModel->rowCount() - fi - 2, 1);
    }

    name.clear();
    fi = 0;
    for (int i = 0; i < fModel->rowCount(); i++) {
        if (i == 0) {
           name = fModel->data(i, 1, Qt::EditRole).toString();
        }
        if (name.isEmpty()) {
            fi = i + 1;
            if (!fModel->data(i + 1, 1, Qt::EditRole).toString().isEmpty()) {
                name = fModel->data(i + 1, 1, Qt::EditRole).toString();
            }
        }
        if (i < fModel->rowCount() - 1) {
            if (!name.isEmpty() && name != fModel->data(i + 1, 1, Qt::EditRole).toString()) {
                qDebug() << name << fModel->data(i + 1, 1, Qt::EditRole).toString();
                fTableView->setSpan(fi, 1, i - fi + 1, 1);
                fi = i + 1;
                name.clear();
            }
        }
    }
    if (fi > 0) {
        fTableView->setSpan(fi, 1, fModel->rowCount() - 1 - fi - 1, 1);
    }

    for (int i = fModel->rowCount() - 2; i > 0; i--) {
        if (fModel->data(i, 0, Qt::EditRole).toString().isEmpty()
                && fModel->data(i - 1, 1, Qt::EditRole).toString().isEmpty()) {
            fModel->insertRow(i);
        }
    }

    QFont f(fTableView->font());
    f.setBold(true);
    for (int i = 0; i < fModel->rowCount(); i++) {
        if (!fModel->data(i, 0, Qt::EditRole).toString().isEmpty()) {
            name = fModel->data(i, 0, Qt::EditRole).toString();
        }
        if (fModel->data(i, 0, Qt::EditRole).toString().isEmpty()) {
            fTableView->setSpan(i, 0, 1, 6);
        }
        if (fModel->data(i, 1, Qt::EditRole).toString().isEmpty()) {
            fTableView->setSpan(i, 1, 1, 5);
        }
        if (fModel->data(i, 0, Qt::EditRole).toString().isEmpty()
                && fModel->data(i, 6, Qt::EditRole).toString().isEmpty()) {
            fTableView->setSpan(i, 0, 1, 7);
        }
        if (fModel->data(i, 0, Qt::EditRole).toString().isEmpty()) {
            fModel->setData(i, 6, f, Qt::FontRole);
            if (i > 0) {
                if (fModel->data(i - 1, 0, Qt::EditRole).toString().isEmpty()) {
                    fModel->setData(i, 0, QString("%1 %2").arg(tr("Total")).arg(name));
                    fModel->setData(i, 0, f, Qt::FontRole);
                }
            }
        }
    }
}
