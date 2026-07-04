#include "dlgsalarydishsales.h"
#include "ui_dlgsalarydishsales.h"

#include <QHeaderView>
#include <QJsonArray>
#include <QJsonObject>
#include <QTableWidgetItem>

#include "c5message.h"
#include "c5user.h"
#include "ninterface.h"

namespace {

QTableWidgetItem *makeReadOnlyItem(const QString &text, Qt::Alignment align = Qt::AlignLeft | Qt::AlignVCenter)
{
    auto *item = new QTableWidgetItem(text);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    item->setTextAlignment(align);
    return item;
}

} // namespace

DlgSalaryDishSales::DlgSalaryDishSales(C5User *user,
                                       const QDate &date,
                                       int staffId,
                                       const QString &staffName,
                                       int positionId,
                                       int positionCount,
                                       QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgSalaryDishSales)
    , mUser(user)
    , mDate(date)
    , mStaffId(staffId)
    , mStaffName(staffName)
    , mPositionId(positionId)
    , mPositionCount(qMax(1, positionCount))
{
    ui->setupUi(this);
    setWindowTitle(tr("Dish sales base"));

    ui->tblItems->horizontalHeader()->setStretchLastSection(false);
    ui->tblItems->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tblItems->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->tblItems->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->tblItems->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    ui->tblItems->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);

    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);

    loadData();
}

DlgSalaryDishSales::~DlgSalaryDishSales()
{
    delete ui;
}

void DlgSalaryDishSales::loadData()
{
    ui->lbHeader->setText(tr("Loading..."));
    ui->lbSummary->clear();
    ui->tblItems->setRowCount(0);

    if (mStaffId <= 0 || mPositionId <= 0 || !mDate.isValid()) {
        ui->lbHeader->setText(tr("Employee or position is not set."));
        return;
    }

    NInterface::query1(QStringLiteral("/engine/v2/officen/salary/dish-sales-detail"),
                       mUser->mSessionKey,
                       this,
                       {
                           {QStringLiteral("date"), mDate.toString(QStringLiteral("yyyy-MM-dd"))},
                           {QStringLiteral("f_staff"), mStaffId},
                           {QStringLiteral("f_position"), mPositionId},
                           {QStringLiteral("f_position_count"), mPositionCount},
                       },
                       [this](const QJsonObject &jo) { fillTable(jo); });
}

void DlgSalaryDishSales::fillTable(const QJsonObject &response)
{
    const QString staffName = response.value(QStringLiteral("f_staff_name")).toString().trimmed();
    const QString displayName = staffName.isEmpty() ? mStaffName : staffName;
    const QString dateStr = response.value(QStringLiteral("date")).toString();

    ui->lbHeader->setText(tr("Date: %1, employee: %2")
                              .arg(dateStr, displayName));

    const QJsonArray items = response.value(QStringLiteral("items")).toArray();
    ui->tblItems->setRowCount(items.size());

    for (int row = 0; row < items.size(); ++row) {
        const QJsonObject it = items.at(row).toObject();
        ui->tblItems->setItem(row, 0, makeReadOnlyItem(it.value(QStringLiteral("f_goods_name")).toString()));
        ui->tblItems->setItem(row, 1,
                              makeReadOnlyItem(QString::number(it.value(QStringLiteral("f_qty")).toDouble(), 'f', 4),
                                               Qt::AlignRight | Qt::AlignVCenter));
        ui->tblItems->setItem(row, 2,
                              makeReadOnlyItem(QString::number(it.value(QStringLiteral("f_sales")).toDouble(), 'f', 2),
                                               Qt::AlignRight | Qt::AlignVCenter));
        ui->tblItems->setItem(row, 3,
                              makeReadOnlyItem(QString::number(it.value(QStringLiteral("f_percent")).toDouble(), 'f', 2),
                                               Qt::AlignRight | Qt::AlignVCenter));
        ui->tblItems->setItem(row, 4,
                              makeReadOnlyItem(QString::number(it.value(QStringLiteral("f_pay")).toDouble(), 'f', 2),
                                               Qt::AlignRight | Qt::AlignVCenter));
    }

    const double salesTotal = response.value(QStringLiteral("f_sales_total")).toDouble();
    const double skipAmount = response.value(QStringLiteral("f_skip_amount")).toDouble();
    const double taxableSales = response.value(QStringLiteral("f_taxable_sales")).toDouble();
    const double dishBase = response.value(QStringLiteral("f_dish_taxable_base")).toDouble();
    const int divideBy = response.value(QStringLiteral("f_divide_by")).toInt(1);

    QStringList summary;
    summary << tr("Sales total: %1").arg(QString::number(salesTotal, 'f', 2));

    if (skipAmount >= 0.0 && qAbs(salesTotal - taxableSales) > 0.001) {
        summary << tr("After skip amount (%1): %2")
                       .arg(QString::number(skipAmount, 'f', 2),
                            QString::number(taxableSales, 'f', 2));
    }

    if (divideBy > 1) {
        summary << tr("Divided by employees with same position: %1").arg(divideBy);
    }

    summary << tr("Dish sales base: %1").arg(QString::number(dishBase, 'f', 2));
    ui->lbSummary->setText(summary.join(QStringLiteral("\n")));

    if (items.isEmpty()) {
        C5Message::info(tr("No percent-based dish sales found for this employee on the selected date."));
    }
}
