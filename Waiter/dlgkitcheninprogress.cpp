#include "dlgkitcheninprogress.h"
#include "ui_dlgkitcheninprogress.h"

#include <QAbstractItemView>
#include <QBrush>
#include <QColor>
#include <QDateTime>
#include <QHeaderView>
#include <QHideEvent>
#include <QJsonArray>
#include <QLineEdit>
#include <QShowEvent>
#include <QStringList>
#include <QTableWidgetItem>
#include <QDialog>
#include <QTime>
#include <algorithm>
#include <limits>
#include <QCoreApplication>
#include <QSettings>
#include <QVariant>
#include "c5message.h"
#include "dict_payment_type.h"
#include "c5user.h"
#include "dlgkitchensettings.h"
#include "dlgorder.h"
#include "dlgsimleoptions.h"
#include "dlgtext.h"
#include "format_date.h"
#include "ninterface.h"

namespace
{

constexpr int kKitchenRoleTableId = int(Qt::UserRole) + 40;

constexpr int kKitchenRoleHallId = int(Qt::UserRole) + 41;

struct KitchenColDef {
    int col;
    const char *id;
    const char *label;
    int defaultWidth;
    bool defaultVisible;
};

/* Order matches the visible columns; ColHeaderId is technical and not exposed. */
const KitchenColDef kKitchenCols[] = {
    {1 /*ColOrderPrefix*/, "order_prefix", QT_TRANSLATE_NOOP("DlgKitchenSettings", "Order"),     120, true},
    {2 /*ColTimeOpen*/,    "time_open",    QT_TRANSLATE_NOOP("DlgKitchenSettings", "Time"),      120, true},
    {3 /*ColTableName*/,   "table_name",   QT_TRANSLATE_NOOP("DlgKitchenSettings", "Table"),      90, true},
    {4 /*ColHallName*/,    "hall_name",    QT_TRANSLATE_NOOP("DlgKitchenSettings", "Hall"),       90, true},
    {5 /*ColDishes*/,      "dishes",       QT_TRANSLATE_NOOP("DlgKitchenSettings", "Dishes"),    300, true},
    {6 /*ColGuest*/,       "guest",        QT_TRANSLATE_NOOP("DlgKitchenSettings", "Guest"),     200, true},
    {7 /*ColPaid*/,        "payment",      QT_TRANSLATE_NOOP("DlgKitchenSettings", "Payment"),   140, true},
    {8 /*ColStatus*/,      "status",       QT_TRANSLATE_NOOP("DlgKitchenSettings", "Status"),     84, true},
};

QString kitchenColRegKey(const QString &id, const char *suffix)
{
    return QStringLiteral("kitchen_col_%1_%2").arg(id, QLatin1String(suffix));
}

/* Mirror C5Config::getRegValue / setRegValue location so existing settings
 * (and any settings written elsewhere by the same convention) stay consistent. */
QString kitchenSettingsApp()
{
    return QString::fromUtf8(_APPLICATION_) + QStringLiteral("\\") + QString::fromUtf8(_MODULE_);
}

QString kitchenSettingsOrg()
{
    return QString::fromUtf8(_ORGANIZATION_);
}

QVariant readKitchenSetting(const QString &key, const QVariant &def)
{
    QSettings s(kitchenSettingsOrg(), kitchenSettingsApp());
    return s.value(key, def);
}

void writeKitchenSetting(const QString &key, const QVariant &value)
{
    QSettings s(kitchenSettingsOrg(), kitchenSettingsApp());
    s.setValue(key, value);
}

QColor kitchenRowBackground(int minLineStatus)
{
    switch(minLineStatus) {
        case 1:
            return QColor(QStringLiteral("#455a71"));
        case 2:
            return QColor(QStringLiteral("#b86228"));
        case 3:
            return QColor(QStringLiteral("#2d7a54"));
        case 4:
            return QColor(QStringLiteral("#54687a"));
        default:
            return QColor(QStringLiteral("#495963"));
    }
}

void paintKitchenOrderRow(QTableWidget *table, int row, int minLineStatus)
{
    if(!table || row < 0) {
        return;
    }

    const QBrush bg(kitchenRowBackground(minLineStatus));
    const QBrush fg(QColor(QStringLiteral("#f5f7fa")));

    for(int c = 0; c < table->columnCount(); ++c) {
        QTableWidgetItem *it = table->item(row, c);

        if(it) {
            it->setBackground(bg);
            it->setForeground(fg);
        }
    }
}

int minStatusInLines(const QJsonArray &lines)
{
    int mn = 99;

    for(const auto &lv : lines) {
        const int st = lv.toObject().value(QStringLiteral("f_status")).toInt();
        mn = std::min(mn, st);
    }

    return mn >= 99 ? 1 : mn;
}

QString formatKitchenOpenedCell(const QJsonObject &order)
{
    const QString d = order.value(QStringLiteral("f_date_open")).toString().trimmed();
    const QString t = order.value(QStringLiteral("f_time_open")).toString().trimmed();
    QString datePart = d;

    if(!d.isEmpty()) {
        const QDate qd = QDate::fromString(d, QStringLiteral("yyyy-MM-dd"));

        if(qd.isValid()) {
            datePart = qd.toString(FORMAT_DATE_TO_STR);
        }
    }

    QString timePart = t;

    if(!t.isEmpty()) {
        const QTime qt = QTime::fromString(t, QStringLiteral("HH:mm:ss"));

        if(qt.isValid()) {
            timePart = qt.toString(FORMAT_TIME_TO_SHORT_STR);
        }
    }

    if(datePart.isEmpty()) {
        return timePart;
    }

    if(timePart.isEmpty()) {
        return datePart;
    }

    return datePart + QLatin1Char('\n') + timePart;
}

qint64 kitchenOrderOpenedSortKeyMs(const QJsonObject &order)
{
    const QString d = order.value(QStringLiteral("f_date_open")).toString().trimmed();
    const QString t = order.value(QStringLiteral("f_time_open")).toString().trimmed();

    if(d.isEmpty() && t.isEmpty()) {
        return std::numeric_limits<qint64>::max();
    }

    const QString combined = t.isEmpty() ? d : (d.isEmpty() ? t : QStringLiteral("%1 %2").arg(d, t));
    const QDateTime dt = QDateTime::fromString(combined, FORMAT_DATETIME_TO_STR_MYSQL);
    return dt.isValid() ? dt.toMSecsSinceEpoch() : std::numeric_limits<qint64>::max();
}

void sortKitchenOrdersOldestFirst(QJsonArray &arr)
{
    struct Item {
        qint64 keyMs = std::numeric_limits<qint64>::max();
        int originalIndex = 0;
        QJsonValue value;
    };

    QVector<Item> vec;
    vec.reserve(arr.size());
    int idx = 0;

    for(const auto &v : arr) {
        Item it;
        it.keyMs = kitchenOrderOpenedSortKeyMs(v.toObject());
        it.originalIndex = idx++;
        it.value = v;
        vec.append(it);
    }

    std::sort(vec.begin(), vec.end(), [](const Item &a, const Item &b) {
        if(a.keyMs != b.keyMs) {
            return a.keyMs < b.keyMs;
        }
        return a.originalIndex < b.originalIndex;
    });
    arr = QJsonArray();

    for(const auto &it : vec) {
        arr.append(it.value);
    }
}

} // namespace

