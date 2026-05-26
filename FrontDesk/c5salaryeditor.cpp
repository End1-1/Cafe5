#include "c5salaryeditor.h"
#include "ui_c5salaryeditor.h"

#include <QDate>
#include <QHeaderView>
#include <QJsonArray>
#include <QPrintDialog>
#include <QPrinter>
#include <QSignalBlocker>
#include <QTextDocument>

#include "c5codenameselectorfunctions.h"
#include "c5message.h"
#include "c5user.h"
#include "c5utils.h"
#include "ninterface.h"

namespace {

constexpr int kSalaryTypeAccrual = 1;

double cellAmount(const QTableWidget *table, int row, int col)
{
    const auto *it = table->item(row, col);
    return it ? str_table_amount(it->text()) : 0.0;
}

void setMoneyCell(QTableWidget *table, int row, int col, double value, bool editable = true)
{
    auto *it = table->item(row, col);
    if (!it) {
        it = new QTableWidgetItem();
        it->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        table->setItem(row, col, it);
    }
    it->setText(QString::number(value, 'f', 2));
    if (!editable) {
        it->setFlags(it->flags() & ~Qt::ItemIsEditable);
    }
}

} // namespace

C5SalaryEditor::C5SalaryEditor(QWidget *parent)
    : C5Widget(parent)
    , ui(new Ui::C5SalaryEditor)
{
    ui->setupUi(this);
    fLabel = tr("Salary accrual");
    fIcon = QIcon(":/salary.png");
    ui->deDate->setDate(QDate::currentDate());
    mOpenDate = ui->deDate->date();
    mLoadOnShow = true;
    initTable();
    recalcTotal();

    connect(ui->tblSalary, &QTableWidget::cellChanged, this, [this](int row, int column) {
        if (column == colFixed || column == colCalculated || column == colBonus) {
            updateRowTotal(row);
        } else if (column == colTotal) {
            recalcTotal();
        }
    });

    connect(ui->tblSalary, &QTableWidget::cellDoubleClicked, this, [this](int row, int column) {
        if (column != colPosition) {
            return;
        }

        ui->tblSalary->setCurrentCell(row, column);
        on_btnChangePosition_clicked();
    });

    connect(ui->deDate, &QDateEdit::dateChanged, this, [this](const QDate &d) {
        if (!d.isValid()) {
            return;
        }

        if (ui->tblSalary->rowCount() > 0
            && C5Message::question(tr("Load document for the selected date? Current rows will be replaced."))
                   != QDialog::Accepted) {
            QSignalBlocker sb(ui->deDate);
            ui->deDate->setDate(mOpenDate.isValid() ? mOpenDate : QDate::currentDate());
            return;
        }

        open(d);
    });
}

C5SalaryEditor::~C5SalaryEditor()
{
    delete ui;
}

QToolBar *C5SalaryEditor::toolBar()
{
    if (!fToolBar) {
        createToolBar();
        fToolBar->addAction(QIcon(":/save.png"), tr("Save"), this, SLOT(saveDocument()));
        fToolBar->addAction(QIcon(":/calc.png"), tr("Calculate"), this, SLOT(calculateDocument()));
        fToolBar->addAction(QIcon(":/recycle.png"), tr("Remove"), this, SLOT(removeDocument()));
        fToolBar->addAction(QIcon(":/print.png"), tr("Print"), this, SLOT(print()));
    }

    return fToolBar;
}

void C5SalaryEditor::open(const QDate &date)
{
    QSignalBlocker sb(ui->deDate);
    ui->deDate->setDate(date);
    mOpenDate = date;
    mLoadOnShow = true;

    if (isVisible()) {
        showEvent(nullptr);
    }
}

