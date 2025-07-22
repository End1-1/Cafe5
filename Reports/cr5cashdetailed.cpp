#include "cr5cashdetailed.h"
#include "cr5cashdetailedfilter.h"
#include "c5gridgilter.h"
#include "c5tablemodel.h"
#include "c5tablewidget.h"
#include "c5mainwindow.h"
#include "c5cashdoc.h"

static const int col_header = 0;
static const int col_date = 1;
static const int col_remark = 2;
static const int col_detail = 3;
static const int col_debit = 5;
static const int col_credit = 4;
static const int col_balance = 6;

CR5CashDetailed::CR5CashDetailed(QWidget *parent) :
    C5ReportWidget(parent)
{
    fIcon = ":/cash.png";
    fLabel = tr("Cash movement");
    fFilterWidget = new CR5CashDetailedFilter();
    fFilter = static_cast<CR5CashDetailedFilter*>(fFilterWidget);
    fColumnNameIndex["f_header"] = col_header;
    fColumnNameIndex["f_date"] = col_date;
    fColumnNameIndex["f_remarks"] = col_remark;
    fColumnNameIndex["f_detail"] = col_detail;
    fColumnNameIndex["f_debit"] = col_credit;
    fColumnNameIndex["f_credit"] = col_debit;
    fColumnNameIndex["f_balance"] = col_balance;
    fTranslation["f_header"] = tr("UUID");
    fTranslation["f_date"] = tr("Date");
    fTranslation["f_remarks"] = tr("Purpose");
    fTranslation["f_detail"] = tr("Comment");
    fTranslation["f_debit"] = tr("Debit");
    fTranslation["f_credit"] = tr("Credit");
    fTranslation["f_balance"] = tr("Balance");
}

QToolBar* CR5CashDetailed::toolBar()
{
    if(!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbNew
            << ToolBarButtons::tbFilter
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        fToolBar = createStandartToolbar(btn);
    }

    return fToolBar;
}

bool CR5CashDetailed::on_tblView_doubleClicked(const QModelIndex &index)
{
    if(index.row() < 0 || index.column() < 0) {
        return false;
    }

    QJsonArray values = fModel->getRowValues(index.row());

    if(values.at(0).toString().isEmpty()) {
        return false;
    }

    auto *cd = __mainWindow->createTab<C5CashDoc>();

    if(!cd->openDoc(values.at(0).toString())) {
        __mainWindow->removeTab(cd);
    }

    return true;
}