DlgKitchenInProgress::DlgKitchenInProgress(C5User *user,
                                           const QVector<HallItem> *halls,
                                           const QVector<TableItem> *tables,
                                           const QVector<GoodsGroupItem *> *groups,
                                           const QVector<DishAItem *> *dishes,
                                           QWidget *parent)
    : C5WaiterDialog(user)
    , ui(new Ui::DlgKitchenInProgress)
    , mHalls(halls)
    , mTables(tables)
    , mGroups(groups)
    , mDishes(dishes)
{
    Q_UNUSED(parent);
    ui->setupUi(this);
    ui->lbStaff->setText(mUser->fullName());
    ui->lbTime->setText(QTime::currentTime().toString(FORMAT_TIME_TO_SHORT_STR));

    ui->tblKitchenOrders->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblKitchenOrders->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tblKitchenOrders->verticalHeader()->setVisible(false);
    ui->tblKitchenOrders->verticalHeader()->setMinimumSectionSize(56);
    ui->tblKitchenOrders->setWordWrap(true);
    ui->tblKitchenOrders->setColumnHidden(ColHeaderId, true);
    ui->tblKitchenOrders->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    applyFixedColumnLayout();

    connect(ui->leOrderNumber, &QLineEdit::textChanged, this, &DlgKitchenInProgress::on_leOrderNumber_textChanged);
    connect(ui->leSearchAny, &QLineEdit::textChanged, this, &DlgKitchenInProgress::on_leSearchAny_textChanged);

    syncFilterButtons();

    connect(&mClockTimer, &QTimer::timeout, this, &DlgKitchenInProgress::tickClock);
    setupButtons();
    ui->btnSettings->setVisible(mUser->id() == 1);
}

