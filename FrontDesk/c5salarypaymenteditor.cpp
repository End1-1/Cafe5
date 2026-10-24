#include "c5salarypaymenteditor.h"
#include "ui_c5salarypaymenteditor.h"

#include <QDate>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonObject>
#include <QPrintDialog>
#include <QPrinter>
#include <QSet>
#include <QSignalBlocker>
#include <QTextDocument>
#include <QVector>

#include "c5cache.h"
#include "c5codenameselectorfunctions.h"
#include "c5config.h"
#include "c5message.h"
#include "c5user.h"
#include "c5utils.h"
#include "dict_payment_type.h"
#include "ninterface.h"
#include "paymenttypenames.h"

namespace {

constexpr int kSalaryTypePayment = 2;

const QString kRegCashbox = QStringLiteral("salary_payment_cashbox");
const QString kRegCashboxName = QStringLiteral("salary_payment_cashbox_name");
const QString kRegPaymentType = QStringLiteral("salary_payment_type");

} // namespace

C5SalaryPaymentEditor::C5SalaryPaymentEditor(QWidget *parent)
    : C5Widget(parent)
    , ui(new Ui::C5SalaryPaymentEditor)
{
    ui->setupUi(this);
    fLabel = tr("Salary payment");
    fIcon = QIcon(":/salary.png");
    ui->deDate->setDate(QDate::currentDate());
    mOpenDate = ui->deDate->date();
    mLoadOnShow = true;
    initTable();
    initPaymentSource();
    recalcTotal();

    connect(ui->tblSalary, &QTableWidget::cellChanged, this, [this](int, int) {
        recalcTotal();
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

C5SalaryPaymentEditor::~C5SalaryPaymentEditor()
{
    delete ui;
}

QToolBar *C5SalaryPaymentEditor::toolBar()
{
    if (!fToolBar) {
        createToolBar();
        fToolBar->addAction(QIcon(":/save.png"), tr("Save"), this, SLOT(saveDocument()));
        fToolBar->addAction(QIcon(":/recycle.png"), tr("Remove"), this, SLOT(removeDocument()));
        fToolBar->addAction(QIcon(":/print.png"), tr("Print"), this, SLOT(print()));
    }

    return fToolBar;
}

void C5SalaryPaymentEditor::open(const QDate &date)
{
    QSignalBlocker sb(ui->deDate);
    ui->deDate->setDate(date);
    mOpenDate = date;
    mLoadOnShow = true;

    if (isVisible()) {
        showEvent(nullptr);
    }
}

void C5SalaryPaymentEditor::showEvent(QShowEvent *e)
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

    if (!mPaymentNamesLoaded) {
        mPaymentNamesLoaded = true;
        PaymentTypeNames::load(mUser->mSessionKey, this, [this]() {
            rebuildPaymentCombo();
        });
    }

    const QString dateStr = mOpenDate.isValid() ? mOpenDate.toString(QStringLiteral("yyyy-MM-dd"))
                                                : ui->deDate->date().toString(QStringLiteral("yyyy-MM-dd"));

    QSignalBlocker sb(ui->tblSalary);
    ui->tblSalary->setRowCount(0);
    ui->leTotalAmount->setText(QStringLiteral("0.00"));

    const QDate openDate = mOpenDate.isValid() ? mOpenDate : ui->deDate->date();

    NInterface::query1(QStringLiteral("/engine/v2/officen/salary/open"),
                       mUser->mSessionKey,
                       this,
                       {{QStringLiteral("date"), dateStr}, {QStringLiteral("f_type"), kSalaryTypePayment}},
                       [this, openDate](const QJsonObject &jo) {
                           QSignalBlocker sb2(ui->tblSalary);

                           applyPaymentSource(jo.value(QStringLiteral("payment")).toObject());
                           const QJsonArray items = jo.value(QStringLiteral("items")).toArray();
                           ui->tblSalary->setRowCount(0);

                           int row = 0;
                           for (const auto &v : items) {
                               const QJsonObject it = v.toObject();
                               const double amount = it.value(QStringLiteral("f_amount_debit")).toString().toDouble();
                               const double debt = it.value(QStringLiteral("f_debt")).toString().toDouble();

                               ui->tblSalary->insertRow(row);

                               auto *itDbId = new QTableWidgetItem(QString::number(it.value(QStringLiteral("f_id")).toInt()));
                               auto *itPositionId = new QTableWidgetItem(QString::number(it.value(QStringLiteral("f_position")).toInt()));
                               auto *itStaffId = new QTableWidgetItem(QString::number(it.value(QStringLiteral("f_staff")).toInt()));
                               auto *itNum = new QTableWidgetItem(QString::number(row + 1));
                               auto *itPos = new QTableWidgetItem(it.value(QStringLiteral("f_position_name")).toString());
                               auto *itName = new QTableWidgetItem(it.value(QStringLiteral("f_staff_name")).toString());
                               auto *itDebt = new QTableWidgetItem(QString::number(debt, 'f', 2));
                               auto *itAmount = new QTableWidgetItem(QString::number(amount, 'f', 2));

                               itDbId->setFlags(itDbId->flags() & ~Qt::ItemIsEditable);
                               itPositionId->setFlags(itPositionId->flags() & ~Qt::ItemIsEditable);
                               itStaffId->setFlags(itStaffId->flags() & ~Qt::ItemIsEditable);
                               itNum->setFlags(itNum->flags() & ~Qt::ItemIsEditable);
                               itPos->setFlags(itPos->flags() & ~Qt::ItemIsEditable);
                               itName->setFlags(itName->flags() & ~Qt::ItemIsEditable);
                               itDebt->setFlags(itDebt->flags() & ~Qt::ItemIsEditable);

                               itNum->setTextAlignment(Qt::AlignCenter);
                               itDebt->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
                               itAmount->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

                               ui->tblSalary->setItem(row, colDbId, itDbId);
                               ui->tblSalary->setItem(row, colPositionId, itPositionId);
                               ui->tblSalary->setItem(row, colStaffId, itStaffId);
                               ui->tblSalary->setItem(row, colNum, itNum);
                               ui->tblSalary->setItem(row, colPosition, itPos);
                               ui->tblSalary->setItem(row, colName, itName);
                               ui->tblSalary->setItem(row, colDebt, itDebt);
                               ui->tblSalary->setItem(row, colAmount, itAmount);

                               ++row;
                           }

                           recalcTotal();

                           if (items.isEmpty()) {
                               offerAutofillFromAccrual(openDate);
                           }
                       });
}

void C5SalaryPaymentEditor::offerAutofillFromAccrual(const QDate &date)
{
    if (!date.isValid() || date != QDate::currentDate()) {
        return;
    }

    if (ui->tblSalary->rowCount() > 0) {
        return;
    }

    if (C5Message::question(tr("Fill the payment document with employees from salary accrual for today?"))
            != QDialog::Accepted) {
        return;
    }

    fillFromAccrual(date);
}

void C5SalaryPaymentEditor::fillFromAccrual(const QDate &date)
{
    if (!date.isValid()) {
        return;
    }

    const QString dateStr = date.toString(QStringLiteral("yyyy-MM-dd"));

    NInterface::query1(QStringLiteral("/engine/v2/officen/salary/open"),
                       mUser->mSessionKey,
                       this,
                       {{QStringLiteral("date"), dateStr}, {QStringLiteral("f_type"), 1}},
                       [this](const QJsonObject &jo) {
                           const QJsonArray items = jo.value(QStringLiteral("items")).toArray();
                           if (items.isEmpty()) {
                               C5Message::info(tr("No salary accrual entries found for this date."));
                               return;
                           }

                           QSignalBlocker sb(ui->tblSalary);
                           QSet<int> seenStaff;

                           for (const QJsonValue &v : items) {
                               const QJsonObject it = v.toObject();
                               const int staffId = it.value(QStringLiteral("f_staff")).toInt();
                               if (staffId <= 0 || seenStaff.contains(staffId)) {
                                   continue;
                               }

                               seenStaff.insert(staffId);
                               const int positionId = it.value(QStringLiteral("f_position")).toInt();
                               const QString staffName = it.value(QStringLiteral("f_staff_name")).toString().trimmed();
                               const QString positionName = it.value(QStringLiteral("f_position_name")).toString().trimmed();
                               const double debt = it.value(QStringLiteral("f_debt")).toVariant().toDouble();

                               appendStaffRow(staffId, staffName, positionId, positionName, debt);
                           }

                           recalcTotal();
                       });
}

void C5SalaryPaymentEditor::appendStaffRow(int staffId, const QString &staffName, int positionId, const QString &positionName, double debt)
{
    const int row = ui->tblSalary->rowCount();
    ui->tblSalary->insertRow(row);

    auto *itDbId = new QTableWidgetItem(QStringLiteral("0"));
    auto *itPositionId = new QTableWidgetItem(QString::number(positionId));
    auto *itStaffId = new QTableWidgetItem(QString::number(staffId));
    auto *itNum = new QTableWidgetItem(QString::number(row + 1));
    auto *itPos = new QTableWidgetItem(positionName);
    auto *itName = new QTableWidgetItem(staffName);
    auto *itDebt = new QTableWidgetItem(QString::number(debt, 'f', 2));
    auto *itAmount = new QTableWidgetItem(QStringLiteral("0.00"));

    itDbId->setFlags(itDbId->flags() & ~Qt::ItemIsEditable);
    itPositionId->setFlags(itPositionId->flags() & ~Qt::ItemIsEditable);
    itStaffId->setFlags(itStaffId->flags() & ~Qt::ItemIsEditable);
    itNum->setFlags(itNum->flags() & ~Qt::ItemIsEditable);
    itPos->setFlags(itPos->flags() & ~Qt::ItemIsEditable);
    itName->setFlags(itName->flags() & ~Qt::ItemIsEditable);
    itDebt->setFlags(itDebt->flags() & ~Qt::ItemIsEditable);
    itNum->setTextAlignment(Qt::AlignCenter);
    itDebt->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    itAmount->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

    ui->tblSalary->setItem(row, colDbId, itDbId);
    ui->tblSalary->setItem(row, colPositionId, itPositionId);
    ui->tblSalary->setItem(row, colStaffId, itStaffId);
    ui->tblSalary->setItem(row, colNum, itNum);
    ui->tblSalary->setItem(row, colPosition, itPos);
    ui->tblSalary->setItem(row, colName, itName);
    ui->tblSalary->setItem(row, colDebt, itDebt);
    ui->tblSalary->setItem(row, colAmount, itAmount);
    ui->tblSalary->setCurrentCell(row, colAmount);
}

void C5SalaryPaymentEditor::on_btnAddStaff_clicked()
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
                           const double debt = jo.value(QStringLiteral("f_debt")).toVariant().toDouble();

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

                           appendStaffRow(employee.id, employeeName, positionId, positionName, debt);
                       });
}

