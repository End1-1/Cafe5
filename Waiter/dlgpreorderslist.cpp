#include "dlgpreorderslist.h"
#include "ui_dlgpreorderslist.h"

#include <QDateTime>
#include <QShowEvent>
#include "c5utils.h"
#include "format_date.h"
#include "dict_currency.h"
#include "c5message.h"
#include "c5user.h"
#include "dlgtables.h"
#include "dlgorder.h"
#include "ninterface.h"

namespace {

QString creationCellText(const QString &dateStr, const QString &timeStr)
{
    const QString d = dateStr.trimmed();
    const QString t = timeStr.trimmed();

    if(d.isEmpty() && t.isEmpty()) {
        return {};
    }

    const QString combined = t.isEmpty() ? d : (d.isEmpty() ? t : QStringLiteral("%1 %2").arg(d, t));
    const QDateTime dt = QDateTime::fromString(combined, FORMAT_DATETIME_TO_STR_MYSQL);

    if(dt.isValid()) {
        return dt.toString(QStringLiteral("dd.MM.yyyy HH:mm"));
    }

    return combined.trimmed();
}

QString preorderCellText(const QString &raw)
{
    const QString s = raw.trimmed();

    if(s.isEmpty()) {
        return {};
    }

    const QDateTime dt = QDateTime::fromString(s, FORMAT_DATETIME_TO_STR_MYSQL);

    if(!dt.isValid()) {
        return s;
    }

    return dt.toString(QStringLiteral("dd.MM.yyyy HH:mm"));
}

} // namespace

DlgPreordersList::DlgPreordersList(C5User *user,
                                   const QVector<HallItem> *halls,
                                   const QVector<TableItem> *tables,
                                   const QVector<GoodsGroupItem*> *groups,
                                   const QVector<DishAItem*> *dishes,
                                   QWidget *parent)
    : C5WaiterDialog(user)
    , ui(new Ui::DlgPreordersList)
    , mHalls(halls)
    , mTables(tables)
    , mGroups(groups)
    , mDishes(dishes)
{
    Q_UNUSED(parent);
    ui->setupUi(this);
    ui->tblPreorders->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblPreorders->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tblPreorders->verticalHeader()->setVisible(false);
}

DlgPreordersList::~DlgPreordersList()
{
    delete ui;
}

void DlgPreordersList::showEvent(QShowEvent *e)
{
    C5WaiterDialog::showEvent(e);
    if(e->spontaneous() || mLoaded) {
        return;
    }
    mLoaded = true;
    loadData();
}

void DlgPreordersList::loadData()
{
    fHttp->createHttpQueryLambda("/engine/v2/waiter/order/get-preorders",
                                 {},
                                 [this](const QJsonObject &jdoc) {
                                     ui->tblPreorders->setRowCount(0);
                                     const QJsonArray rows = jdoc.value("preorders").toArray();
                                     int r = 0;
                                     for(const auto &v : rows) {
                                         const QJsonObject o = v.toObject();
                                         ui->tblPreorders->insertRow(r);
                                         ui->tblPreorders->setItem(r, 0, new QTableWidgetItem(o.value("f_id").toString()));
                                         ui->tblPreorders->setItem(r, 1, new QTableWidgetItem(o.value("f_prefix").toString()));
                                         ui->tblPreorders->setItem(r, 2, new QTableWidgetItem(o.value("f_hall_name").toString()));
                                         ui->tblPreorders->setItem(r, 3, new QTableWidgetItem(o.value("f_table_name").toString()));
                                         ui->tblPreorders->setItem(r, 4, new QTableWidgetItem(o.value("f_staff_name").toString()));
                                         ui->tblPreorders->setItem(r, 5, new QTableWidgetItem(creationCellText(o.value("f_date_open").toString(),
                                                                                                      o.value("f_time_open").toString())));
                                         ui->tblPreorders->setItem(r, 6, new QTableWidgetItem(preorderCellText(o.value("f_preorder_datetime").toString())));
                                         const double prepaidAmt = o.value("f_deposit_prepaid").toVariant().toDouble();
                                         ui->tblPreorders->setItem(r, 7,
                                                                  new QTableWidgetItem(QString("%1 %2").arg(float_str(prepaidAmt, 2), CURRENCY_SHORT)));
                                         ui->tblPreorders->setItem(r, 8, new QTableWidgetItem(o.value("f_guest_name").toString().trimmed()));
                                         ui->tblPreorders->setItem(r,
                                                                   9,
                                                                   new QTableWidgetItem(o.value("f_guest_phone").toString().trimmed()));
                                         ui->tblPreorders->setItem(r,
                                                                   10,
                                                                   new QTableWidgetItem(o.value("f_guest_address").toString().trimmed()));
                                         auto *itHiddenTable = new QTableWidgetItem(QString::number(o.value("f_table").toInt()));
                                         auto *itHiddenHall = new QTableWidgetItem(QString::number(o.value("f_hall").toInt()));
                                         ui->tblPreorders->setItem(r, 11, itHiddenTable);
                                         ui->tblPreorders->setItem(r, 12, itHiddenHall);
                                         ++r;
                                     }
                                     ui->tblPreorders->setColumnHidden(0, true);
                                     ui->tblPreorders->setColumnHidden(11, true);
                                     ui->tblPreorders->setColumnHidden(12, true);
                                     ui->tblPreorders->resizeColumnsToContents();
                                 },
                                 [](const QJsonObject &) {},
                                 {},
                                 true);
}

void DlgPreordersList::openCurrent()
{
    const int row = ui->tblPreorders->currentRow();
    if(row < 0) {
        C5Message::error(tr("Select preorder"));
        return;
    }

    mOrderId = ui->tblPreorders->item(row, 0)->text();
    mTableId = ui->tblPreorders->item(row, 11)->text().toInt();
    mHallId = ui->tblPreorders->item(row, 12)->text().toInt();
    accept();
}

void DlgPreordersList::on_btnOpen_clicked()
{
    openCurrent();
}

void DlgPreordersList::on_btnCreate_clicked()
{
    if(!mHalls || !mTables || !mGroups || !mDishes) {
        C5Message::error(tr("Data source is empty"));
        return;
    }

    DlgTables tsel(mUser);
    const int tableId = tsel.exec();
    if(tableId <= 0) {
        return;
    }

    TableItem t;
    for(const auto &ti : *mTables) {
        if(ti.id == tableId) {
            t = ti;
            break;
        }
    }

    HallItem h;
    for(const auto &hi : *mHalls) {
        if(hi.id == t.hall) {
            h = hi;
            break;
        }
    }

    if(t.id <= 0 || h.id <= 0) {
        C5Message::error(tr("Cannot resolve hall/table for selected row"));
        return;
    }

    auto *d = new DlgOrder(mUser, h, t, mGroups, mDishes);
    d->setCreateAsPreorder(true);
    d->exec();
    d->deleteLater();
    loadData();
}

void DlgPreordersList::on_btnExit_clicked()
{
    reject();
}

void DlgPreordersList::on_tblPreorders_cellDoubleClicked(int row, int column)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    openCurrent();
}

