#include "cr5goodsmovement.h"
#include "cr5goodsmovementfilter.h"
#include "c5mainwindow.h"
#include "c5storedoc.h"
#include "c5changedocinputprice.h"
#include "c5tablemodel.h"
#include "c5dlgselectreporttemplate.h"

CR5GoodsMovement::CR5GoodsMovement(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/goods.png";
    fLabel = tr("Movements in the store");
    fSimpleQuery = false;
    fMainTable = "a_store_draft s";
    fLeftJoinTables << "left join a_type_sign st on st.f_id=s.f_type [st]"
                    << "left join a_type dt on dt.f_id=a.f_type [dt]"
                    << "left join c_storages ss on ss.f_id=s.f_store [ss]"
                    << "left join c_goods g on g.f_id=s.f_goods [g]"
                    << "left join c_groups gg on gg.f_id=g.f_group [gg]"
                    << "left join c_units u on u.f_id=g.f_unit [u]"
                    << "left join a_header a on a.f_id=s.f_document [a]"
                    << "left join a_state ass on ass.f_id=a.f_state [ass]"
                    << "left join a_header_store ai on ai.f_id=a.f_id [ai]"
                    << "Left join c_goods_prices gpr on gpr.f_goods=g.f_id [gpr]"
                    << "left join c_partners p on p.f_id=a.f_partner [p]"
                    ;
    fColumnsFields << "s.f_document"
                   << "s.f_id as f_storerec"
                   << "dt.f_name as f_docname"
                   << "a.f_date"
                   << "ass.f_name as f_statename"
                   << "a.f_userid"
                   << "p.f_taxname"
                   << "ss.f_name as f_store"
                   << "st.f_name as f_type"
                   << "gg.f_name as f_group"
                   << "g.f_name as f_goods"
                   << "g.f_scancode"
                   << "s.f_comment as f_storecomment"
                   << "a.f_comment as f_doccomment"
                   << "sum(s.f_qty) as f_qty"
                   << "u.f_name as f_unit"
                   << "s.f_price"
                   << "sum(s.f_total) as f_total"
                   << "gpr.f_price1"
                   << "sum(s.f_qty)*if(gpr.f_price1disc>0,gpr.f_price1disc,gpr.f_price1) as f_salepricetotal"
                   << "gpr.f_price2"
                   << "sum(s.f_qty)*if(gpr.f_price2disc>0,gpr.f_price2disc,gpr.f_price2) as f_salepricetotal2"
                   << "a.f_comment"
                   ;
    fColumnsGroup << "s.f_document"
                  << "s.f_id as f_storerec"
                  << "dt.f_name as f_docname"
                  << "a.f_userid"
                  << "a.f_date"
                  << "ass.f_name as f_statename"
                  << "a.f_userid"
                  << "p.f_taxname"
                  << "st.f_name as f_type"
                  << "ss.f_name as f_store"
                  << "gg.f_name as f_group"
                  << "g.f_name as f_goods"
                  << "g.f_scancode"
                  << "s.f_comment as f_storecomment"
                  << "a.f_comment as f_doccomment"
                  << "gpr.f_price1"
                  << "gpr.f_price2"
                  << "s.f_price"
                  << "u.f_name as f_unit"
                  << "g.f_name as f_goods"
                  << "a.f_comment"
                  ;
    fColumnsSum << "f_qty"
                << "f_total"
                << "f_salepricetotal"
                << "f_salepricetotal2"
                ;
    fTranslation["f_userid"] = tr("Document");
    fTranslation["f_storerec"] = tr("Store record");
    fTranslation["f_docname"] = tr("Type");
    fTranslation["f_date"] = tr("Date");
    fTranslation["f_document"]  = tr("Op.num.");
    fTranslation["f_type"] = tr("In/Out");
    fTranslation["f_store"] = tr("Storage");
    fTranslation["f_group"] = tr("Group");
    fTranslation["f_goods"] = tr("Goods");
    fTranslation["f_scancode"] = tr("Scancode");
    fTranslation["f_taxname"] = tr("Partner");
    fTranslation["f_storecomment"] = tr("Record comment");
    fTranslation["f_doccomment"] = tr("Document comment");
    fTranslation["f_saleprice"] = tr("Retail price");
    fTranslation["f_saleprice2"] = tr("Whosale price");
    fTranslation["f_qty"] = tr("Qty");
    fTranslation["f_unit"] = tr("Unit");
    fTranslation["f_price"] = tr("Price");
    fTranslation["f_total"] = tr("Amount");
    fTranslation["f_salepricetotal"] = tr("Total retail");
    fTranslation["f_salepricetotal2"] = tr("Total whosale");
    fTranslation["f_comment"] = tr("Comment");
    fTranslation["f_statename"] = tr("State");
    fColumnsVisible["a.f_date"] = true;
    fColumnsVisible["s.f_id as f_storerec"] = false;
    fColumnsVisible["dt.f_name as f_docname"] = true;
    fColumnsVisible["a.f_userid"] = true;
    fColumnsVisible["s.f_document"] = false;
    fColumnsVisible["st.f_name as f_type"] = true;
    fColumnsVisible["ss.f_name as f_store"] = true;
    fColumnsVisible["gg.f_name as f_group"] = true;
    fColumnsVisible["g.f_name as f_goods"] = true;
    fColumnsVisible["g.f_scancode"] = true;
    fColumnsVisible["gpr.f_price1"] = true;
    fColumnsVisible["gpr.f_price2"] = false;
    fColumnsVisible["sum(s.f_qty) as f_qty"] = true;
    fColumnsVisible["u.f_name as f_unit"] = true;
    fColumnsVisible["s.f_price"] = true;
    fColumnsVisible["p.f_taxname"] = false;
    fColumnsVisible["sum(s.f_total) as f_total"] = true;
    fColumnsVisible["s.f_comment as f_storecomment"] = false;
    fColumnsVisible["a.f_comment as f_doccomment"] = false;
    fColumnsVisible["sum(gpr.f_price1*s.f_qty) as f_salepricetotal"] = true;
    fColumnsVisible["sum(gpr.f_price2*s.f_qty) as f_salepricetotal2"] = false;
    fColumnsVisible["f_comment"] = true;
    fColumnsVisible["ass.f_name as f_statename"] = true;
    restoreColumnsVisibility();
    fFilterWidget = new CR5GoodsMovementFilter(fDBParams);
    fFilter = static_cast<CR5GoodsMovementFilter *>(fFilterWidget);
}

