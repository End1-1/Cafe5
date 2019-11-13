#include "cr5debtstopartner.h"
#include "c5mainwindow.h"
#include "cr5storedocuments.h"

CR5DebtsToPartner::CR5DebtsToPartner(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/contract.png";
    fLabel = tr("Debts to partners");
    fSimpleQuery = true;
    fSqlQuery = "select p.f_Id, p.f_taxname, sum(h.f_amount) as f_sum from c_partners p "
                "inner join a_header h on h.f_partner=p.f_id "
                "where h.f_state=1 and h.f_type=1 and h.f_paid=0 "
                "group by 1, 2 ";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_taxname"] = tr("Partner");
    fTranslation["f_sum"] = tr("Debt");
    connect(this, SIGNAL(tblDoubleClicked(int,int,QList<QVariant>)), this, SLOT(doubleClick(int,int,QList<QVariant>)));
}

QToolBar *CR5DebtsToPartner::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
            btn << ToolBarButtons::tbClearFilter
                << ToolBarButtons::tbRefresh
                << ToolBarButtons::tbExcel
                << ToolBarButtons::tbPrint;
            createStandartToolbar(btn);
    }
    return fToolBar;
}

void CR5DebtsToPartner::doubleClick(int row, int column, const QList<QVariant> &vals)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    if (vals.count() == 0) {
        return;
    }
    CR5StoreDocuments *d = __mainWindow->createTab<CR5StoreDocuments>(fDBParams);
    d->setPartnersFilter(vals.at(0).toInt());
    d->buildQuery();
}