void C5SalaryEditor::showEvent(QShowEvent *e)
{
    if (e) {
        QWidget::showEvent(e);
    }

    if (!mLoadOnShow) {
        return;
    }

    if (e && e->spontaneous()) {
        return;
    }

    mLoadOnShow = false;

    const QString dateStr = mOpenDate.isValid() ? mOpenDate.toString(QStringLiteral("yyyy-MM-dd"))
                                                : ui->deDate->date().toString(QStringLiteral("yyyy-MM-dd"));

    QSignalBlocker sb(ui->tblSalary);
    ui->tblSalary->setRowCount(0);
    ui->leTotalAmount->setText(QStringLiteral("0.00"));

    NInterface::query1(QStringLiteral("/engine/v2/officen/salary/open"),
                       mUser->mSessionKey,
                       this,
                       {{QStringLiteral("date"), dateStr}, {QStringLiteral("f_type"), kSalaryTypeAccrual}},
                       [this](const QJsonObject &jo) {
                           QSignalBlocker sb2(ui->tblSalary);

                           const QJsonArray items = jo.value(QStringLiteral("items")).toArray();
                           ui->tblSalary->setRowCount(0);

                           int row = 0;
                           for (const auto &v : items) {
                               const QJsonObject it = v.toObject();

                               const double fixed = it.value(QStringLiteral("f_fixed")).toString().toDouble();
                               const double calculated = it.value(QStringLiteral("f_calculated")).toString().toDouble();
                               const double bonus = it.value(QStringLiteral("f_bonus")).toString().toDouble();
                               const double total = fixed + calculated + bonus;

                               ui->tblSalary->insertRow(row);

                               auto *itDbId = new QTableWidgetItem(QString::number(it.value(QStringLiteral("f_id")).toInt()));
                               auto *itPositionId = new QTableWidgetItem(QString::number(it.value(QStringLiteral("f_position")).toInt()));
                               auto *itStaffId = new QTableWidgetItem(QString::number(it.value(QStringLiteral("f_staff")).toInt()));
                               auto *itNum = new QTableWidgetItem(QString::number(row + 1));
                               auto *itPos = new QTableWidgetItem(it.value(QStringLiteral("f_position_name")).toString());
                               auto *itName = new QTableWidgetItem(it.value(QStringLiteral("f_staff_name")).toString());
                               auto *itFixed = new QTableWidgetItem(QString::number(fixed, 'f', 2));
                               auto *itDishBase = new QTableWidgetItem(QStringLiteral("0.00"));
                               auto *itCalculated = new QTableWidgetItem(QString::number(calculated, 'f', 2));
                               auto *itBonus = new QTableWidgetItem(QString::number(bonus, 'f', 2));
                               auto *itTotal = new QTableWidgetItem(QString::number(total, 'f', 2));

                               itDbId->setFlags(itDbId->flags() & ~Qt::ItemIsEditable);
                               itPositionId->setFlags(itPositionId->flags() & ~Qt::ItemIsEditable);
                               itStaffId->setFlags(itStaffId->flags() & ~Qt::ItemIsEditable);
                               itNum->setFlags(itNum->flags() & ~Qt::ItemIsEditable);
                               itPos->setFlags(itPos->flags() & ~Qt::ItemIsEditable);
                               itName->setFlags(itName->flags() & ~Qt::ItemIsEditable);
                               itTotal->setFlags(itTotal->flags() & ~Qt::ItemIsEditable);

                               itNum->setTextAlignment(Qt::AlignCenter);
                               itFixed->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
                               itDishBase->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
                               itDishBase->setFlags(itDishBase->flags() & ~Qt::ItemIsEditable);
                               itCalculated->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
                               itBonus->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
                               itTotal->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

                               ui->tblSalary->setItem(row, colDbId, itDbId);
                               ui->tblSalary->setItem(row, colPositionId, itPositionId);
                               ui->tblSalary->setItem(row, colStaffId, itStaffId);
                               ui->tblSalary->setItem(row, colNum, itNum);
                               ui->tblSalary->setItem(row, colPosition, itPos);
                               ui->tblSalary->setItem(row, colName, itName);
                               ui->tblSalary->setItem(row, colFixed, itFixed);
                               ui->tblSalary->setItem(row, colDishBase, itDishBase);
                               ui->tblSalary->setItem(row, colCalculated, itCalculated);
                               ui->tblSalary->setItem(row, colBonus, itBonus);
                               ui->tblSalary->setItem(row, colTotal, itTotal);

                               ++row;
                           }

                           recalcTotal();
                       });
}