DlgKitchenInProgress::~DlgKitchenInProgress()
{
    mClockTimer.stop();
    delete ui;
}

QString DlgKitchenInProgress::kitchenStatusText(int status) const
{
    switch(status) {
        case 1:
            return tr("Accepted", "kitchen_line_status");
        case 2:
            return tr("Cooking", "kitchen_line_status");
        case 3:
            return tr("Ready", "kitchen_line_status");
        case 4:
            return tr("Served", "kitchen_line_status");
        default:
            return QString::number(status);
    }
}

QString DlgKitchenInProgress::paymentColumnText(const QJsonObject &order) const
{
    QStringList parts;

    for(int pt : payment_types) {
        const QString field = payment_fields.value(pt);

        if(field.isEmpty()) {
            continue;
        }

        const double amt = order.value(field).toVariant().toDouble();

        if(amt > 1e-9) {
            const char *const nm = payment_names.value(pt);

            if(nm) {
                parts << QCoreApplication::translate("PaymentType", nm);
            }
        }
    }

    if(parts.isEmpty()) {
        return tr("No", "kitchen_paid_column");
    }

    return parts.join(QStringLiteral(", "));
}

QString DlgKitchenInProgress::formatQtyWidth5(double qty)
{
    QString s = QString::number(qty, 'f', 3);
    const int dot = s.indexOf(QLatin1Char('.'));

    if(dot >= 0) {
        while(s.endsWith(QLatin1Char('0')) && s.size() > dot + 2) {
            s.chop(1);
        }

        if(s.endsWith(QLatin1Char('.'))) {
            s.chop(1);
        }
    }

    if(s.size() > 5) {
        s = s.left(5);
    }

    return s.leftJustified(5, QLatin1Char(' '));
}

QString DlgKitchenInProgress::guestMultiline(const QJsonObject &order)
{
    QStringList lines;
    const QString name = order.value(QStringLiteral("f_guest_name")).toString().trimmed();
    const QString phone = order.value(QStringLiteral("f_guest_phone")).toString().trimmed();
    const QString addr = order.value(QStringLiteral("f_guest_address")).toString().trimmed();

    if(!name.isEmpty()) {
        lines << name;
    }

    if(!phone.isEmpty()) {
        lines << phone;
    }

    if(!addr.isEmpty()) {
        lines << addr;
    }

    return lines.join(QLatin1Char('\n'));
}

QString DlgKitchenInProgress::normalizedOrderSuffix(const QString &s)
{
    const QString t = s.trimmed();
    int i = 0;

    while(i < t.size() && t.at(i).isLetter()) {
        ++i;
    }

    return t.mid(i).trimmed();
}

QString DlgKitchenInProgress::haystackForSearch(const QJsonObject &order) const
{
    QStringList parts;
    parts << order.value(QStringLiteral("f_order_prefix")).toString();
    parts << order.value(QStringLiteral("f_table_name")).toString();
    parts << order.value(QStringLiteral("f_hall_name")).toString();
    parts << order.value(QStringLiteral("f_time_open")).toString();
    for(int pt : payment_types) {
        const QString field = payment_fields.value(pt);

        if(!field.isEmpty()) {
            parts << order.value(field).toVariant().toString();
        }
    }
    parts << guestMultiline(order);

    const QJsonArray lines = order.value(QStringLiteral("lines")).toArray();

    for(const auto &lv : lines) {
        const QJsonObject lo = lv.toObject();
        parts << lo.value(QStringLiteral("f_goods_name")).toString();
        parts << lo.value(QStringLiteral("f_comment")).toString();
        parts << QString::number(lo.value(QStringLiteral("f_qty")).toVariant().toDouble());
        parts << QString::number(lo.value(QStringLiteral("f_status")).toInt());
        parts << lo.value(QStringLiteral("f_goods_row_id")).toString();
    }

    return parts.join(QLatin1Char('\n'));
}