void CR5CashDetailed::buildQuery()
{
    if(fFilter->cash() == 0) {
        if(!C5GridGilter::filter(fFilter, fWhereCondition, fColumnsVisible, fTranslation)) {
            return;
        }

        if(fFilter->cash() == 0) {
            C5Message::info(tr("Cash must be selected"));
            return;
        }
    }

    QJsonArray er;
    er << QJsonValue() << QJsonValue() << QJsonValue() << QJsonValue() << QJsonValue() << QJsonValue() << QJsonValue();
    std::vector<QJsonArray >& rows = fModel->fRawData;
    rows.clear();
    QString where = "where 1=1 ";

    if(fFilter->shift() > 0) {
        where += QString(" and h.f_working_session<%1 ").arg(fFilter->shift());
    }  else {
        where += QString(" and h.f_date<'%1' ").arg(fFilter->date1().toString(FORMAT_DATE_TO_STR_MYSQL));
    }

    QString query = QString("select '' as f_header, '%1' as f_date, '%2' as f_remarks, "
                            "c.f_sign, cast(sum(c.f_amount*c.f_sign) as float) as f_amount "
                            "from e_cash c "
                            "inner join a_header h on h.f_id=c.f_header and h.f_type=:f_type "
                            + where +
    "and f_cash=:f_cash and h.f_state=:f_state ")
                    .arg(fFilter->date1().toString(FORMAT_DATE_TO_STR_MYSQL))
                    .arg(tr("Brought forward"));
    C5Database db;
    db[":f_cash"] = fFilter->cash();
    db[":f_type"] = DOC_TYPE_CASH;
    db[":f_state"] = DOC_STATE_SAVED;
    db.exec(query);

    if(db.nextRow()) {
        int r;
        rows.push_back(er);
        r = rows.size() - 1;
        rows[r][col_header] = db.getString("f_header");
        rows[r][col_date] = db.getString("f_date");
        rows[r][col_remark] = db.getString("f_remarks");
        rows[r][col_detail] = "";

        if(db.getDouble("f_amount") > 0.001) {
            rows[r][col_credit] = db.getDouble("f_amount");
            rows[r][col_debit] = 0;
        } else {
            rows[r][col_credit] = 0;
            rows[r][col_debit] = abs(db.getDouble("f_amount"));
        }
    }

    where = "where 1=1 ";

    if(fFilter->shift() > 0) {
        where += QString(" and h.f_working_session=%1 ").arg(fFilter->shift());
    }  else {
        where += QString(" and h.f_date between '%1' and '%2' ")
                 .arg(fFilter->date1().toString(FORMAT_DATE_TO_STR_MYSQL))
                 .arg(fFilter->date2().toString(FORMAT_DATE_TO_STR_MYSQL));;
    }

    query = "select c.f_header, h.f_date, h.f_comment, c.f_remarks, c.f_sign, c.f_amount "
            "from e_cash c "
            "inner join a_header h on h.f_id=c.f_header and h.f_type=:f_type "
            + where +
    "and f_cash=:f_cash  "
    "and h.f_state=:f_state "
    "order by h.f_date, h.f_userid ";
    db[":f_cash"] = fFilter->cash();
    db[":f_type"] = DOC_TYPE_CASH;
    db[":f_state"] = DOC_STATE_SAVED;
    db.exec(query);

    while(db.nextRow()) {
        int r;
        rows.push_back(er);
        r = rows.size() - 1;
        rows[r][col_header] = db.getString("f_header");
        rows[r][col_date] = db.getString("f_date");
        rows[r][col_remark] = db.getString("f_comment");
        rows[r][col_detail] = db.getString("f_remarks");

        if(db.getInt("f_sign") == 1) {
            rows[r][col_credit] = db.getDouble("f_amount");
            rows[r][col_debit] = 0;
        } else {
            rows[r][col_credit] = 0;
            rows[r][col_debit] = abs(db.getDouble("f_amount"));
        }
    }

    fModel->fColumnType = {{4, QMetaType::Double}, {5, QMetaType::Double}, {6, QMetaType::Double}};
    fModel->setExternalData(fColumnNameIndex, fTranslation);
    restoreColumnsWidths();
    sum();
    emit refreshed();
}

void CR5CashDetailed::refreshData()
{
    buildQuery();
}

void CR5CashDetailed::restoreColumnsWidths()
{
    C5ReportWidget::restoreColumnsWidths();
    fTableView->setColumnWidth(0, 0);
}

void CR5CashDetailed::sumColumnsData()
{
    sum();
}

int CR5CashDetailed::newRow()
{
    __mainWindow->createTab<C5CashDoc>()->loadSuggest();
    return 0;
}

void CR5CashDetailed::sum()
{
    double debit = 0;
    double credit = 0;
    double total = 0;

    for(int i = 0; i < fModel->rowCount(); i++) {
        debit += fModel->data(i, col_credit, Qt::EditRole).toDouble();
        credit += fModel->data(i, col_debit, Qt::EditRole).toDouble();
        total = total + fModel->data(i, col_credit, Qt::EditRole).toDouble() - fModel->data(i, col_debit,
                Qt::EditRole).toDouble();
        fModel->setData(i, col_balance, total);
    }

    QStringList vheader;
    vheader << QString::number(fModel->rowCount());
    fTableTotal->setVerticalHeaderLabels(vheader);
    fTableTotal->setDouble(0, col_credit, debit);
    fTableTotal->setDouble(0, col_debit, credit);
    fTableTotal->setDouble(0, col_balance, total);
    fTableTotal->setVisible(true);
}
