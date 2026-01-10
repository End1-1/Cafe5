#include "cr5storedocuments.h"
#include "cr5storedocumentsfilter.h"
#include "c5tablemodel.h"
#include "c5mainwindow.h"
#include "c5storedoc.h"
#include "c5message.h"

CR5StoreDocuments::CR5StoreDocuments(QWidget *parent) :
    C5ReportWidget(parent)
{
    fLabel = tr("Document in the store");
    fIcon = ":/documents.png";
    fSimpleQuery = true;
    fFilterWidget = new CR5StoreDocumentsFilter();
    fFilter = static_cast<CR5StoreDocumentsFilter*>(fFilterWidget);
    fTranslation["f_document"] = tr("Document");
    fTranslation["f_docstatename"] = tr("State");
    fTranslation["f_date"] = tr("Date");
    fTranslation["f_userid"] = tr("Doc num");
    fTranslation["f_typename"] = tr("Operation");
    fTranslation["f_storename"] = tr("Storage");
    fTranslation["f_amount"] = tr("Amount");
    fTranslation["f_partnername"] = tr("Partner");
    fTranslation["f_comment"] = tr("Comment");
    fColumnsSum << "f_amount";
}

void CR5StoreDocuments::buildQuery()
{
    fSqlQuery = "SELECT h.f_id as f_document, t.f_name as f_typename,"
                "h.f_date, h.f_userid, ds.f_name as f_docstatename, "
                "p.f_taxname as f_partnername, "
                "concat(coalesce(so.f_name, ''), '->', coalesce(si.f_name, '')) as f_storename,  "
                "h.f_amount, h.f_comment  "
                "from a_header h "
                "LEFT JOIN a_header_store hs ON hs.f_id=h.f_id  "
                "LEFT JOIN c_storages so ON so.f_id=hs.f_storeout "
                "LEFT JOIN c_storages si ON si.f_id=hs.f_storein "
                "LEFT JOIN a_state ds ON ds.f_id=h.f_state "
                "LEFT JOIN c_partners p ON p.f_id=h.f_partner "
                "LEFT JOIN a_type t ON t.f_id=h.f_type "
                + fFilter->condition();
    fOrderCondition = "order by h.f_date,h.f_userid ";
    C5Grid::buildQuery();
    fTableView->setColumnWidth(0, 0);
    sumColumnsData();
}

QToolBar* CR5StoreDocuments::toolBar()
{
    if(!fToolBar) {
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

bool CR5StoreDocuments::tblDoubleClicked(int row, int column, const QJsonArray &values)
{
    Q_UNUSED(row);
    Q_UNUSED(column);

    if(values.count() == 0) {
        return true;
    }

    QString e;
    C5StoreDoc *sd = __mainWindow->createTab<C5StoreDoc>();

    if(!sd->openDoc(values.at(0).toString(), e)) {
        __mainWindow->removeTab(sd);
        C5Message::error(e);
    }

    return true;
}