void DlgKitchenInProgress::syncFilterButtons()
{
    ui->btnFilterAccepted->setChecked(mStatusFilter == 1);
    ui->btnFilterCooking->setChecked(mStatusFilter == 2);
    ui->btnFilterReady->setChecked(mStatusFilter == 3);
}

void DlgKitchenInProgress::fillTableRows(const QJsonArray &orders)
{
    ui->tblKitchenOrders->setUpdatesEnabled(false);
    ui->tblKitchenOrders->setRowCount(0);
    m_linesPickByTableRow.clear();

    int r = 0;

    for(const auto &ov : orders) {
        const QJsonObject order = ov.toObject();
        const QJsonArray lines = order.value(QStringLiteral("lines")).toArray();

        if(lines.isEmpty()) {
            continue;
        }

        QVector<KitchenLinePick> picks;
        QStringList dishTexts;
        QStringList statusTexts;

        for(const auto &lv : lines) {
            const QJsonObject lo = lv.toObject();
            const QString lineId = lo.value(QStringLiteral("f_goods_row_id")).toString();
            const int st = lo.value(QStringLiteral("f_status")).toInt();
            const double qty = lo.value(QStringLiteral("f_qty")).toVariant().toDouble();
            const QString name = lo.value(QStringLiteral("f_goods_name")).toString();
            const QString comment = lo.value(QStringLiteral("f_comment")).toString().trimmed();
            const QString qty5 = formatQtyWidth5(qty);
            QString dishLine = qty5 + name;

            if(!comment.isEmpty()) {
                dishLine += QStringLiteral(" (") + comment + QLatin1Char(')');
            }

            dishTexts << dishLine;
            statusTexts << kitchenStatusText(st);
            KitchenLinePick p;
            p.lineId = lineId;
            p.status = st;
            p.pickerLabel = qty5 + name.trimmed();
            picks.append(p);
        }

        ui->tblKitchenOrders->insertRow(r);
        const int rowColorStatus = minStatusInLines(lines);

        auto *itHeader = new QTableWidgetItem(order.value(QStringLiteral("f_header_id")).toString());
        auto *itOrd = new QTableWidgetItem(order.value(QStringLiteral("f_order_prefix")).toString());
        auto *itTable = new QTableWidgetItem(order.value(QStringLiteral("f_table_name")).toString());
        itTable->setData(kKitchenRoleTableId, order.value(QStringLiteral("f_table")).toInt());
        itTable->setData(kKitchenRoleHallId, order.value(QStringLiteral("f_hall")).toInt());
        auto *itHall = new QTableWidgetItem(order.value(QStringLiteral("f_hall_name")).toString());
        auto *itDishes = new QTableWidgetItem(dishTexts.join(QLatin1Char('\n')));
        auto *itGuest = new QTableWidgetItem(guestMultiline(order));
        auto *itPaid = new QTableWidgetItem(paymentColumnText(order));
        auto *itTime = new QTableWidgetItem(formatKitchenOpenedCell(order));
        auto *itStatus = new QTableWidgetItem(statusTexts.join(QLatin1Char('\n')));

        const Qt::Alignment top = Qt::AlignTop;

        itHeader->setTextAlignment(Qt::AlignLeft | top);
        itOrd->setTextAlignment(Qt::AlignLeft | top);
        itTable->setTextAlignment(Qt::AlignLeft | top);
        itHall->setTextAlignment(Qt::AlignLeft | top);
        itDishes->setTextAlignment(Qt::AlignLeft | top);
        itGuest->setTextAlignment(Qt::AlignLeft | top);
        itPaid->setTextAlignment(Qt::AlignLeft | top);
        itTime->setTextAlignment(Qt::AlignLeft | top);
        itStatus->setTextAlignment(Qt::AlignLeft | top);

        ui->tblKitchenOrders->setItem(r, ColHeaderId, itHeader);
        ui->tblKitchenOrders->setItem(r, ColOrderPrefix, itOrd);
        ui->tblKitchenOrders->setItem(r, ColTableName, itTable);
        ui->tblKitchenOrders->setItem(r, ColHallName, itHall);
        ui->tblKitchenOrders->setItem(r, ColDishes, itDishes);
        ui->tblKitchenOrders->setItem(r, ColGuest, itGuest);
        ui->tblKitchenOrders->setItem(r, ColPaid, itPaid);
        ui->tblKitchenOrders->setItem(r, ColTimeOpen, itTime);
        ui->tblKitchenOrders->setItem(r, ColStatus, itStatus);

        paintKitchenOrderRow(ui->tblKitchenOrders, r, rowColorStatus);
        m_linesPickByTableRow.append(picks);

        ++r;
    }

    ui->tblKitchenOrders->resizeRowsToContents();
    applyFixedColumnLayout();
    ui->tblKitchenOrders->setUpdatesEnabled(true);
}

