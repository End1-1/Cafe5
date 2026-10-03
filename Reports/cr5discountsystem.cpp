#include "cr5discountsystem.h"
#include "ce5discountcard.h"
#include "c5editor.h"
#include "c5tablemodel.h"

CR5DiscountSystem::CR5DiscountSystem(QWidget *parent) :
    C5ReportWidget(parent)
{
    fLabel = tr("Discount system");
    fIconName = ":/discount.png";
    fSimpleQuery = true;
    fSqlQuery =
        "select d.f_id, d.f_number, coalesce(dt.f_name, '') as dtname, "
        "concat_ws(', ', c.f_contact, c.f_taxname, c.f_name, c.f_phone) as f_contact, "
        "d.f_value, c.f_info, d.f_code, d.f_datestart, d.f_dateend, d.f_active, d.f_mode "
        "from ("
        "  select f_id, f_number, f_client, f_value, f_mode, f_code, f_datestart, f_dateend, f_active "
        "  from b_discount_cards "
        "  union all "
        "  select f_id, f_number, f_client, f_value, 4 as f_mode, f_code, f_datestart, f_dateend, f_active "
        "  from b_accumulate_cards"
        ") d "
        "left join c_partners c on c.f_id=d.f_client "
        "left join b_card_types dt on dt.f_id=d.f_mode "
        "order by d.f_code";
    fTranslation["f_id"] = tr("Code");
    fTranslation["dtname"] = tr("Mode");
    fTranslation["f_number"] = tr("Card number");
    fTranslation["f_contact"] = tr("Contact name");
    fTranslation["f_value"] = tr("Value");
    fTranslation["f_info"] = tr("Client info");
    fTranslation["f_code"] = tr("Card code");
    fTranslation["f_datestart"] = tr("Start date");
    fTranslation["f_dateend"] = tr("End date");
    fTranslation["f_active"] = tr("State");
    fTranslation["f_mode"] = tr("Mode id");
    fColumnsVisible["f_id"] = true;
    fColumnsVisible["f_number"] = true;
    fColumnsVisible["dtname"] = true;
    fColumnsVisible["f_contact"] = true;
    fColumnsVisible["f_value"] = true;
    fColumnsVisible["f_info"] = true;
    fColumnsVisible["f_code"] = true;
    fColumnsVisible["f_datestart"] = true;
    fColumnsVisible["f_dateend"] = true;
    fColumnsVisible["f_active"] = true;
    fColumnsVisible["f_mode"] = false;
    restoreColumnsVisibility();
    fEditor = new CE5DiscountCard();
}

QToolBar* CR5DiscountSystem::toolBar()
{
    if(!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbNew
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        fToolBar = createStandartToolbar(btn);
    }

    return fToolBar;
}

void CR5DiscountSystem::completeRefresh()
{
    C5ReportWidget::completeRefresh();
    const int modeCol = fModel->indexForColumnName("f_mode");

    if(modeCol > -1) {
        fTableView->setColumnHidden(modeCol, true);
    }
}

bool CR5DiscountSystem::on_tblView_doubleClicked(const QModelIndex &index)
{
    if(index.row() < 0 || index.column() < 0) {
        return false;
    }

    QJsonArray values = fModel->getRowValues(index.row());

    if(tblDoubleClicked(index.row(), index.column(), values)) {
        return false;
    }

    if(!fEditor) {
        return false;
    }

    const int idCol = fModel->indexForColumnName("f_id");
    const int modeCol = fModel->indexForColumnName("f_mode");
    const int id = idCol > -1 ? fModel->data(index.row(), idCol, Qt::EditRole).toInt() : values.at(0).toInt();
    const int mode = modeCol > -1 ? fModel->data(index.row(), modeCol, Qt::EditRole).toInt() : 1;
    auto *card = static_cast<CE5DiscountCard*>(fEditor);
    card->prepareLoad(mode);
    C5Editor *e = C5Editor::createEditor(mUser, fEditor, id);
    QList<QMap<QString, QVariant> > data;
    const bool yes = e->getResult(data);
    fEditor->setParent(nullptr);
    delete e;

    if(!yes) {
        return false;
    }

    int row = index.row();

    for(int i = 0; i < data.count(); i++) {
        if(i > 0) {
            fModel->insertRow(row);
            row++;
        }

        for(QMap<QString, QVariant>::const_iterator it = data.at(i).begin(); it != data.at(i).end(); it++) {
            const int col = fModel->indexForColumnName(it.key());

            if(col > -1) {
                fModel->setData(row, col, it.value());
            }
        }
    }

    return true;
}

int CR5DiscountSystem::newRow()
{
    if(fEditor == nullptr) {
        return -1;
    }

    auto *card = static_cast<CE5DiscountCard*>(fEditor);
    card->prepareLoad(0);
    C5Editor *e = C5Editor::createEditor(mUser, fEditor, 0);
    QList<QMap<QString, QVariant> > data;
    const bool yes = e->getResult(data);
    fEditor->setParent(nullptr);
    delete e;

    if(!yes) {
        return -1;
    }

    int row = 0;
    QModelIndexList ml = fTableView->selectionModel()->selectedIndexes();

    if(ml.count() > 0) {
        row = ml.at(0).row();
    } else {
        row = fModel->rowCount();
    }

    for(int i = 0; i < data.count(); i++) {
        fModel->insertRow(row);

        if(ml.count() > 0) {
            row++;
        }

        for(QMap<QString, QVariant>::const_iterator it = data.at(i).begin(); it != data.at(i).end(); it++) {
            const int col = fModel->indexForColumnName(it.key());

            if(col > -1) {
                fModel->setData(row, col, it.value());
            }
        }
    }

    fTableView->setCurrentIndex(fModel->index(row + 1, 0));
    return row;
}