QToolBar *CR5GoodsMovement::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbFilter
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        fToolBar = createStandartToolbar(btn);
        auto *a = fToolBar->addAction(QIcon(":/pricing.png"), tr("Change price"), this, SLOT(changePrice()));
        a = new QAction(QIcon(":/template.png"), tr("Templates"), this);
        connect(a, SIGNAL(triggered()), this, SLOT(templates()));
        fToolBar->insertAction(fToolBar->actions().at(3), a);
    }
    return fToolBar;
}

void CR5GoodsMovement::setDate(const QDate &d1, const QDate &d2)
{
    fFilter->setDate(d1, d2);
}

void CR5GoodsMovement::setDocType(const QString &docType)
{
    fFilter->setDocType(docType);
}

void CR5GoodsMovement::setStore(const QString &store)
{
    fFilter->setStore(store);
}

void CR5GoodsMovement::setGoods(const QString &goods)
{
    fFilter->setGoods(goods);
}

void CR5GoodsMovement::setReason(const QString &reason)
{
    fFilter->setReason(reason);
}

void CR5GoodsMovement::setInOut(int inout)
{
    fFilter->setInOut(inout);
}

bool CR5GoodsMovement::tblDoubleClicked(int row, int column, const QList<QVariant> &values)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    if (!fColumnsVisible["s.f_document"]) {
        C5Message::info(tr("Document id column must be included in the report"));
        return true;
    }
#ifdef NEWVERSION
    auto *si = __mainWindow->createTab<StoreInputDocument>(fDBParams);
    if (!si->openDoc(values.at(fModel->indexForColumnName("f_document")).toString())) {
        __mainWindow->removeTab(si);
    }
#else
    C5StoreDoc *sd = __mainWindow->createTab<C5StoreDoc>(fDBParams);
    QString e;
    if (!sd->openDoc(values.at(fModel->indexForColumnName("f_document")).toString(), e)) {
        __mainWindow->removeTab(sd);
        C5Message::error(e);
    }
#endif
    return true;
}

void CR5GoodsMovement::changePrice()
{
    if (!fColumnsVisible["s.f_id as f_storerec"]) {
        C5Message::info(tr("The store record column must be included in the report"));
        return;
    }
    QModelIndexList ml = this->fTableView->selectionModel()->selectedIndexes();
    QSet<int> rows;
    foreach (QModelIndex m, ml) {
        rows << m.row();
    }
    if (rows.count() > 1) {
        C5Message::error(tr("Select only one row"));
        return;
    }
    if (rows.count() == 0) {
        C5Message::error(tr("Nothing was selected"));
        return;
    }
    C5ChangeDocInputPrice::changePrice(fDBParams, fModel->data(rows.values().at(0),
                                       fModel->indexForColumnName("f_storerec"),
                                       Qt::EditRole).toString());
}

void CR5GoodsMovement::templates()
{
    C5DlgSelectReportTemplate d(2, fDBParams);
    if (d.exec() == QDialog::Accepted) {
        QString sql = d.fSelectedTemplate.sql;
        sql.replace("%date1", fFilter->date1s());
        sql.replace("%date2", fFilter->date2s());
        if (sql.contains("%partner")) {
            if (fFilter->partners().isEmpty()) {
                sql.replace("%partner", "");
            } else {
                sql.replace("%partner", QString("and a.f_partner in(%1)").arg(fFilter->partners()));
            }
        }
        executeSql(sql);
    }
}

void CR5GoodsMovement::restoreColumnsWidths()
{
    C5ReportWidget::restoreColumnsWidths();
    int idx = fModel->indexForColumnName("f_document");
    if (idx != -1) {
        fTableView->setColumnWidth(idx, 0);
    }
}
