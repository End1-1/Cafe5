#include "cr5debtstopartner.h"
#include "c5mainwindow.h"
#include "cr5documents.h"
#include "cr5documentsfilter.h"
#include "c5cashdoc.h"
#include "c5tablemodel.h"
#include "cr5debtstopartnerfilter.h"

CR5DebtsToPartner::CR5DebtsToPartner(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/contract.png";
    fLabel = tr("Debts to partners");
    fSimpleQuery = true;
    setProperty("query", "select p.f_id, p.f_taxname, b1.f_amount as f_initial, db.f_amount as f_db, cr.f_amount as f_cr,  "
                "coalesce(b1.f_amount, 0) + coalesce(db.f_amount, 0) - coalesce(cr.f_amount, 0) as f_final "
                "from c_partners p "
                "left join (select h.f_partner, sum(dc.f_dc*dc.f_amount) as f_amount from a_dc dc inner join a_header h on h.f_id=dc.f_doc where h.f_date<'%d1' and h.f_state=1 group by 1) b1 on b1.f_partner=p.f_id "
                "left join (select h.f_partner, sum(dc.f_amount) as f_amount from a_dc dc inner join a_header h on h.f_id=dc.f_doc where dc.f_dc=1 and h.f_state=1 and h.f_date between '%d1' and '%d2' group by 1) db on db.f_partner=p.f_id "
                "left join (select h.f_partner, sum(dc.f_amount) as f_amount from a_dc dc inner join a_header h on h.f_id=dc.f_doc where dc.f_dc=-1 and h.f_state=1 and h.f_date between '%d1' and '%d2' group by 1) cr on cr.f_partner=p.f_id ");
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_taxname"] = tr("Partner");
    fTranslation["f_initial"] = tr("Initial");
    fTranslation["f_db"] = tr("Debit");
    fTranslation["f_cr"] = tr("Credit");
    fTranslation["f_final"] = tr("Remains");
    fColumnsSum << "f_initial" << "f_db" << "f_cr" << "f_final";
    fFilterWidget = new CR5DebtsToPartnerFilter(dbParams);
}

QToolBar *CR5DebtsToPartner::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
            btn << ToolBarButtons::tbFilter
                << ToolBarButtons::tbClearFilter
                << ToolBarButtons::tbRefresh
                << ToolBarButtons::tbExcel
                << ToolBarButtons::tbPrint;
            createStandartToolbar(btn);
            fToolBar->addAction(QIcon(":/bill.png"), tr("Pay"), this, SLOT(pay()));
    }
    return fToolBar;
}

void CR5DebtsToPartner::buildQuery()
{
    auto *f = static_cast<CR5DebtsToPartnerFilter*>(fFilterWidget);
    fSqlQuery = property("query").toString()
            .replace("%d1", f->d1().toString(FORMAT_DATE_TO_STR_MYSQL))
            .replace("%d2", f->d2().toString(FORMAT_DATE_TO_STR_MYSQL));
    C5ReportWidget::buildQuery();
}

void CR5DebtsToPartner::refreshData()
{
    C5ReportWidget::refreshData();
    for (int i = fModel->rowCount() - 1; i > -1; i--) {
        if (fModel->data(i, 2, Qt::EditRole).toDouble() == 0
                && fModel->data(i, 3, Qt::EditRole).toDouble() == 0
                && fModel->data(i, 4, Qt::EditRole).toDouble() == 0
                && fModel->data(i, 5, Qt::EditRole).toDouble() == 0) {
            fModel->removeRow(i);
        }
    }
    fModel->resetProxyData();
}

bool CR5DebtsToPartner::tblDoubleClicked(int row, int column, const QList<QVariant> &vals)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    if (vals.count() == 0) {
        return true;
    }
    if (column == 3 || column == 4) {
        auto *fc = static_cast<CR5DebtsToPartnerFilter*>(fFilterWidget);
        auto *d = __mainWindow->createTab<CR5Documents>(fDBParams);
        auto *f = d->filter<CR5DebtsToPartnerFilter>();
//        f->setFilterValue("supplier", vals.at(0));
//        f->setFilterValue("date1", fc->d1());
//        f->setFilterValue("date2", fc->d2());
        int dc = 0;
        switch (column) {
        case 3:
            dc = 1;
            break;
        case 4:
            dc = -1;
            break;
        }
        QString sql = QString("select distinct(h.f_id) as f_id,h.f_userid as f_docnum, "
                              "ds.f_name as f_statename,dt.f_name as f_typename,h.f_date,p.f_taxname, "
                              "h.f_amount,h.f_comment,hs.f_invoice,u.f_login as f_operatorname, "
                              "h.f_createdate,h.f_createtime "
                              "from a_header h "
                              "inner join a_dc on a_dc.f_doc=h.f_id and a_dc.f_dc=%1 "
                              "left join a_state ds on ds.f_id=h.f_state "
                              "left join a_type dt on dt.f_id=h.f_type "
                              "left join c_partners p on p.f_id=h.f_partner "
                              "left join a_header_store hs on hs.f_id=h.f_id "
                              "left join s_user u on u.f_id=h.f_operator "
                              "where h.f_date between '%2' and '%3' and h.f_state=1 "
                              "and h.f_partner in (%4)  order by h.f_date, h.f_userid")
                .arg(dc)
                .arg(fc->d1().toString(FORMAT_DATE_TO_STR_MYSQL))
                .arg(fc->d2().toString(FORMAT_DATE_TO_STR_MYSQL))
                .arg(vals.at(0).toString());


        d->setSimpleQuery(sql);
    }
    return true;
}

void CR5DebtsToPartner::pay()
{
    int r;
    if (!currentRow(r)) {
        return;
    }
    double max = fModel->data(r, 5, Qt::EditRole).toDouble();
    if (max > 0.01) {
        max = 0;
    }
    max = abs(max);
    auto *cd = __mainWindow->createTab<C5CashDoc>(fDBParams);
    cd->setProperty("partner", fModel->data(r, 0, Qt::EditRole).toInt());
    connect(cd, SIGNAL(saved(QString)), this, SLOT(payed(QString)));
    cd->setComment(QString("%1, %2").arg(tr("Payment for dept"), fModel->data(r, 1, Qt::EditRole).toString()));
    cd->setPartner(fModel->data(r, 0, Qt::EditRole).toInt());
    cd->addRow(QString("%1, %2").arg(tr("Payment for dept"), fModel->data(r, 1, Qt::EditRole).toString()), max);
}

