#include "cr5costumerdebts.h"
#include "cr5costumerdebtsfilter.h"
#include "c5tablemodel.h"
#include "c5costumerdebtpayment.h"
#include "c5salefromstoreorder.h"
#include "c5waiterorder.h"
#include "c5mainwindow.h"
#include "c5storedoc.h"

CR5CostumerDebts::CR5CostumerDebts(QWidget *parent) :
    C5ReportWidget(parent)
{
    fIcon = ":/cash.png";
    fLabel = tr("Customers debts");
    fColumnsSum << "f_amount";
    fColumnsSum << "f_amountmin";
    fColumnsSum << "f_amountplu";
    fColumnsSum << "f_amountret";
    fColumnsSum << "f_amd";
    fColumnsSum << "f_usd";
    fColumnsSum << "f_amountbank";
    fColumnsSum << "f_amountdebt";
    fColumnsSum << "fdebt";
    fColumnsSum << "fbefore";
    fColumnsSum << "frefund";
    fColumnsSum << "fpartnerback";
    fColumnsSum << "fpayment";
    fColumnsSum << "fdiff";
    fTranslation["f_order"] = tr("Sale document");
    fTranslation["f_storedoc"] = tr("Store document");
    fTranslation["f_cash"] = tr("Cash document");
    fTranslation["f_id"] = tr("Row");
    fTranslation["f_cashname"] = tr("Cash");
    fTranslation["f_contact"] = tr("Contact");
    fTranslation["f_date"] = tr("Date");
    fTranslation["f_ordernum"] = tr("Order");
    fTranslation["f_amount"] = tr("Amount");
    fTranslation["fbefore"] = tr("Before");
    fTranslation["frefund"] = tr("Refund");
    fTranslation["f_amountmin"] = tr("Debt");
    fTranslation["f_amountret"] = tr("Return");
    fTranslation["f_amountplu"] = tr("Payment");
    fTranslation["f_amounttot"] = tr("Balance");
    fTranslation["f_govnumber"] = tr("Gov. number");
    fTranslation["fid"] = tr("ID");
    fTranslation["fpartner"] = tr("Partner");
    fTranslation["ftill"] = tr("Till");
    fTranslation["fdebt"] = tr("Debt");
    fTranslation["fpay"] = tr("Payment");
    fTranslation["fremain"] = tr("Remain");
    fTranslation["f_partner"] = tr("Partner");
    fTranslation["f_amd"] = tr("AMD");
    fTranslation["f_usd"] = tr("USD");
    fTranslation["f_currency"] = tr("Currency");
    fTranslation["f_taxname"] = tr("Taxname");
    fTranslation["f_amountbank"] = tr("Bank transfer");
    fTranslation["f_amountdebt"] = tr("Cash debt");
    fTranslation["f_hall"] = tr("Hall");
    fTranslation["fpartnername"] = tr("Partner");
    fTranslation["faddress"] = tr("Address");
    fTranslation["fdebt"] = tr("Debt");
    fTranslation["fpayment"] = tr("Payment");
    fTranslation["fpartnerback"] = tr("Return");
    fTranslation["fdiff"] = tr("Balance");
    restoreColumnsVisibility();
    fFilterWidget = new CR5CostumerDebtsFilter();
    fFilter = static_cast<CR5CostumerDebtsFilter*>(fFilterWidget);
}

QToolBar* CR5CostumerDebts::toolBar()
{
    if(!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbFilter
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        fToolBar = createStandartToolbar(btn);
        fToolBar->addAction(QIcon(":/new.png"), tr("New customer payment"), this, SLOT(newCustomerPayment()));
        fToolBar->addAction(QIcon(":/new.png"), tr("New partner payment"), this, SLOT(newPartnerPayment()));
    }

    return fToolBar;
}

