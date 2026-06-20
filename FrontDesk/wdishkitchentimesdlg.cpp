#include "wdishkitchentimesdlg.h"
#include "ui_wdishkitchentimesdlg.h"
#include "c5message.h"
#include "c5user.h"
#include "c5utils.h"
#include "ninterface.h"
#include <QCoreApplication>
#include <QDateTime>
#include <QHeaderView>
#include <QDialogButtonBox>
#include <QEventLoop>
#include <QJsonObject>
#include <QPushButton>
#include <QTableWidgetItem>

namespace
{

enum RowKind {
    RowKitchenStatus1 = 0,
    RowKitchenStatus4 = 3,
    RowAppend = 4,
    RowPrint = 5,
};

QString kitchenStatusLabel(int status)
{
    switch(status) {
    case 1:
        return QCoreApplication::translate("WDishKitchenTimesDlg", "Accepted");
    case 2:
        return QCoreApplication::translate("WDishKitchenTimesDlg", "Cooking");
    case 3:
        return QCoreApplication::translate("WDishKitchenTimesDlg", "Ready");
    case 4:
        return QCoreApplication::translate("WDishKitchenTimesDlg", "Served");
    default:
        return QCoreApplication::translate("WDishKitchenTimesDlg", "Status %1").arg(status);
    }
}

QString earliestProcessTime(const QJsonObject &process, int status)
{
    QString best;
    QDateTime bestDt;

    for(int sub = 1; sub <= 4; ++sub) {
        const QString key = QStringLiteral("f_status_%1_%2_time").arg(status).arg(sub);
        const QString raw = process.value(key).toString().trimmed();
        if(raw.isEmpty()) {
            continue;
        }

        const QDateTime dt = QDateTime::fromString(raw, QStringLiteral("yyyy-MM-dd HH:mm:ss"));
        if(!dt.isValid()) {
            if(best.isEmpty()) {
                best = raw;
            }
            continue;
        }

        if(!bestDt.isValid() || dt < bestDt) {
            bestDt = dt;
            best = raw;
        }
    }

    return best;
}

void addRow(QTableWidget *table, int row, const QString &label, const QString &time, const QString &rowKey, bool timeEditable)
{
    auto *labelItem = new QTableWidgetItem(label);
    labelItem->setFlags(Qt::ItemIsEnabled);
    labelItem->setData(Qt::UserRole, rowKey);
    table->setItem(row, 0, labelItem);

    auto *timeItem = new QTableWidgetItem(time);
    if(timeEditable) {
        timeItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
    } else {
        timeItem->setFlags(Qt::ItemIsEnabled);
    }
    table->setItem(row, 1, timeItem);
}

} // namespace