void DlgKitchenInProgress::applyFixedColumnLayout()
{
    /* Technical id column always hidden. */
    ui->tblKitchenOrders->setColumnHidden(ColHeaderId, true);

    for(const KitchenColDef &c : kKitchenCols) {
        const QString idStr = QLatin1String(c.id);
        const bool visible = readKitchenSetting(kitchenColRegKey(idStr, "visible"), c.defaultVisible).toBool();
        const int width = qMax(20, readKitchenSetting(kitchenColRegKey(idStr, "width"), c.defaultWidth).toInt());
        ui->tblKitchenOrders->setColumnHidden(c.col, !visible);
        ui->tblKitchenOrders->setColumnWidth(c.col, width);
    }
}

void DlgKitchenInProgress::applyFrontendFilters()
{
    QJsonArray filtered;
    const QString orderRaw = ui->leOrderNumber->text().trimmed();
    const QString orderKey = normalizedOrderSuffix(orderRaw);
    const QString searchRaw = ui->leSearchAny->text().trimmed();
    const bool useSearch = searchRaw.size() > 2;
    const QString needle = useSearch ? searchRaw.toLower() : QString();

    for(const auto &v : mAllKitchenRows) {
        const QJsonObject order = v.toObject();
        const QJsonArray lines = order.value(QStringLiteral("lines")).toArray();

        if(lines.isEmpty()) {
            continue;
        }

        if(mStatusFilter != 0) {
            bool any = false;

            for(const auto &lv : lines) {
                if(lv.toObject().value(QStringLiteral("f_status")).toInt() == mStatusFilter) {
                    any = true;
                    break;
                }
            }

            if(!any) {
                continue;
            }
        }

        if(!orderRaw.isEmpty()) {
            if(orderKey.isEmpty()) {
                continue;
            }

            const QString prefKey = normalizedOrderSuffix(order.value(QStringLiteral("f_order_prefix")).toString());

            if(prefKey != orderKey) {
                continue;
            }
        }

        if(useSearch && !haystackForSearch(order).toLower().contains(needle)) {
            continue;
        }

        filtered.append(order);
    }

    fillTableRows(filtered);
}

void DlgKitchenInProgress::ingestServerDoc(const QJsonObject &jdoc)
{
    mAllKitchenRows = jdoc.value(QStringLiteral("data")).toArray();
    sortKitchenOrdersOldestFirst(mAllKitchenRows);
    applyFrontendFilters();
}

void DlgKitchenInProgress::reloadList()
{
    fHttp->createHttpQueryLambda(QStringLiteral("/engine/v2/waiter/in-progress/get"),
                                 {},
                                 [this](const QJsonObject &jdoc) {
                                     ingestServerDoc(jdoc);
                                 },
                                 [](const QJsonObject &) {},
                                 {},
                                 false);
}

void DlgKitchenInProgress::applyKitchenLineStatusToLines(const QVector<QString> &lineIds, int status)
{
    QVector<QString> ids;

    for(const QString &s : lineIds) {
        if(!s.isEmpty()) {
            ids.append(s);
        }
    }

    if(ids.isEmpty()) {
        return;
    }

    if(status == 4) {
        const QString msg = ids.size() == 1
                                ? tr("Mark as served to the guest? The line will disappear from the kitchen list.",
                                     "kitchen_confirm_served")
                                : tr("Mark all selected lines as served to the guest? They will disappear from the kitchen list.",
                                     "kitchen_confirm_served_all");

        if(C5Message::question(msg) != QDialog::Accepted) {
            return;
        }
    }

    runKitchenStatusUpdateChain(ids, 0, status);
}