void C5SalaryEditor::appendStaffRow(int staffId, const QString &staffName, int positionId, const QString &positionName)
{
    const int row = ui->tblSalary->rowCount();
    ui->tblSalary->insertRow(row);

    auto *itDbId = new QTableWidgetItem(QStringLiteral("0"));
    auto *itPositionId = new QTableWidgetItem(QString::number(positionId));
    auto *itStaffId = new QTableWidgetItem(QString::number(staffId));
    auto *itNum = new QTableWidgetItem(QString::number(row + 1));
    auto *itPos = new QTableWidgetItem(positionName);
    auto *itName = new QTableWidgetItem(staffName);
    auto *itFixed = new QTableWidgetItem(QStringLiteral("0.00"));
    auto *itDishBase = new QTableWidgetItem(QStringLiteral("0.00"));
    auto *itCalculated = new QTableWidgetItem(QStringLiteral("0.00"));
    auto *itBonus = new QTableWidgetItem(QStringLiteral("0.00"));
    auto *itTotal = new QTableWidgetItem(QStringLiteral("0.00"));

    itDbId->setFlags(itDbId->flags() & ~Qt::ItemIsEditable);
    itPositionId->setFlags(itPositionId->flags() & ~Qt::ItemIsEditable);
    itStaffId->setFlags(itStaffId->flags() & ~Qt::ItemIsEditable);
    itNum->setFlags(itNum->flags() & ~Qt::ItemIsEditable);
    itPos->setFlags(itPos->flags() & ~Qt::ItemIsEditable);
    itName->setFlags(itName->flags() & ~Qt::ItemIsEditable);
    itTotal->setFlags(itTotal->flags() & ~Qt::ItemIsEditable);

    itNum->setTextAlignment(Qt::AlignCenter);
    itFixed->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    itDishBase->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    itDishBase->setFlags(itDishBase->flags() & ~Qt::ItemIsEditable);
    itCalculated->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    itBonus->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    itTotal->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

    ui->tblSalary->setItem(row, colDbId, itDbId);
    ui->tblSalary->setItem(row, colPositionId, itPositionId);
    ui->tblSalary->setItem(row, colStaffId, itStaffId);
    ui->tblSalary->setItem(row, colNum, itNum);
    ui->tblSalary->setItem(row, colPosition, itPos);
    ui->tblSalary->setItem(row, colName, itName);
    ui->tblSalary->setItem(row, colFixed, itFixed);
    ui->tblSalary->setItem(row, colDishBase, itDishBase);
    ui->tblSalary->setItem(row, colCalculated, itCalculated);
    ui->tblSalary->setItem(row, colBonus, itBonus);
    ui->tblSalary->setItem(row, colTotal, itTotal);
    ui->tblSalary->setCurrentCell(row, colFixed);
}

void C5SalaryEditor::on_btnAddStaff_clicked()
{
    const auto employees = selectItem<StructEmployee>(true, false, ui->btnAddStaff->mapToGlobal(QPoint(0, ui->btnAddStaff->height())));
    if (employees.isEmpty()) {
        return;
    }

    const auto &employee = employees.first();
    QString employeeName = QString("%1 %2").arg(employee.firstName, employee.lastName).trimmed();
    if (employeeName.isEmpty()) {
        employeeName = employee.login;
    }

    const QPoint selectorPos = ui->btnAddStaff->mapToGlobal(QPoint(0, ui->btnAddStaff->height()));

    NInterface::query1(QStringLiteral("/engine/v2/officen/salary/staffposition"),
                       mUser->mSessionKey,
                       this,
                       {{QStringLiteral("staff"), employee.id}},
                       [this, employee, employeeName, selectorPos](const QJsonObject &jo) {
                           int positionId = jo.value(QStringLiteral("f_position")).toInt();
                           QString positionName = jo.value(QStringLiteral("f_position_name")).toString().trimmed();

                           if (positionId <= 0) {
                               positionId = employee.groupId;
                           }
                           if (positionName.isEmpty()) {
                               positionName = employee.groupName.trimmed();
                           }

                           if (positionId <= 0 || positionName.isEmpty()) {
                               const auto groups = selectItem<StructEmployeeGroup>(true, false, selectorPos);
                               if (groups.isEmpty()) {
                                   return;
                               }
                               positionId = groups.first().id;
                               positionName = groups.first().name;
                           }

                           appendStaffRow(employee.id, employeeName, positionId, positionName);
                       });
}

void C5SalaryEditor::on_btnRemoveStaff_clicked()
{
    const int row = ui->tblSalary->currentRow();
    if (row < 0) {
        return;
    }

    ui->tblSalary->removeRow(row);
    for (int i = 0; i < ui->tblSalary->rowCount(); ++i) {
        auto *it = ui->tblSalary->item(i, colNum);
        if (!it) {
            it = new QTableWidgetItem();
            it->setFlags(it->flags() & ~Qt::ItemIsEditable);
            it->setTextAlignment(Qt::AlignCenter);
            ui->tblSalary->setItem(i, colNum, it);
        }
        it->setText(QString::number(i + 1));
    }
    recalcTotal();
}

