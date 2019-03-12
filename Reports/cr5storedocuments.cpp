#include "cr5storedocuments.h"
#include "cr5storedocumentsfilter.h"
#include "c5tablemodel.h"
#include "c5storedoc.h"

CR5StoreDocuments::CR5StoreDocuments(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fLabel = tr("Document in the store");
    fIcon = ":/documents.png";
    fSimpleQuery = true;
    fFilterWidget = new CR5StoreDocumentsFilter(dbParams);
    fTranslation["f_document"] = tr("Document");
    fTranslation["f_docstatename"] = tr("State");
    fTranslation["f_date"] = tr("Date");
    fTranslation["f_userid"] = tr("Doc num");
    fTranslation["f_typename"] = tr("Operation");
    fTranslation["f_storename"] = tr("Storage");
    fTranslation["f_amount"] = tr("Amount");
    fTranslation["f_partnername"] = tr("Partner");
    fColumnsSum << "7";
    connect(this, SIGNAL(tblDoubleClicked(int,int,QList<QVariant>)), this, SLOT(tblDoubleClickEvent(int,int,QList<QVariant>)));
}

void CR5StoreDocuments::buildQuery()
{
    fSqlQuery = "select distinct(b.f_document) as f_document, ds.f_name as f_docstatename, h.f_date, h.f_userid, \
            p.f_taxname as f_partnername, t.f_name as f_typename, s.f_name as f_storename, sum(b.f_total*b.f_type) as f_amount \
            from a_store b \
            left join a_header h on h.f_id=b.f_document \
            left join c_storages s on s.f_id=b.f_store \
            left join a_type t on t.f_id=h.f_type \
            left join a_state ds on ds.f_id=h.f_state \
            left join c_partners p on p.f_id=h.f_partner ";
    fSqlQuery += fFilterWidget->condition();
    fSqlQuery += " group by 1,2,3,4,5,6,7 order by h.f_date,h.f_userid ";
    C5Grid::buildQuery();
    fTableView->setColumnWidth(0, 0);
    sumColumnsData();
}

QToolBar *CR5StoreDocuments::toolBar()
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

void CR5StoreDocuments::tblDoubleClickEvent(int row, int column, const QList<QVariant> &values)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    if (values.count() == 0) {
        return;
    }
    C5StoreDoc *sd = __mainWindow->createTab<C5StoreDoc>(fDBParams);
    if (!sd->openDoc(values.at(0).toString())) {
        __mainWindow->removeTab(sd);
    }
}