void DlgKitchenInProgress::runKitchenStatusUpdateChain(const QVector<QString> &lineIds, int index, int status)
{
    if(index >= lineIds.size()) {
        return;
    }

    const QString id = lineIds.at(index);
    const bool isLast = (index + 1 >= lineIds.size());

    fHttp->createHttpQueryLambda(QStringLiteral("/engine/v2/waiter/in-progress/update-status"),
                                 QJsonObject{
                                     {QStringLiteral("id"), id},
                                     {QStringLiteral("status"), status},
                                 },
                                 [this, lineIds, index, status, isLast](const QJsonObject &jdoc) {
                                     if(isLast) {
                                         ingestServerDoc(jdoc);
                                     } else {
                                         runKitchenStatusUpdateChain(lineIds, index + 1, status);
                                     }
                                 },
                                 [](const QJsonObject &) {},
                                 {},
                                 false);
}

void DlgKitchenInProgress::openStatusPickerForLineIds(const QVector<QString> &lineIds)
{
    if(lineIds.isEmpty()) {
        return;
    }

    QStringList titles;
    titles << kitchenStatusText(1) << kitchenStatusText(2) << kitchenStatusText(3) << kitchenStatusText(4);
    const QList<int> values = {1, 2, 3, 4};
    DlgSimleOptions dso(titles, values, mUser, this);
    const int result = dso.exec();

    if(result >= 1 && result <= 4) {
        applyKitchenLineStatusToLines(lineIds, result);
    }
}

void DlgKitchenInProgress::openStatusPickerForTableRow(int row)
{
    if(row < 0 || row >= m_linesPickByTableRow.size()) {
        return;
    }

    const QVector<KitchenLinePick> &picks = m_linesPickByTableRow.at(row);

    if(picks.isEmpty()) {
        return;
    }

    QVector<QString> chosenIds;

    if(picks.size() == 1) {
        chosenIds.append(picks.constFirst().lineId);
    } else {
        QStringList titles;
        QList<int> vals;
        titles << tr("All", "kitchen_pick_line");
        vals << -1;

        for(int i = 0; i < picks.size(); ++i) {
            titles << picks.at(i).pickerLabel;
            vals << (i + 1);
        }

        const int colCount = picks.size() > 18 ? 3 : (picks.size() > 8 ? 2 : 1);
        DlgSimleOptions pickLine(titles, vals, colCount, mUser, this);
        const int pickVal = pickLine.exec();

        if(pickVal == 0) {
            return;
        }

        if(pickVal == -1) {
            for(const auto &p : picks) {
                chosenIds.append(p.lineId);
            }
        } else if(pickVal >= 1 && pickVal <= picks.size()) {
            chosenIds.append(picks.at(pickVal - 1).lineId);
        } else {
            return;
        }
    }

    openStatusPickerForLineIds(chosenIds);
}

void DlgKitchenInProgress::showEvent(QShowEvent *e)
{
    C5WaiterDialog::showEvent(e);

    if(e->spontaneous()) {
        return;
    }

    ui->lbStaff->setText(mUser->fullName());
    ui->lbTime->setText(QTime::currentTime().toString(FORMAT_TIME_TO_SHORT_STR));
    applyFixedColumnLayout();
    mClockTimer.start(1000);
    reloadList();
}

void DlgKitchenInProgress::hideEvent(QHideEvent *e)
{
    mClockTimer.stop();
    C5WaiterDialog::hideEvent(e);
}

void DlgKitchenInProgress::tickClock()
{
    ui->lbTime->setText(QTime::currentTime().toString(FORMAT_TIME_TO_SHORT_STR));
}

void DlgKitchenInProgress::on_btnExit_clicked()
{
    reject();
}

void DlgKitchenInProgress::on_leOrderNumber_returnPressed()
{
    applyFrontendFilters();
}

void DlgKitchenInProgress::on_leSearchAny_returnPressed()
{
    applyFrontendFilters();
}

void DlgKitchenInProgress::on_leOrderNumber_textChanged(const QString &)
{
    applyFrontendFilters();
}

void DlgKitchenInProgress::on_leSearchAny_textChanged(const QString &)
{
    applyFrontendFilters();
}