void CR5CostumerDebts::buildQuery()
{
    if(fFilter->debtMode()) {
        fSimpleQuery = true;

        if(fFilter->viewMode() == 2) {
            fSimpleQuery = true;
            fSqlQuery = "CREATE TEMPORARY TABLE debts_total (f_partnerid INTEGER, f_partner TEXT, f_amd DECIMAL(14,2), f_usd DECIMAL(14,2)); "
                        "DELETE FROM debts_total; "
                        "INSERT INTO debts_total (f_partnerid, f_partner, f_amd, f_usd) "
                        "SELECT f_id, f_taxname, 0, 0 FROM c_partners; "
                        "UPDATE debts_total d INNER JOIN (SELECT f_costumer AS f_partner, SUM(f_amount)AS f_amd FROM  b_clients_debts WHERE f_source=2 AND f_currency=1 GROUP BY 1) b ON d.f_partnerid=b.f_partner "
                        "SET d.f_amd=b.f_amd WHERE d.f_partnerid=b.f_partner; "
                        "UPDATE debts_total d INNER JOIN (SELECT f_costumer AS f_partner, SUM(f_amount)AS f_usd FROM  b_clients_debts WHERE f_source=2 AND f_currency=2 GROUP BY 1) b ON d.f_partnerid=b.f_partner "
                        "SET d.f_usd=b.f_usd WHERE d.f_partnerid=b.f_partner; "
                        "DELETE FROM debts_total WHERE f_amd=0 AND f_usd=0; "
                        "SELECT f_partner, f_amd, f_usd FROM debts_total;";
            fModel->translate(fTranslation);
            refreshData();
            fTableView->resizeColumnsToContents();
            fTableView->setColumnWidth(0, 300);
            fTableView->setColumnWidth(1, 100);
            fTableView->setColumnWidth(2, 100);
            emit refreshed();
        }  else if(fFilter->viewMode() == 1) {
            fSqlQuery = "SELECT cd.f_id, cd.f_date, p.f_taxname, cd.f_order, cd.f_cash, "
                        "cd.f_storedoc, cd.f_amount, c.f_name as f_currency "
                        "FROM b_clients_debts cd "
                        "LEFT JOIN c_partners p ON p.f_id=cd.f_costumer "
                        "LEFT JOIN e_currency c ON c.f_id=cd.f_currency ";
            fSqlQuery += fFilter->condition();
            fModel->translate(fTranslation);
            C5ReportWidget::buildQuery();
            fTableView->setColumnWidth(3, 0);
            fTableView->setColumnWidth(4, 0);
            fTableView->setColumnWidth(5, 0);
        } else if(fFilter->viewMode() == 3) {
            queryDebt3();
        }
    } else {
        if(fFilter->viewMode() == 2) {
            fSimpleQuery = true;
            QString flag;

            if(!fFilter->flag().isEmpty()) {
                flag = QString(" and f_flag in (%1) ").arg(fFilter->flag());
            }

            fSqlQuery = "CREATE TEMPORARY TABLE debts_total (f_partnerid INTEGER, f_partner TEXT, f_amd DECIMAL(14,2), f_usd DECIMAL(14,2)); "
                        "DELETE FROM debts_total; "
                        "INSERT INTO debts_total (f_partnerid, f_partner, f_amd, f_usd) "
                        "SELECT f_id, f_taxname, 0, 0 FROM c_partners; "
                        "UPDATE debts_total d INNER JOIN "
                        "(SELECT f_costumer AS f_partner, SUM(f_amount) AS f_amd "
                        "FROM  b_clients_debts WHERE f_source=1 AND f_currency=1 %flag% GROUP BY 1) b ON d.f_partnerid=b.f_partner "
                        "SET d.f_amd=b.f_amd WHERE d.f_partnerid=b.f_partner; "
                        "UPDATE debts_total d INNER JOIN "
                        "(SELECT f_costumer AS f_partner, SUM(f_amount) AS f_usd "
                        "FROM  b_clients_debts WHERE f_source=1 AND f_currency=2 %flag% GROUP BY 1) b ON d.f_partnerid=b.f_partner "
                        "SET d.f_usd=b.f_usd WHERE d.f_partnerid=b.f_partner; "
                        "DELETE FROM debts_total WHERE f_amd=0 AND f_usd=0; ";
            fSqlQuery.replace("%flag%", flag);

            if(!fFilter->manager().isEmpty()) {
                fSqlQuery += QString("delete from debts_total where f_partnerid not in (select f_id from c_partners where f_manager in (%1));").arg(fFilter->manager());
            }

            fSqlQuery += "SELECT f_partner, f_amd, f_usd FROM debts_total;";
            fModel->translate(fTranslation);
            refreshData();
            fTableView->resizeColumnsToContents();
            fTableView->setColumnWidth(0, 300);
            fTableView->setColumnWidth(1, 100);
            fTableView->setColumnWidth(2, 100);
            emit refreshed();
        }  else if(fFilter->viewMode() == 1) {
            fSimpleQuery = true;
            fSqlQuery = "SELECT cd.f_id, cd.f_date, p.f_taxname, cd.f_order, cd.f_cash, cd.f_storedoc, "
                        "cd.f_amount, bm.f_amountmin, bpp.f_amountret, bp.f_amountplu, bt.f_amounttot, "
                        "c.f_name as f_currency, oh.f_amountbank, oh.f_amountdebt, "
                        "h.f_name as f_hall "
                        "FROM b_clients_debts cd "
                        "left join (select cd.f_id, f_amount as f_amountmin from b_clients_debts cd "
                        "left join c_partners p on p.f_id=cd.f_costumer "
                        + fFilter->condition() + " and cd.f_amount<0 "
                        + ") bm on bm.f_id=cd.f_id "
                                     "left join (select cd.f_id, f_amount as f_amountret from b_clients_debts cd "
                                     "left join c_partners p on p.f_id=cd.f_costumer "
                        + fFilter->condition() + " and cd.f_amount>0  "
                                     "and f_storedoc in (select distinct(f_document) from a_store where f_reason=11) "
                        + ") bpp on bpp.f_id=cd.f_id "
                                     "left join (select cd.f_id, f_amount as f_amountplu from b_clients_debts cd "
                                     "left join c_partners p on p.f_id=cd.f_costumer "
                        + fFilter->condition() + " and cd.f_amount>0 "
                                     "and f_storedoc is null "
                        + ") bp on bp.f_id=cd.f_id "
                                     "left join (select f_costumer, sum(f_amount) as f_amounttot from b_clients_debts cd "
                                     "left join c_partners p on p.f_id=cd.f_costumer "
                                     "where 1=1 " + fFilter->sourceCond() + fFilter->flagCond()
                        + " group by 1) bt on bt.f_costumer=cd.f_costumer "
                                     "LEFT JOIN c_partners p ON p.f_id=cd.f_costumer "
                                     "LEFT JOIN e_currency c ON c.f_id=cd.f_currency "
                                     "left join o_header oh on oh.f_id=cd.f_order "
                                     "LEFT join h_halls h on h.f_id=cd.f_flag ";
            fSqlQuery += fFilter->condition();
            fModel->translate(fTranslation);
            C5ReportWidget::buildQuery();
            fTableView->resizeColumnsToContents();
            fTableView->setColumnWidth(3, 0);
            fTableView->setColumnWidth(4, 0);
            fTableView->setColumnWidth(5, 0);
        } else if(fFilter->viewMode() == 3) {
            queryDebt3();
        }
    }
}