WDishKitchenTimesDlg::WDishKitchenTimesDlg(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::WDishKitchenTimesDlg)
{
    ui->setupUi(this);
    ui->tblTimes->horizontalHeader()->setStretchLastSection(true);
    ui->tblTimes->verticalHeader()->setVisible(false);

    if(auto *saveBtn = ui->buttonBox->button(QDialogButtonBox::Save)) {
        connect(saveBtn, &QPushButton::clicked, this, &WDishKitchenTimesDlg::onSaveClicked);
    }
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

WDishKitchenTimesDlg::~WDishKitchenTimesDlg()
{
    delete ui;
}

void WDishKitchenTimesDlg::setUser(C5User *user)
{
    mUser = user;
}

void WDishKitchenTimesDlg::setDish(const WaiterDish &dish)
{
    mDishId = dish.id.trimmed();
    mHeaderId = dish.header.trimmed();
    mSavedOrder = {};

    setWindowTitle(tr("Dish times"));
    ui->lbDishName->setText(QStringLiteral("%1 — %2").arg(dish.dishName, float_str(dish.qty, 3)));

    const QJsonObject process = dish.data.value(QStringLiteral("f_goods_process")).toObject();
    const int currentKitchen = dish.data.value(QStringLiteral("f_kitchen_status")).toInt();

    constexpr int rowCount = 6;
    ui->tblTimes->setRowCount(rowCount);

    for(int st = 1; st <= 4; ++st) {
        QString label = kitchenStatusLabel(st);
        if(st == currentKitchen) {
            label += QStringLiteral(" *");
        }
        addRow(ui->tblTimes,
               st - 1,
               label,
               earliestProcessTime(process, st),
               QString::number(st),
               true);
    }

    addRow(ui->tblTimes,
           RowAppend,
           tr("Append time"),
           dish.data.value(QStringLiteral("f_append_time")).toString(),
           QStringLiteral("append"),
           true);
    addRow(ui->tblTimes,
           RowPrint,
           tr("Print time"),
           dish.data.value(QStringLiteral("f_print_time")).toString(),
           QStringLiteral("print"),
           true);

    ui->tblTimes->resizeColumnsToContents();
    ui->tblTimes->resizeRowsToContents();
}

QString WDishKitchenTimesDlg::normalizeTimeText(const QString &raw)
{
    const QString trimmed = raw.trimmed();
    if(trimmed.isEmpty()) {
        return QString();
    }

    QDateTime dt = QDateTime::fromString(trimmed, QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    if(!dt.isValid()) {
        dt = QDateTime::fromString(trimmed, QStringLiteral("dd.MM.yyyy HH:mm:ss"));
    }
    if(!dt.isValid()) {
        dt = QDateTime::fromString(trimmed, QStringLiteral("dd.MM.yyyy HH:mm"));
    }
    if(!dt.isValid()) {
        dt = QDateTime::fromString(trimmed, Qt::ISODate);
    }

    if(!dt.isValid()) {
        return QString();
    }

    return dt.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
}

bool WDishKitchenTimesDlg::collectAndSave()
{
    if(mDishId.isEmpty()) {
        C5Message::error(tr("Dish id is missing"));
        return false;
    }

    if(!mUser || mUser->mSessionKey.isEmpty()) {
        C5Message::error(tr("Not authorized"));
        return false;
    }

    QJsonObject processTimes;
    QString appendTime;
    QString printTime;

    for(int st = 1; st <= 4; ++st) {
        auto *labelItem = ui->tblTimes->item(st - 1, 0);
        auto *timeItem = ui->tblTimes->item(st - 1, 1);
        if(!labelItem || !timeItem) {
            continue;
        }

        const QString normalized = normalizeTimeText(timeItem->text());
        if(!timeItem->text().trimmed().isEmpty() && normalized.isEmpty()) {
            C5Message::error(tr("Invalid time format on row \"%1\". Use yyyy-MM-dd HH:mm:ss")
                                 .arg(labelItem->text()));
            return false;
        }
        processTimes.insert(QString::number(st), normalized);
    }

    for(int row = RowAppend; row <= RowPrint; ++row) {
        auto *labelItem = ui->tblTimes->item(row, 0);
        auto *timeItem = ui->tblTimes->item(row, 1);
        if(!labelItem || !timeItem) {
            continue;
        }

        const QString key = labelItem->data(Qt::UserRole).toString();
        const QString normalized = normalizeTimeText(timeItem->text());
        if(!timeItem->text().trimmed().isEmpty() && normalized.isEmpty()) {
            C5Message::error(tr("Invalid time format on row \"%1\". Use yyyy-MM-dd HH:mm:ss")
                                 .arg(labelItem->text()));
            return false;
        }

        if(key == QStringLiteral("append")) {
            appendTime = normalized;
        } else if(key == QStringLiteral("print")) {
            printTime = normalized;
        }
    }

    QJsonObject params;
    params.insert(QStringLiteral("id"), mDishId);
    params.insert(QStringLiteral("process_times"), processTimes);
    params.insert(QStringLiteral("f_append_time"), appendTime);
    params.insert(QStringLiteral("f_print_time"), printTime);

    bool ok = false;
    QEventLoop loop;
    NInterface::query(QStringLiteral("/engine/v2/waiter/order/set-dish-kitchen-times"),
                      mUser->mSessionKey,
                      this,
                      params,
                      [&](const QJsonObject &jo) {
                          mSavedOrder = jo.value(QStringLiteral("order")).toObject();
                          ok = !mSavedOrder.isEmpty();
                          loop.quit();
                      },
                      [&](const QJsonObject &) {
                          loop.quit();
                          return true;
                      },
                      true,
                      30000);
    loop.exec();

    if(!ok) {
        C5Message::error(tr("Failed to save dish times"));
        return false;
    }

    return true;
}

void WDishKitchenTimesDlg::onSaveClicked()
{
    if(collectAndSave()) {
        accept();
    }
}