void DlgKitchenInProgress::on_btnFilterAccepted_clicked()
{
    mStatusFilter = (mStatusFilter == 1) ? 0 : 1;
    syncFilterButtons();
    applyFrontendFilters();
}

void DlgKitchenInProgress::on_btnFilterCooking_clicked()
{
    mStatusFilter = (mStatusFilter == 2) ? 0 : 2;
    syncFilterButtons();
    applyFrontendFilters();
}

void DlgKitchenInProgress::on_btnFilterReady_clicked()
{
    mStatusFilter = (mStatusFilter == 3) ? 0 : 3;
    syncFilterButtons();
    applyFrontendFilters();
}

void DlgKitchenInProgress::on_tblKitchenOrders_cellClicked(int row, int column)
{
    if(column == ColTableName) {
        openDlgOrderForKitchenRow(row);
        return;
    }

    if(column != ColStatus) {
        return;
    }

    openStatusPickerForTableRow(row);
}

void DlgKitchenInProgress::openDlgOrderForKitchenRow(int row)
{
    if(row < 0 || !mHalls || !mTables || !mGroups || !mDishes) {
        return;
    }

    QTableWidgetItem *itTable = ui->tblKitchenOrders->item(row, ColTableName);
    QTableWidgetItem *itHdr = ui->tblKitchenOrders->item(row, ColHeaderId);

    if(!itTable || !itHdr) {
        return;
    }

    const int tableId = itTable->data(kKitchenRoleTableId).toInt();
    const int hallId = itTable->data(kKitchenRoleHallId).toInt();
    const QString headerId = itHdr->text().trimmed();

    if(headerId.isEmpty() || tableId <= 0 || hallId <= 0) {
        C5Message::error(tr("Cannot open order"));
        return;
    }

    TableItem t;

    for(const TableItem &ti : *mTables) {
        if(ti.id == tableId) {
            t = ti;
            break;
        }
    }

    HallItem h;

    for(const HallItem &hi : *mHalls) {
        if(hi.id == hallId) {
            h = hi;
            break;
        }
    }

    if(t.id <= 0 || h.id <= 0) {
        C5Message::error(tr("Cannot resolve hall/table"));
        return;
    }

    auto *order = new DlgOrder(mUser, h, t, mGroups, mDishes);
    order->setOrderId(headerId);
    order->exec();
    order->deleteLater();
    reloadList();
}

void DlgKitchenInProgress::on_btnSearchAnyKbd_clicked()
{
    QString s = ui->leSearchAny->text();

    if(!DlgText::getText(mUser, tr("Search"), s)) {
        return;
    }

    ui->leSearchAny->setText(s);
}

void DlgKitchenInProgress::on_btnOrderNumberKbd_clicked()
{
    QString s = ui->leOrderNumber->text();

    if(!DlgText::getText(mUser, tr("Order No"), s)) {
        return;
    }

    ui->leOrderNumber->setText(s);
}

void DlgKitchenInProgress::on_btnSettings_clicked()
{
    QVector<KitchenColSetting> cols;
    cols.reserve(int(sizeof(kKitchenCols) / sizeof(kKitchenCols[0])));

    for(const KitchenColDef &c : kKitchenCols) {
        KitchenColSetting s;
        s.id = QLatin1String(c.id);
        s.label = QCoreApplication::translate("DlgKitchenSettings", c.label);
        s.defaultWidth = c.defaultWidth;
        s.defaultVisible = c.defaultVisible;
        s.minWidth = 30;
        s.maxWidth = 1200;
        s.visible = readKitchenSetting(kitchenColRegKey(s.id, "visible"), c.defaultVisible).toBool();
        s.width = qBound(s.minWidth,
                         readKitchenSetting(kitchenColRegKey(s.id, "width"), c.defaultWidth).toInt(),
                         s.maxWidth);
        cols.append(s);
    }

    DlgKitchenSettings dlg(mUser, cols, this);

    if(dlg.exec() != QDialog::Accepted) {
        return;
    }

    const QVector<KitchenColSetting> updated = dlg.result();

    for(const KitchenColSetting &s : updated) {
        writeKitchenSetting(kitchenColRegKey(s.id, "visible"), s.visible);
        writeKitchenSetting(kitchenColRegKey(s.id, "width"), s.width);
    }

    applyFixedColumnLayout();
}