void CR5CostumerDebts::restoreColumnsWidths()
{
    C5Grid::restoreColumnsWidths();

    if(!fSimpleQuery) {
        if(fColumnsVisible["cd.f_order"]) {
            fTableView->setColumnWidth(fModel->fColumnNameIndex["f_order"], 0);
        }

        if(fColumnsVisible["cd.f_id"]) {
            fTableView->setColumnWidth(fModel->fColumnNameIndex["f_id"], 0);
        }
    }
}

bool CR5CostumerDebts::tblDoubleClicked(int row, int column, const QJsonArray &vals)
{
    Q_UNUSED(row);
    Q_UNUSED(column);

    if(fFilter->viewMode() != 1) {
        return true;
    }

    if(vals.empty()) {
        return true;
    }

    if(!vals.at(3).toString().isEmpty()) {
        if(!vals.at(3).toString().isEmpty()) {
            C5Database db;
            db[":f_id"] = vals.at(3).toString();
            db.exec("select f_source from o_header where f_id=:f_id");

            if(db.nextRow()) {
                switch(abs(db.getInt(0))) {
                case 1: {
                    C5WaiterOrder *wo = __mainWindow->createTab<C5WaiterOrder>();
                    wo->setOrder(vals.at(3).toString());
                    break;
                }

                case 2: {
                    C5SaleFromStoreOrder::openOrder(vals.at(3).toString());
                    break;
                }
                }
            }

            return true;
        }
    } else if(!vals.at(4).toString().isEmpty()) {
        C5CostumerDebtPayment d(0);
        d.setId(vals.at(4).toString());
        d.exec();
    } else if(!vals.at(5).toString().isEmpty()) {
        C5StoreDoc *sd = __mainWindow->createTab<C5StoreDoc>();
        QString err;
        sd->openDoc(vals.at(5).toString(), err);

        if(!err.isEmpty()) {
            C5Message::error(err);
        }
    }

    return true;
}