void C5SalaryPaymentEditor::on_btnRemoveStaff_clicked()
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

void C5SalaryPaymentEditor::on_btnChangePosition_clicked()
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

void C5SalaryPaymentEditor::saveDocument()
{
    commit_table_edits(ui->tblSalary);

    QJsonArray items;
    int rowsNoStaff = 0;
    int rowsNoAmount = 0;

    for (int row = 0; row < ui->tblSalary->rowCount(); ++row) {
        const auto *itStaff = ui->tblSalary->item(row, colStaffId);
        const auto *itPos = ui->tblSalary->item(row, colPositionId);
        const auto *itAmount = ui->tblSalary->item(row, colAmount);

        const int staffId = itStaff ? itStaff->text().toInt() : 0;
        const int posId = itPos ? itPos->text().toInt() : 0;
        const double amount = itAmount ? qAbs(str_table_amount(itAmount->text())) : 0.0;

        if (staffId <= 0) {
            ++rowsNoStaff;
            continue;
        }
        if (amount <= 0.001) {
            ++rowsNoAmount;
            continue;
        }

        QJsonObject jitem;
        jitem[QStringLiteral("f_staff")] = staffId;
        jitem[QStringLiteral("f_position")] = posId; // 0 = server uses s_user.f_group
        jitem[QStringLiteral("f_amount_debit")] = amount;
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

    const int cashboxId = mCurrentPayment.isUnpaid() ? 0 : mCurrentPayment.cashboxId;
    const int paymentTypeId = mCurrentPayment.isUnpaid() ? PAYMENT_TYPE_CASH : mCurrentPayment.paymentTypeId;

    QJsonObject params;
    params[QStringLiteral("date")] = ui->deDate->date().toString(QStringLiteral("yyyy-MM-dd"));
    params[QStringLiteral("f_type")] = kSalaryTypePayment;
    params[QStringLiteral("items")] = items;
    params[QStringLiteral("cashbox_id")] = cashboxId;
    params[QStringLiteral("payment_type_id")] = paymentTypeId;

    __c5config.setRegValue(kRegCashbox, cashboxId);
    __c5config.setRegValue(kRegCashboxName, mCurrentPayment.cashboxName);
    __c5config.setRegValue(kRegPaymentType, paymentTypeId);

    sendSave(params);
}

void C5SalaryPaymentEditor::sendSave(QJsonObject params)
{
    NInterface::query1(QStringLiteral("/engine/v2/officen/salary/save"),
                       mUser->mSessionKey,
                       this,
                       params,
                       [this, params](const QJsonObject &jo) mutable {
                           if (jo.value(QStringLiteral("need_confirm")).toBool()) {
                               const double balance = jo.value(QStringLiteral("balance")).toVariant().toDouble();
                               const double total = jo.value(QStringLiteral("total")).toVariant().toDouble();
                               if (C5Message::question(tr("Not enough money in the cashbox: balance %1, payment %2. Save anyway?")
                                                           .arg(float_str(balance, 2), float_str(total, 2)))
                                       != QDialog::Accepted) {
                                   return;
                               }
                               params[QStringLiteral("confirm_low_balance")] = 1;
                               sendSave(params);
                               return;
                           }
                           C5Message::info(tr("Saved"));
                           recalcTotal();
                       });
}

void C5SalaryPaymentEditor::removeDocument()
{
    const QDate date = mOpenDate.isValid() ? mOpenDate : ui->deDate->date();
    if (C5Message::question(tr("Remove the salary payment document for %1? Its cashbox operations will be removed too.")
                                .arg(date.toString(QStringLiteral("dd.MM.yyyy"))))
            != QDialog::Accepted) {
        return;
    }

    NInterface::query1(QStringLiteral("/engine/v2/officen/salary/remove-document"),
                       mUser->mSessionKey,
                       this,
                       {{QStringLiteral("date"), date.toString(QStringLiteral("yyyy-MM-dd"))},
                        {QStringLiteral("f_type"), kSalaryTypePayment}},
                       [this](const QJsonObject &) {
                           QSignalBlocker sb(ui->tblSalary);
                           ui->tblSalary->setRowCount(0);
                           recalcTotal();
                       });
}

void C5SalaryPaymentEditor::initPaymentSource()
{
    mPaymentOptions.settingsGroup = QStringLiteral("SalaryPayment");
    mPaymentOptions.load();
    mPaymentPresets = mPaymentOptions.buildPresets(loadCashboxList());
    applyPaymentSource(QJsonObject());
}

QVector<QPair<int, QString>> C5SalaryPaymentEditor::loadCashboxList() const
{
    QVector<QPair<int, QString>> out;
    C5Cache *cache = C5Cache::cache(cache_cash_names);
    if (!cache) {
        return out;
    }
    for (int r = 0; r < cache->rowCount(); ++r) {
        const int id = cache->getInt(r, 0);
        if (id > 0) {
            out.append({id, cache->getString(r, 1)});
        }
    }
    return out;
}

void C5SalaryPaymentEditor::applyPaymentSource(const QJsonObject &payment)
{
    StorePaymentPreset preset;
    if (payment.value(QStringLiteral("exists")).toBool()) {
        preset.cashboxId = payment.value(QStringLiteral("f_cashbox_id")).toVariant().toInt();
        preset.cashboxName = payment.value(QStringLiteral("f_cashbox_name")).toString();
        preset.paymentTypeId = payment.value(QStringLiteral("f_payment_type_id")).toVariant().toInt();
    } else {
        preset.cashboxId = __c5config.getRegValue(kRegCashbox, 0).toInt();
        preset.cashboxName = __c5config.getRegValue(kRegCashboxName).toString();
        preset.paymentTypeId = __c5config.getRegValue(kRegPaymentType, PAYMENT_TYPE_CASH).toInt();
    }
    selectPayment(preset);
}

void C5SalaryPaymentEditor::selectPayment(const StorePaymentPreset &preset)
{
    mPaymentPresets = mPaymentOptions.buildPresets(loadCashboxList());
    mCurrentPayment = StorePaymentPreset();
    if (!preset.isUnpaid()) {
        for (const StorePaymentPreset &p : mPaymentPresets) {
            if (p.cashboxId == preset.cashboxId && p.paymentTypeId == preset.paymentTypeId) {
                mCurrentPayment = p;
                break;
            }
        }
        if (mCurrentPayment.isUnpaid()) {
            // Saved document keeps its source visible even if it is no longer in the settings.
            mCurrentPayment = preset;
            mCurrentPayment.paymentTypeName = PaymentTypeNames::title(preset.paymentTypeId);
            mCurrentPayment.currencyId = 1;
            mPaymentPresets.append(mCurrentPayment);
        }
    }
    rebuildPaymentCombo();
}

void C5SalaryPaymentEditor::rebuildPaymentCombo()
{
    const QSignalBlocker blocker(ui->cbPayment);
    ui->cbPayment->clear();
    const bool showUnpaid = mPaymentOptions.unpaidEnabled;
    if (showUnpaid) {
        ui->cbPayment->addItem(tr("Not from cashbox"));
    }
    for (const StorePaymentPreset &p : mPaymentPresets) {
        ui->cbPayment->addItem(p.label());
    }

    int idx = -1;
    if (mCurrentPayment.isUnpaid()) {
        if (showUnpaid) {
            idx = 0;
        } else if (!mPaymentPresets.isEmpty()) {
            mCurrentPayment = mPaymentPresets.first();
            idx = 0;
        }
    } else {
        for (int i = 0; i < mPaymentPresets.size(); ++i) {
            if (mPaymentPresets.at(i).matches(mCurrentPayment)) {
                idx = i + (showUnpaid ? 1 : 0);
                break;
            }
        }
    }
    if (idx >= 0) {
        ui->cbPayment->setCurrentIndex(idx);
    }
}

void C5SalaryPaymentEditor::on_cbPayment_currentIndexChanged(int index)
{
    if (index < 0) {
        return;
    }
    const bool showUnpaid = mPaymentOptions.unpaidEnabled;
    if (showUnpaid && index == 0) {
        mCurrentPayment = StorePaymentPreset();
        return;
    }
    const int presetIndex = index - (showUnpaid ? 1 : 0);
    if (presetIndex >= 0 && presetIndex < mPaymentPresets.size()) {
        mCurrentPayment = mPaymentPresets.at(presetIndex);
    }
}

void C5SalaryPaymentEditor::on_btnPaymentConfig_clicked()
{
    if (!DlgStoreInputPayment::configure(mPaymentOptions, loadCashboxList(), this, tr("Not from cashbox"))) {
        return;
    }
    selectPayment(mCurrentPayment);
}

void C5SalaryPaymentEditor::print()
{
    QTextDocument doc;
    doc.setHtml(buildPrintHtml());

    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog dlg(&printer, this);
    if (dlg.exec() == QDialog::Accepted) {
        doc.print(&printer);
    }
}

void C5SalaryPaymentEditor::initTable()
{
    ui->tblSalary->setRowCount(0);
    ui->tblSalary->setColumnCount(8);
    ui->tblSalary->setHorizontalHeaderLabels({
        QStringLiteral("DbId"),
        QStringLiteral("PositionId"),
        QStringLiteral("StaffId"),
        QStringLiteral("#"),
        tr("Position"),
        tr("Name"),
        tr("Debt"),
        tr("Paid amount"),
    });
    ui->tblSalary->setColumnHidden(colDbId, true);
    ui->tblSalary->setColumnHidden(colPositionId, true);
    ui->tblSalary->setColumnHidden(colStaffId, true);
    ui->tblSalary->horizontalHeader()->setSectionResizeMode(colNum, QHeaderView::ResizeToContents);
    ui->tblSalary->horizontalHeader()->setSectionResizeMode(colPosition, QHeaderView::Stretch);
    ui->tblSalary->horizontalHeader()->setSectionResizeMode(colName, QHeaderView::Stretch);
    ui->tblSalary->horizontalHeader()->setSectionResizeMode(colDebt, QHeaderView::ResizeToContents);
    ui->tblSalary->horizontalHeader()->setSectionResizeMode(colAmount, QHeaderView::ResizeToContents);
    ui->tblSalary->verticalHeader()->setVisible(false);
    ui->tblSalary->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void C5SalaryPaymentEditor::recalcTotal()
{
    double total = 0;
    for (int i = 0; i < ui->tblSalary->rowCount(); ++i) {
        const auto *itAmount = ui->tblSalary->item(i, colAmount);
        total += itAmount ? qAbs(str_table_amount(itAmount->text())) : 0;
    }
    ui->leTotalAmount->setText(QString::number(total, 'f', 2));
}

QString C5SalaryPaymentEditor::buildPrintHtml() const
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
    html += QStringLiteral("<h2>") + tr("Salary payment").toHtmlEscaped() + QStringLiteral("</h2>");
    html += QStringLiteral("<p>") + tr("Date: %1").arg(ui->deDate->date().toString(QStringLiteral("dd.MM.yyyy"))).toHtmlEscaped()
            + QStringLiteral("</p>");
    html += QStringLiteral("<table><thead><tr>"
                           "<th>#</th><th>") + tr("Position").toHtmlEscaped() + QStringLiteral("</th><th>")
            + tr("Name").toHtmlEscaped() + QStringLiteral("</th><th>") + tr("Debt").toHtmlEscaped()
            + QStringLiteral("</th><th>") + tr("Paid amount").toHtmlEscaped()
            + QStringLiteral("</th></tr></thead><tbody>");

    for (int i = 0; i < ui->tblSalary->rowCount(); ++i) {
        const QString pos = ui->tblSalary->item(i, colPosition) ? ui->tblSalary->item(i, colPosition)->text() : QString();
        const QString name = ui->tblSalary->item(i, colName) ? ui->tblSalary->item(i, colName)->text() : QString();
        const QString debt = ui->tblSalary->item(i, colDebt) ? ui->tblSalary->item(i, colDebt)->text() : QStringLiteral("0.00");
        const QString amount = ui->tblSalary->item(i, colAmount) ? ui->tblSalary->item(i, colAmount)->text() : QStringLiteral("0.00");
        html += QStringLiteral("<tr>");
        html += QStringLiteral("<td class='num'>") + QString::number(i + 1).toHtmlEscaped() + QStringLiteral("</td>");
        html += QStringLiteral("<td>") + pos.toHtmlEscaped() + QStringLiteral("</td>");
        html += QStringLiteral("<td>") + name.toHtmlEscaped() + QStringLiteral("</td>");
        html += QStringLiteral("<td class='money'>") + debt.toHtmlEscaped() + QStringLiteral("</td>");
        html += QStringLiteral("<td class='money'>") + amount.toHtmlEscaped() + QStringLiteral("</td>");
        html += QStringLiteral("</tr>");
    }

    html += QStringLiteral("</tbody></table>");
    html += QStringLiteral("<p><b>") + tr("Total paid: %1").arg(ui->leTotalAmount->text()).toHtmlEscaped() + QStringLiteral("</b></p>");
    html += QStringLiteral("</body></html>");
    return html;
}