void C5SalaryEditor::on_btnChangePosition_clicked()
{
    const int row = ui->tblSalary->currentRow();
    if (row < 0) {
        return;
    }

    QPoint selectorPos = {-1, -1};
    if (auto *it = ui->tblSalary->item(row, colPosition)) {
        const QRect r = ui->tblSalary->visualItemRect(it);
        selectorPos = ui->tblSalary->viewport()->mapToGlobal(QPoint(r.left(), r.bottom()));
    }

    const auto groups = selectItem<StructEmployeeGroup>(true, false, selectorPos);
    if (groups.isEmpty()) {
        return;
    }

    auto *itId = ui->tblSalary->item(row, colPositionId);
    if (!itId) {
        itId = new QTableWidgetItem();
        itId->setFlags(itId->flags() & ~Qt::ItemIsEditable);
        ui->tblSalary->setItem(row, colPositionId, itId);
    }
    itId->setText(QString::number(groups.first().id));

    auto *itName = ui->tblSalary->item(row, colPosition);
    if (!itName) {
        itName = new QTableWidgetItem();
        ui->tblSalary->setItem(row, colPosition, itName);
    }
    itName->setFlags(itName->flags() & ~Qt::ItemIsEditable);
    itName->setText(groups.first().name);
}

void C5SalaryEditor::applyCalculatedRow(int row, double fixed, double dishBase, double calculated, double total)
{
    setMoneyCell(ui->tblSalary, row, colFixed, fixed, true);
    setMoneyCell(ui->tblSalary, row, colDishBase, dishBase, false);
    setMoneyCell(ui->tblSalary, row, colCalculated, calculated, true);
    setMoneyCell(ui->tblSalary, row, colTotal, total, false);
}

void C5SalaryEditor::calculateDocument()
{
    commit_table_edits(ui->tblSalary);

    if (ui->tblSalary->rowCount() == 0) {
        C5Message::info(tr("Add employees first."));
        return;
    }

    QJsonArray rows;

    for (int row = 0; row < ui->tblSalary->rowCount(); ++row) {
        const int staffId = ui->tblSalary->item(row, colStaffId)
                                ? ui->tblSalary->item(row, colStaffId)->text().toInt()
                                : 0;
        const int positionId = ui->tblSalary->item(row, colPositionId)
                                 ? ui->tblSalary->item(row, colPositionId)->text().toInt()
                                 : 0;
        const double bonus = cellAmount(ui->tblSalary, row, colBonus);

        QJsonObject r;
        r[QStringLiteral("f_staff")] = staffId;
        r[QStringLiteral("f_position")] = positionId;
        r[QStringLiteral("f_bonus")] = bonus;
        rows.push_back(r);
    }

    const QString dateStr = ui->deDate->date().toString(QStringLiteral("yyyy-MM-dd"));

    QJsonObject params;
    params[QStringLiteral("date")] = dateStr;
    params[QStringLiteral("rows")] = rows;

    NInterface::query1(QStringLiteral("/engine/v2/officen/salary/calculate-accrual"),
                       mUser->mSessionKey,
                       this,
                       params,
                       [this, rows](const QJsonObject &jo) {
                           const QJsonArray items = jo.value(QStringLiteral("items")).toArray();
                           const int n = qMin(items.size(), rows.size());

                           QSignalBlocker sb(ui->tblSalary);
                           for (int i = 0; i < n; ++i) {
                               const QJsonObject it = items.at(i).toObject();
                               applyCalculatedRow(i,
                                                  it.value(QStringLiteral("f_fixed")).toDouble(),
                                                  it.value(QStringLiteral("f_dish_taxable_base")).toDouble(),
                                                  it.value(QStringLiteral("f_calculated")).toDouble(),
                                                  it.value(QStringLiteral("f_total")).toDouble());
                           }
                           recalcTotal();
                       });
}

