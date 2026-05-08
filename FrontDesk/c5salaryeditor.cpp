#include "c5salaryeditor.h"
#include "ui_c5salaryeditor.h"

#include <QDate>
#include <QHeaderView>
#include <QMessageBox>
#include <QPrintDialog>
#include <QPrinter>
#include <QTextDocument>
#include <QtMath>
#include "c5codenameselectorfunctions.h"
#include "c5message.h"
#include "c5user.h"
#include "c5utils.h"
#include "ninterface.h"

C5SalaryEditor::C5SalaryEditor(QWidget *parent)
    : C5Widget(parent)
    , ui(new Ui::C5SalaryEditor)
{
    ui->setupUi(this);
    fLabel = tr("Salary editor");
    fIcon = QIcon(":/salary.png");
    ui->deDate->setDate(QDate::currentDate());
    initTable();
    recalcTotal();

    connect(ui->tblSalary, &QTableWidget::cellChanged, this, [this](int, int) {
        recalcTotal();
    });

    connect(ui->tblSalary, &QTableWidget::cellDoubleClicked, this, [this](int row, int column) {
        if(column != colPosition) {
            return;
        }

        ui->tblSalary->setCurrentCell(row, column);
        on_btnChangePosition_clicked();
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
        fToolBar->addAction(QIcon(":/recycle.png"), tr("Remove"), this, SLOT(removeDocument()));
        fToolBar->addAction(QIcon(":/print.png"), tr("Print"), this, SLOT(print()));
    }

    return fToolBar;
}

void C5SalaryEditor::open(const QDate &date, int type)
{
    ui->deDate->setDate(date);
    ui->cbType->setCurrentIndex(qMax(0, type - 1));
    mOpenDate = date;
    mOpenType = type;
    mLoadOnShow = true;

    if(isVisible()) {
        // If widget is already shown (rare), load immediately.
        showEvent(nullptr);
    }
}

void C5SalaryEditor::showEvent(QShowEvent *e)
{
    if(e) {
        QWidget::showEvent(e);
    }

    if(!mLoadOnShow) {
        return;
    }

    if(e && e->spontaneous()) {
        // Only load when shown because of our open()/user action.
        return;
    }

    mLoadOnShow = false;

    const int f_type = ui->cbType->currentIndex() + 1;
    const QString dateStr = mOpenDate.isValid() ? mOpenDate.toString("yyyy-MM-dd") : ui->deDate->date().toString("yyyy-MM-dd");

    QSignalBlocker sb(ui->tblSalary);
    ui->tblSalary->setRowCount(0);
    ui->leTotalAmount->setText("0.00");

    NInterface::query1("/engine/v2/officen/salary/open",
                       mUser->mSessionKey,
                       this,
                       {{"date", dateStr}, {"f_type", f_type}},
                       [this, f_type](const QJsonObject &jo) {
                           QSignalBlocker sb2(ui->tblSalary);

                           const QJsonArray items = jo.value("items").toArray();
                           ui->tblSalary->setRowCount(0);

                           int row = 0;
                           for (const auto &v : items) {
                               const QJsonObject it = v.toObject();

                               const int staffId = it.value("f_staff").toInt();
                               const int positionId = it.value("f_position").toInt();
                               const QString staffName = it.value("f_staff_name").toString();
                               const QString positionName = it.value("f_position_name").toString();
                               const double credit = it.value("f_amount_credit").toString().toDouble();
                               const double debit = it.value("f_amount_debit").toString().toDouble();

                               const double amount = (f_type == 1) ? credit : debit;

                               ui->tblSalary->insertRow(row);

                               auto *itDbId = new QTableWidgetItem(QString::number(it.value("f_id").toInt()));
                               auto *itPositionId = new QTableWidgetItem(QString::number(positionId));
                               auto *itStaffId = new QTableWidgetItem(QString::number(staffId));
                               auto *itNum = new QTableWidgetItem(QString::number(row + 1));
                               auto *itPos = new QTableWidgetItem(positionName);
                               auto *itName = new QTableWidgetItem(staffName);
                               auto *itAmount = new QTableWidgetItem(QString::number(amount, 'f', 2));

                               itDbId->setFlags(itDbId->flags() & ~Qt::ItemIsEditable);
                               itPositionId->setFlags(itPositionId->flags() & ~Qt::ItemIsEditable);
                               itStaffId->setFlags(itStaffId->flags() & ~Qt::ItemIsEditable);
                               itNum->setFlags(itNum->flags() & ~Qt::ItemIsEditable);
                               itPos->setFlags(itPos->flags() & ~Qt::ItemIsEditable);
                               itName->setFlags(itName->flags() & ~Qt::ItemIsEditable);

                               itNum->setTextAlignment(Qt::AlignCenter);
                               itAmount->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

                               ui->tblSalary->setItem(row, colDbId, itDbId);
                               ui->tblSalary->setItem(row, colPositionId, itPositionId);
                               ui->tblSalary->setItem(row, colStaffId, itStaffId);
                               ui->tblSalary->setItem(row, colNum, itNum);
                               ui->tblSalary->setItem(row, colPosition, itPos);
                               ui->tblSalary->setItem(row, colName, itName);
                               ui->tblSalary->setItem(row, colAmount, itAmount);

                               ++row;
                           }

                           recalcTotal();
                       });
}

void C5SalaryEditor::on_btnAddStaff_clicked()
{
    const auto employees = selectItem<StructEmployee>(true, false, ui->btnAddStaff->mapToGlobal(QPoint(0, ui->btnAddStaff->height())));
    if(employees.isEmpty()) {
        return;
    }
    const auto &employee = employees.first();
    const QString employeeName = QString("%1 %2").arg(employee.firstName, employee.lastName).trimmed().isEmpty()
                                     ? employee.login
                                     : QString("%1 %2").arg(employee.firstName, employee.lastName).trimmed();

    // Default position comes from the selected employee (employee.groupId/groupName)
    const int positionId = employee.groupId;
    QString positionName = employee.groupName;
    if(positionName.isEmpty()) {
        positionName = tr("No position");
    }

    const int row = ui->tblSalary->rowCount();
    ui->tblSalary->insertRow(row);

    auto *itDbId = new QTableWidgetItem();
    auto *itPositionId = new QTableWidgetItem();
    auto *itStaffId = new QTableWidgetItem();
    auto *itNum = new QTableWidgetItem(QString::number(row + 1));
    auto *itPos = new QTableWidgetItem();
    auto *itName = new QTableWidgetItem();
    auto *itAmount = new QTableWidgetItem("0.00");

    itDbId->setText("0");
    itPositionId->setText(QString::number(positionId));
    itStaffId->setText(QString::number(employee.id));
    itDbId->setFlags(itDbId->flags() & ~Qt::ItemIsEditable);
    itPositionId->setFlags(itPositionId->flags() & ~Qt::ItemIsEditable);
    itStaffId->setFlags(itStaffId->flags() & ~Qt::ItemIsEditable);
    itNum->setFlags(itNum->flags() & ~Qt::ItemIsEditable);
    itPos->setFlags(itPos->flags() & ~Qt::ItemIsEditable);
    itName->setFlags(itName->flags() & ~Qt::ItemIsEditable);
    itNum->setTextAlignment(Qt::AlignCenter);
    itAmount->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

    ui->tblSalary->setItem(row, colDbId, itDbId);
    ui->tblSalary->setItem(row, colPositionId, itPositionId);
    ui->tblSalary->setItem(row, colStaffId, itStaffId);
    ui->tblSalary->setItem(row, colNum, itNum);
    ui->tblSalary->setItem(row, colPosition, itPos);
    ui->tblSalary->setItem(row, colName, itName);
    ui->tblSalary->setItem(row, colAmount, itAmount);
    itPos->setText(positionName);
    itName->setText(employeeName);
    ui->tblSalary->setCurrentCell(row, colAmount);
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
    if(auto *it = ui->tblSalary->item(row, colPosition)) {
        const QRect r = ui->tblSalary->visualItemRect(it);
        selectorPos = ui->tblSalary->viewport()->mapToGlobal(QPoint(r.left(), r.bottom()));
    }

    const auto groups = selectItem<StructEmployeeGroup>(true, false, selectorPos);
    if(groups.isEmpty()) {
        return;
    }
    const int positionId = groups.first().id;
    const QString positionName = groups.first().name;

    auto *itId = ui->tblSalary->item(row, colPositionId);
    if(!itId) {
        itId = new QTableWidgetItem();
        itId->setFlags(itId->flags() & ~Qt::ItemIsEditable);
        ui->tblSalary->setItem(row, colPositionId, itId);
    }
    itId->setText(QString::number(positionId));

    auto *itName = ui->tblSalary->item(row, colPosition);
    if(!itName) {
        itName = new QTableWidgetItem();
        ui->tblSalary->setItem(row, colPosition, itName);
    }
    itName->setFlags(itName->flags() & ~Qt::ItemIsEditable);
    itName->setText(positionName);
}

void C5SalaryEditor::saveDocument()
{
    const int f_type = ui->cbType->currentIndex() + 1; // 1=accrual, 2=payment/withholding
    QJsonArray items;

    for(int row = 0; row < ui->tblSalary->rowCount(); ++row) {
        const auto *itStaff = ui->tblSalary->item(row, colStaffId);
        const auto *itPos = ui->tblSalary->item(row, colPositionId);
        const auto *itAmount = ui->tblSalary->item(row, colAmount);

        const int staffId = itStaff ? itStaff->text().toInt() : 0;
        const int posId = itPos ? itPos->text().toInt() : 0;
        const double amount = itAmount ? str_float(itAmount->text()) : 0.0;

        if(staffId <= 0 || posId <= 0) {
            continue;
        }

        const double absAmount = qAbs(amount);
        const double credit = (f_type == 1) ? absAmount : 0.0;
        const double debit = (f_type == 2) ? absAmount : 0.0;

        QJsonObject jitem;
        jitem["f_staff"] = staffId;
        jitem["f_position"] = posId;
        jitem["f_amount_credit"] = credit;
        jitem["f_amount_debit"] = debit;
        items.push_back(jitem);
    }

    if(items.isEmpty()) {
        C5Message::error(tr("Nothing to save (empty rows)."));
        return;
    }

    const QDate d = ui->deDate->date();
    QJsonObject params;
    params["date"] = d.toString("yyyy-MM-dd");
    params["f_type"] = f_type;
    params["items"] = items;

    NInterface::query1("/engine/v2/officen/salary/save",
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
    ui->tblSalary->setColumnCount(7);
    ui->tblSalary->setColumnHidden(colDbId, true);
    ui->tblSalary->setColumnHidden(colPositionId, true);
    ui->tblSalary->setColumnHidden(colStaffId, true);
    ui->tblSalary->horizontalHeader()->setSectionResizeMode(colNum, QHeaderView::ResizeToContents);
    ui->tblSalary->horizontalHeader()->setSectionResizeMode(colPosition, QHeaderView::Stretch);
    ui->tblSalary->horizontalHeader()->setSectionResizeMode(colName, QHeaderView::Stretch);
    ui->tblSalary->horizontalHeader()->setSectionResizeMode(colAmount, QHeaderView::ResizeToContents);
    ui->tblSalary->verticalHeader()->setVisible(false);
    ui->tblSalary->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void C5SalaryEditor::recalcTotal()
{
    double total = 0;
    for (int i = 0; i < ui->tblSalary->rowCount(); ++i) {
        const auto *itAmount = ui->tblSalary->item(i, colAmount);
        total += itAmount ? qAbs(itAmount->text().toDouble()) : 0;
    }
    ui->leTotalAmount->setText(QString::number(total, 'f', 2));
}

QString C5SalaryEditor::buildPrintHtml() const
{
    QString html;
    html += "<html><head><meta charset='utf-8'>"
            "<style>"
            "table{width:100%;border-collapse:collapse;}"
            "th,td{border:1px solid #000;padding:6px;}"
            "th{text-align:center;}"
            "td.num{text-align:center;}"
            "td.money{text-align:right;}"
            "</style></head><body>";

    const int f_type = ui->cbType->currentIndex() + 1;
    const QString modeTitle = (f_type == 1) ? tr("Salary accrual") : tr("Salary payment / withholding");
    html += "<h2>" + modeTitle + "</h2>";
    html += "<p>Date: " + ui->deDate->date().toString("dd.MM.yyyy") + "</p>";
    html += "<table><thead><tr>"
            "<th>#</th><th>Position</th><th>Name</th><th>Amount</th>"
            "</tr></thead><tbody>";

    for (int i = 0; i < ui->tblSalary->rowCount(); ++i) {
        const QString pos = ui->tblSalary->item(i, colPosition) ? ui->tblSalary->item(i, colPosition)->text() : "";
        const QString name = ui->tblSalary->item(i, colName) ? ui->tblSalary->item(i, colName)->text() : "";
        const QString amount = ui->tblSalary->item(i, colAmount) ? ui->tblSalary->item(i, colAmount)->text() : "0.00";
        html += "<tr>";
        html += "<td class='num'>" + QString::number(i + 1) + "</td>";
        html += "<td>" + pos.toHtmlEscaped() + "</td>";
        html += "<td>" + name.toHtmlEscaped() + "</td>";
        html += "<td class='money'>" + amount.toHtmlEscaped() + "</td>";
        html += "</tr>";
    }

    html += "</tbody></table>";
    html += "<p><b>Total: " + ui->leTotalAmount->text().toHtmlEscaped() + "</b></p>";
    html += "</body></html>";
    return html;
}