void CR5CostumerDebts::newCustomerPayment()
{
    C5CostumerDebtPayment(BCLIENTDEBTS_SOURCE_SALE).exec();
}

void CR5CostumerDebts::newPartnerPayment()
{
    C5CostumerDebtPayment(BCLIENTDEBTS_SOURCE_INPUT).exec();
}

void CR5CostumerDebts::queryDebt3()
{
    QString sql = "CREATE TEMPORARY TABLE debt3 (fpartnerid INTEGER, fpartnername TINYTEXT, faddress TINYTEXT, "
                  "fbefore decimal, fdebt DECIMAL, fpayment DECIMAL, fpartnerback decimal, frefund decimal, fdiff decimal); "
                  "INSERT INTO debt3 (fpartnerid, fpartnername, faddress, fdebt, fpayment, fdiff) "
                  "SELECT f_id, f_address, f_taxname, 0, 0, 0 FROM c_partners %manager%; "
                  "update debt3 d inner join (select f_costumer, sum(f_amount) as famount from b_clients_debts where f_date<'%d1%' %flag% group by 1) "
                  "dm on dm.f_costumer=d.fpartnerid set d.fbefore=dm.famount; "
                  "UPDATE debt3 d INNER JOIN (SELECT f_costumer, SUM(f_amount) AS famount fROM b_clients_debts WHERE f_amount<0 and f_date between '%d1%' and '%d2%' %flag% GROUP BY 1) "
                  "    dm ON dm.f_costumer=d.fpartnerid SET d.fdebt=dm.famount; "
                  //payment
                  "UPDATE debt3 d INNER JOIN (SELECT f_costumer, SUM(f_amount) AS famount "
                  "    fROM b_clients_debts WHERE f_amount>0 and f_storedoc is null and f_date between '%d1%' and '%d2%' %flag% GROUP BY 1) "
                  "    dm ON dm.f_costumer=d.fpartnerid SET d.fpayment=dm.famount; "
                  //income from partner back
                  "UPDATE debt3 d INNER JOIN (SELECT f_costumer, SUM(f_amount) AS famount "
                  "    fROM b_clients_debts WHERE f_amount>0 and f_storedoc in (SELECT distinct(f_document) FROM a_store WHERE f_reason=11) and f_date between '%d1%' and '%d2%' %flag% GROUP BY 1) "
                  "    dm ON dm.f_costumer=d.fpartnerid SET d.fpartnerback=dm.famount; "
                  //refund
                  "UPDATE debt3 d INNER JOIN (SELECT f_costumer, SUM(f_amount) AS famount "
                  "FROM b_clients_debts WHERE f_amount>0 and f_storedoc is not null "
                  "and f_date between '%d1%' and '%d2%' %flag% GROUP BY 1) "
                  "    dm ON dm.f_costumer=d.fpartnerid SET d.frefund=dm.famount; "
                  "UPDATE debt3 d INNER JOIN (SELECT f_costumer, SUM(f_amount) AS famount "
                  "fROM b_clients_debts WHERE f_id>0 %flag% and f_date<='%d2%' GROUP BY 1) "
                  "    dm ON dm.f_costumer=d.fpartnerid SET d.fdiff=dm.famount;	 "
                  //clean unused rows
                  "delete from debt3 where coalesce(fdebt, 0)=0 and coalesce(fpayment, 0)=0 "
                  "and coalesce(fdiff, 0)=0 and coalesce(frefund,0)=0 and coalesce(fbefore,0)=0;"
                  "SELECT * FROM debt3;";
    sql.replace("%d1%", fFilter->date1()).replace("%d2%", fFilter->date2());

    if(fFilter->manager().isEmpty()) {
        sql.replace("%manager%", "");
    } else {
        sql.replace("%manager%", " where f_manager in (" + fFilter->manager() + ") ");
    }

    if(fFilter->flag().isEmpty()) {
        sql.replace("%flag%", "");
    } else {
        sql.replace("%flag%", QString(" and f_flag=%1").arg(fFilter->flag()));
    }

    fSimpleQuery = true;
    fSqlQuery = sql;
    fModel->translate(fTranslation);
    C5ReportWidget::buildQuery();
    fTableView->resizeColumnsToContents();
    fTableView->setColumnWidth(0, 0);
}