void C5SalaryEditor::saveDocument()
{
    commit_table_edits(ui->tblSalary);

    QJsonArray items;
    int rowsNoStaff = 0;
    int rowsNoAmount = 0;

    for (int row = 0; row < ui->tblSalary->rowCount(); ++row) {
        const auto *itStaff = ui->tblSalary->item(row, colStaffId);
        const auto *itPos = ui->tblSalary->item(row, colPositionId);

        const int staffId = itStaff ? itStaff->text().toInt() : 0;
        const int posId = itPos ? itPos->text().toInt() : 0;

        const double fixed = qAbs(cellAmount(ui->tblSalary, row, colFixed));
        const double calculated = qAbs(cellAmount(ui->tblSalary, row, colCalculated));
        const double bonus = qAbs(cellAmount(ui->tblSalary, row, colBonus));
        const double credit = fixed + calculated + bonus;

        if (staffId <= 0) {
            ++rowsNoStaff;
            continue;
        }
        if (credit <= 0.001) {
            ++rowsNoAmount;
            continue;
        }

        QJsonObject jitem;
        jitem[QStringLiteral("f_staff")] = staffId;
        jitem[QStringLiteral("f_position")] = posId; // 0 = server uses s_user.f_group
        jitem[QStringLiteral("f_fixed")] = fixed;
        jitem[QStringLiteral("f_calculated")] = calculated;
        jitem[QStringLiteral("f_bonus")] = bonus;
        jitem[QStringLiteral("f_amount_credit")] = credit;
        items.push_back(jitem);
    }

    if (items.isEmpty()) {
        QString msg = tr("Nothing to save.");
        if (rowsNoAmount > 0) {
            msg += QStringLiteral("<br>") + tr("%n row(s): amount is zero.", nullptr, rowsNoAmount);
        }
        if (rowsNoStaff > 0) {
            msg += QStringLiteral("<br>") + tr("%n row(s): employee is not set.", nullptr, rowsNoStaff);
        }
        if (rowsNoAmount == 0 && rowsNoStaff == 0) {
            msg += QStringLiteral("<br>") + tr("Add at least one employee with amounts.");
        }
        C5Message::error(msg);
        return;
    }

    QJsonObject params;
    params[QStringLiteral("date")] = ui->deDate->date().toString(QStringLiteral("yyyy-MM-dd"));
    params[QStringLiteral("f_type")] = kSalaryTypeAccrual;
    params[QStringLiteral("items")] = items;

    NInterface::query1(QStringLiteral("/engine/v2/officen/salary/save"),
                       mUser->mSessionKey,
                       this,
                       params,
                       [this](const QJsonObject &) {
                           C5Message::info(tr("Saved"));
                           recalcTotal();
                       });
}

void C5SalaryEditor::removeDocument()
{
    ui->tblSalary->setRowCount(0);
    recalcTotal();
}

void C5SalaryEditor::print()
{
    QTextDocument doc;
    doc.setHtml(buildPrintHtml());

    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog dlg(&printer, this);
    if (dlg.exec() == QDialog::Accepted) {
        doc.print(&printer);
    }
}

