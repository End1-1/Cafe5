#include "cr5draftoutputbyrecipe.h"
#include "cr5draftoutputbyrecipefilter.h"
#include "c5tablemodel.h"
#include "c5storedraftwriter.h"
#include "c5storedoc.h"
#include "c5mainwindow.h"

CR5DraftOutputByRecipe::CR5DraftOutputByRecipe(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/goods.png";
    fLabel = tr("Draft output by receipes");
    fSimpleQuery = false;

    fLeftJoinTables << "inner join o_header oh on oh.f_id=ob.f_header [oh]"
                    << "inner join d_dish d on d.f_id=ob.f_dish [d]"
                    << "inner join d_part2 dp on dp.f_id=d.f_part [dp]"
                    << "inner join d_recipes dr on dr.f_dish=d.f_id [dr]"
                    << "inner join c_storages st on st.f_id=ob.f_store [st]"
                    << "inner join c_goods g on g.f_id=dr.f_goods [g]"
                    << "inner join c_groups gr on gr.f_id=g.f_group [gr]"
                    << "inner join c_units u on u.f_id=g.f_unit [u]";

    fColumnsFields << "concat(oh.f_prefix, oh.f_hallid) as f_prefix"
                   << "oh.f_id"
                   << "oh.f_datecash"
                   << "st.f_name as f_storename"
                   << "dp.f_name as f_part2name"
                   << "d.f_name as f_dishname"
                   << "gr.f_name as f_goodsgroupname"
                   << "dr.f_goods"
                   << "g.f_name as f_goodsname"
                   << "sum(ob.f_qty1) as f_saleqty"
                   << "sum(dr.f_qty*ob.f_qty1) as f_storeqty"
                   << "u.f_name as f_unitname";

    fColumnsGroup << "concat(oh.f_prefix, oh.f_hallid) as f_prefix"
                   << "oh.f_id"
                   << "oh.f_datecash"
                   << "st.f_name as f_storename"
                   << "dp.f_name as f_part2name"
                   << "d.f_name as f_dishname"
                   << "gr.f_name as f_goodsgroupname"
                   << "dr.f_goods"
                   << "g.f_name as f_goodsname"
                   << "u.f_name as f_unitname";

    fTranslation["f_prefix"] = tr("Head");
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_datecash"] = tr("Date cash");
    fTranslation["f_storename"] = tr("Store");
    fTranslation["f_part2name"] = tr("Dish group");
    fTranslation["f_dishname"] = tr("Dish");
    fTranslation["f_goodsgroupname"] = tr("Goods group");
    fTranslation["f_goods"] = tr("Goods code");
    fTranslation["f_goodsname"] = tr("Goods");
    fTranslation["f_saleqty"] = tr("Dish qty");
    fTranslation["f_storeqty"] = tr("Store qty");
    fTranslation["f_unitname"] = tr("Unit");

    fColumnsVisible["concat(oh.f_prefix, oh.f_hallid) as f_prefix"] = false;
    fColumnsVisible["oh.f_id"] = false;
    fColumnsVisible["oh.f_datecash"] = false;
    fColumnsVisible["st.f_name as f_storename"] = true;
    fColumnsVisible["dp.f_name as f_part2name"] = false;
    fColumnsVisible["dr.f_goods"] = true;
    fColumnsVisible["d.f_name as f_dishname"] = false;
    fColumnsVisible["gr.f_name as f_goodsgroupname"] = true;
    fColumnsVisible["g.f_name as f_goodsname"] = true;
    fColumnsVisible["sum(ob.f_qty1) as f_saleqty"] = true;
    fColumnsVisible["sum(dr.f_qty*ob.f_qty1) as f_storeqty"] = true;
    fColumnsVisible["u.f_name as f_unitname"] = true;

    fColumnsOrder << "oh.f_datecash";

    fColumnsSum << "f_saleqty"
                << "f_storeqty";

    fMainTable = "o_body ob";

    fFilterWidget = new CR5DraftOutputByRecipeFilter(dbParams);
    fFilter = static_cast<CR5DraftOutputByRecipeFilter*>(fFilterWidget);
}

QToolBar *CR5DraftOutputByRecipe::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbFilter
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        fToolBar = createStandartToolbar(btn);
        fToolBar->addAction(QIcon(":/yesterday.png"), tr("Date\nback"), this, SLOT(dateBack()));
        fToolBar->addAction(QIcon(":/tomorrow.png"), tr("Date\nforward"), this, SLOT(dateForward()));
        fToolBar->addAction(QIcon(":/storeinput.png"), tr("Create store\noutput"), this, SLOT(createStoreOutput()));
    }
    return fToolBar;
}

void CR5DraftOutputByRecipe::dateBack()
{
    fFilter->addDays(-1);
    buildQuery();
}

void CR5DraftOutputByRecipe::dateForward()
{
    fFilter->addDays(1);
    buildQuery();
}

void CR5DraftOutputByRecipe::createStoreOutput()
{
    QList<IGoods> goodsSale;
    for (int i = 0; i < fModel->rowCount(); i++) {
        IGoods g;
        g.goodsId = fModel->data(i, fModel->indexForColumnName("f_goods"), Qt::EditRole).toInt();
        g.goodsQty = fModel->data(i, fModel->indexForColumnName("f_storeqty"), Qt::EditRole).toDouble();
        goodsSale.append(g);
    }
    if (goodsSale.count() > 0) {
        C5Database db(fDBParams);
        C5StoreDraftWriter dw(db);
        QString documentId;
        QString cashid;
        QString comment = QString("%1 %2-%3")
                .arg(tr("Output of sale"))
                .arg(fFilter->date1().toString(FORMAT_DATE_TO_STR))
                .arg(fFilter->date2().toString(FORMAT_DATE_TO_STR));
        dw.writeAHeader(documentId, dw.storeDocNum(DOC_TYPE_STORE_OUTPUT, fFilter->store(), true, 0), DOC_STATE_DRAFT, DOC_TYPE_STORE_OUTPUT, __userid, fFilter->date2(),
                        QDate::currentDate(), QTime::currentTime(), 0, 0, comment, 0, 0);

        dw.writeAHeaderStore(documentId, __userid, __userid, "", QDate(), 0, fFilter->store(), 0, cashid, 0, 0);
        int rownum = 1;
        foreach (const IGoods &g, goodsSale) {
            QString sdid;
            dw.writeAStoreDraft(sdid, documentId, fFilter->store(), -1, g.goodsId, g.goodsQty, g.goodsPrice, g.goodsPrice * g.goodsQty, DOC_REASON_SALE, "", rownum++, "");
        }
        //= dw.writeDraft(docDate, doctype, store, reason, data, comment);
        auto *sd = __mainWindow->createTab<C5StoreDoc>(fDBParams);
        QString e;
        if (!sd->openDoc(documentId, e)) {
            __mainWindow->removeTab(sd);
            sd = nullptr;
            C5Message::error(e);
        }
    }
}