void C5SalaryEditor::initTable()
{
    ui->tblSalary->setRowCount(0);
    ui->tblSalary->setColumnCount(11);
    ui->tblSalary->setHorizontalHeaderLabels({
        QStringLiteral("DbId"),
        QStringLiteral("PositionId"),
        QStringLiteral("StaffId"),
        QStringLiteral("#"),
        tr("Position"),
        tr("Name"),
        tr("Fixed"),
        tr("Dish sales base"),
        tr("Calculated"),
        tr("Bonus"),
        tr("Total"),
    });
    ui->tblSalary->setColumnHidden(colDbId, true);
    ui->tblSalary->setColumnHidden(colPositionId, true);
    ui->tblSalary->setColumnHidden(colStaffId, true);
    ui->tblSalary->horizontalHeader()->setSectionResizeMode(colNum, QHeaderView::ResizeToContents);
    ui->tblSalary->horizontalHeader()->setSectionResizeMode(colPosition, QHeaderView::Stretch);
    ui->tblSalary->horizontalHeader()->setSectionResizeMode(colName, QHeaderView::Stretch);
    ui->tblSalary->horizontalHeader()->setSectionResizeMode(colFixed, QHeaderView::ResizeToContents);
    ui->tblSalary->horizontalHeader()->setSectionResizeMode(colDishBase, QHeaderView::ResizeToContents);
    ui->tblSalary->horizontalHeader()->setSectionResizeMode(colCalculated, QHeaderView::ResizeToContents);
    ui->tblSalary->horizontalHeader()->setSectionResizeMode(colBonus, QHeaderView::ResizeToContents);
    ui->tblSalary->horizontalHeader()->setSectionResizeMode(colTotal, QHeaderView::ResizeToContents);
    ui->tblSalary->verticalHeader()->setVisible(false);
    ui->tblSalary->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void C5SalaryEditor::updateRowTotal(int row)
{
    const double total = qAbs(cellAmount(ui->tblSalary, row, colFixed))
                         + qAbs(cellAmount(ui->tblSalary, row, colCalculated))
                         + qAbs(cellAmount(ui->tblSalary, row, colBonus));

    QSignalBlocker sb(ui->tblSalary);
    auto *itTotal = ui->tblSalary->item(row, colTotal);
    if (!itTotal) {
        itTotal = new QTableWidgetItem();
        itTotal->setFlags(itTotal->flags() & ~Qt::ItemIsEditable);
        itTotal->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->tblSalary->setItem(row, colTotal, itTotal);
    }
    itTotal->setText(QString::number(total, 'f', 2));

    recalcTotal();
}

void C5SalaryEditor::recalcTotal()
{
    double total = 0;
    for (int i = 0; i < ui->tblSalary->rowCount(); ++i) {
        total += qAbs(cellAmount(ui->tblSalary, i, colTotal));
    }
    ui->leTotalAmount->setText(QString::number(total, 'f', 2));
}

QString C5SalaryEditor::buildPrintHtml() const
{
    QString html;
    html += QStringLiteral("<html><head><meta charset='utf-8'>"
                           "<style>"
                           "table{width:100%;border-collapse:collapse;}"
                           "th,td{border:1px solid #000;padding:6px;}"
                           "th{text-align:center;}"
                           "td.num{text-align:center;}"
                           "td.money{text-align:right;}"
                           "</style></head><body>");
    html += QStringLiteral("<h2>") + tr("Salary accrual").toHtmlEscaped() + QStringLiteral("</h2>");
    html += QStringLiteral("<p>") + tr("Date: %1").arg(ui->deDate->date().toString(QStringLiteral("dd.MM.yyyy"))).toHtmlEscaped()
            + QStringLiteral("</p>");
    html += QStringLiteral("<table><thead><tr>"
                           "<th>#</th><th>") + tr("Position").toHtmlEscaped() + QStringLiteral("</th><th>")
            + tr("Name").toHtmlEscaped() + QStringLiteral("</th><th>") + tr("Fixed").toHtmlEscaped()
            + QStringLiteral("</th><th>") + tr("Dish sales base").toHtmlEscaped()
            + QStringLiteral("</th><th>") + tr("Calculated").toHtmlEscaped() + QStringLiteral("</th><th>")
            + tr("Bonus").toHtmlEscaped() + QStringLiteral("</th><th>") + tr("Total").toHtmlEscaped()
            + QStringLiteral("</th></tr></thead><tbody>");

    for (int i = 0; i < ui->tblSalary->rowCount(); ++i) {
        html += QStringLiteral("<tr>");
        html += QStringLiteral("<td class='num'>") + QString::number(i + 1).toHtmlEscaped() + QStringLiteral("</td>");
        html += QStringLiteral("<td>") + (ui->tblSalary->item(i, colPosition) ? ui->tblSalary->item(i, colPosition)->text().toHtmlEscaped() : QString()) + QStringLiteral("</td>");
        html += QStringLiteral("<td>") + (ui->tblSalary->item(i, colName) ? ui->tblSalary->item(i, colName)->text().toHtmlEscaped() : QString()) + QStringLiteral("</td>");
        html += QStringLiteral("<td class='money'>") + (ui->tblSalary->item(i, colFixed) ? ui->tblSalary->item(i, colFixed)->text().toHtmlEscaped() : QStringLiteral("0.00")) + QStringLiteral("</td>");
        html += QStringLiteral("<td class='money'>") + (ui->tblSalary->item(i, colDishBase) ? ui->tblSalary->item(i, colDishBase)->text().toHtmlEscaped() : QStringLiteral("0.00")) + QStringLiteral("</td>");
        html += QStringLiteral("<td class='money'>") + (ui->tblSalary->item(i, colCalculated) ? ui->tblSalary->item(i, colCalculated)->text().toHtmlEscaped() : QStringLiteral("0.00")) + QStringLiteral("</td>");
        html += QStringLiteral("<td class='money'>") + (ui->tblSalary->item(i, colBonus) ? ui->tblSalary->item(i, colBonus)->text().toHtmlEscaped() : QStringLiteral("0.00")) + QStringLiteral("</td>");
        html += QStringLiteral("<td class='money'>") + (ui->tblSalary->item(i, colTotal) ? ui->tblSalary->item(i, colTotal)->text().toHtmlEscaped() : QStringLiteral("0.00")) + QStringLiteral("</td>");
        html += QStringLiteral("</tr>");
    }

    html += QStringLiteral("</tbody></table>");
    html += QStringLiteral("<p><b>") + tr("Total accrued: %1").arg(ui->leTotalAmount->text()).toHtmlEscaped() + QStringLiteral("</b></p>");
    html += QStringLiteral("</body></html>");
    return html;
}
