#include "dlgkitcheninprogressfine.h"
#include "ui_dlgkitcheninprogressfine.h"

#include <QButtonGroup>
#include <QCoreApplication>
#include <QDate>
#include <QDateEdit>
#include <QDateTime>
#include <QDialog>
#include <QDialogButtonBox>
#include <QHideEvent>
#include <QHBoxLayout>
#include <QResizeEvent>
#include <QSettings>
#include <QShowEvent>
#include <QTime>
#include <QVBoxLayout>
#include "c5message.h"
#include "c5user.h"
#include "c5utils.h"
#include "dlgorder.h"
#include "dlgsimleoptions.h"
#include "format_date.h"
#include "ninterface.h"
#include <algorithm>
#include <limits>

namespace
{

static bool pickKitchenHistoryDate(QWidget *parent, QDate &outDate)
{
    QDialog dlg(parent);
    dlg.setWindowTitle(QCoreApplication::translate("DlgKitchenInProgressFine", "Archive date", "kitchen_history_mode"));
    dlg.setMinimumSize(280, 120);
    auto *lay = new QVBoxLayout(&dlg);
    lay->setContentsMargins(12, 12, 12, 12);
    auto *de = new QDateEdit(QDate::currentDate(), &dlg);
    de->setCalendarPopup(true);
    de->setDisplayFormat(QStringLiteral("dd.MM.yyyy"));
    de->setMaximumDate(QDate::currentDate());
    lay->addWidget(de);
    auto *bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    QObject::connect(bb, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    QObject::connect(bb, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    lay->addWidget(bb);

    if(dlg.exec() != QDialog::Accepted) {
        return false;
    }

    outDate = de->date();
    return outDate.isValid();
}

QString kitchenFineSettingsApp()
{
    return QString::fromUtf8(_APPLICATION_) + QStringLiteral("\\") + QString::fromUtf8(_MODULE_);
}

QString kitchenFineSettingsOrg()
{
    return QString::fromUtf8(_ORGANIZATION_);
}

QVariant readKitchenFineSetting(const QString &key, const QVariant &def)
{
    QSettings s(kitchenFineSettingsOrg(), kitchenFineSettingsApp());
    return s.value(key, def);
}

QString formatQtyPickerLabel(double qty, const QString &name)
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

    return s.leftJustified(5, QLatin1Char(' ')) + name.trimmed();
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

bool isLineOverdueReady(const QJsonObject &line, int overdueMinutes)
{
    if(line.value(QStringLiteral("f_status")).toInt() != 3) {
        return false;
    }

    const QString readyAt = line.value(QStringLiteral("f_ready_at")).toString().trimmed();

    if(readyAt.isEmpty()) {
        return false;
    }

    QDateTime dt = QDateTime::fromString(readyAt, QStringLiteral("yyyy-MM-dd HH:mm:ss"));

    if(!dt.isValid()) {
        dt = QDateTime::fromString(readyAt, FORMAT_DATETIME_TO_STR_MYSQL);
    }

    if(!dt.isValid()) {
        return false;
    }

    return dt.secsTo(QDateTime::currentDateTime()) > overdueMinutes * 60;
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

void sortKitchenRowsByOpenedTime(QJsonArray &arr)
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

QJsonArray jsonPayloadDataArray(const QJsonObject &jdoc)
{
    const QJsonValue v = jdoc.value(QStringLiteral("data"));

    if(v.isArray()) {
        return v.toArray();
    }

    return QJsonArray();
}

constexpr int kFineColumnSpacing = 16;

} // namespace

DlgKitchenInProgressFine::DlgKitchenInProgressFine(C5User *user,
                                                   const QVector<HallItem> *halls,
                                                   const QVector<TableItem> *tables,
                                                   const QVector<GoodsGroupItem *> *groups,
                                                   const QVector<DishAItem *> *dishes,
                                                   QWidget *parent)
    : C5WaiterDialog(user)
    , ui(new Ui::DlgKitchenInProgressFine)
    , mHalls(halls)
    , mTables(tables)
    , mGroups(groups)
    , mDishes(dishes)
{
    Q_UNUSED(parent);
    ui->setupUi(this);

    ui->lbTime->setText(QTime::currentTime().toString(FORMAT_TIME_TO_SHORT_STR));
    ui->btnTabLive->setText(tr("Current orders", "kitchen_fine_tab_live"));
    ui->btnTabHistory->setText(tr("Order history", "kitchen_fine_tab_history"));

    auto *tabGroup = new QButtonGroup(this);
    tabGroup->setExclusive(true);
    tabGroup->addButton(ui->btnTabLive);
    tabGroup->addButton(ui->btnTabHistory);

    mCardsRoot = ui->scrollCardsContents;
    mCardsRoot->setObjectName(QStringLiteral("scrollCardsContents"));
    mCardsColumnsLayout = new QHBoxLayout(mCardsRoot);
    mCardsColumnsLayout->setContentsMargins(0, 0, 0, 0);
    mCardsColumnsLayout->setSpacing(kFineColumnSpacing);
    mCardsColumnsLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    ui->scrollCards->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrollCards->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->scrollCards->setWidgetResizable(false);

    connect(&mClockTimer, &QTimer::timeout, this, &DlgKitchenInProgressFine::tickClock);
    setupButtons();

    mOriginalWindowTitle = windowTitle();
    setKitchenHistoryReadonly(false);
    syncTopTabs();
    updateLiveCountBadge(0);
}

DlgKitchenInProgressFine::~DlgKitchenInProgressFine()
{
    mClockTimer.stop();
    delete ui;
}

int DlgKitchenInProgressFine::overdueMinutes()
{
    const int v = readKitchenFineSetting(QStringLiteral("kitchen_fine_overdue_minutes"), 10).toInt();
    return v > 0 ? v : 10;
}

QString DlgKitchenInProgressFine::kitchenStatusText(int status) const
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

QString DlgKitchenInProgressFine::cardStatusButtonText(KitchenFineCardColor color) const
{
    switch(color) {
        case KitchenFineCardColor::Yellow:
            return tr("In progress", "kitchen_fine_status_button");
        case KitchenFineCardColor::Green:
            return tr("Ready", "kitchen_fine_status_button");
        case KitchenFineCardColor::Overdue:
            return tr("Pick up order!", "kitchen_fine_overdue_button");
        case KitchenFineCardColor::Blue:
        default:
            return tr("Accepted", "kitchen_fine_status_button");
    }
}

KitchenFineCardColor DlgKitchenInProgressFine::resolveCardColor(const QJsonArray &lines, bool historyMode) const
{
    if(!historyMode) {
        for(const auto &lv : lines) {
            if(isLineOverdueReady(lv.toObject(), overdueMinutes())) {
                return KitchenFineCardColor::Overdue;
            }
        }
    }

    switch(minStatusInLines(lines)) {
        case 2:
            return KitchenFineCardColor::Yellow;
        case 3:
            return KitchenFineCardColor::Green;
        case 1:
        default:
            return KitchenFineCardColor::Blue;
    }
}

QVector<DlgKitchenInProgressFine::KitchenLinePick> DlgKitchenInProgressFine::buildLinePicks(const QJsonArray &lines) const
{
    QVector<KitchenLinePick> picks;

    for(const auto &lv : lines) {
        const QJsonObject lo = lv.toObject();
        KitchenLinePick p;
        p.lineId = lo.value(QStringLiteral("f_goods_row_id")).toString();
        p.status = lo.value(QStringLiteral("f_status")).toInt();
        p.pickerLabel = formatQtyPickerLabel(lo.value(QStringLiteral("f_qty")).toVariant().toDouble(),
                                             lo.value(QStringLiteral("f_goods_name")).toString());
        picks.append(p);
    }

    return picks;
}

void DlgKitchenInProgressFine::syncTopTabs()
{
    ui->btnTabLive->setChecked(!mKitchenHistoryReadonly);
    ui->btnTabHistory->setChecked(mKitchenHistoryReadonly);
    ui->lbLiveCount->setVisible(!mKitchenHistoryReadonly);
}

void DlgKitchenInProgressFine::updateLiveCountBadge(int count)
{
    ui->lbLiveCount->setText(QString::number(count));
}

void DlgKitchenInProgressFine::clearAllColumns()
{
    for(QVBoxLayout *col : std::as_const(mColumnLayouts)) {
        while(col->count() > 1) {
            QLayoutItem *it = col->takeAt(0);

            if(it->widget()) {
                it->widget()->deleteLater();
            }

            delete it;
        }
    }

    for(QWidget *w : std::as_const(mColumnWidgets)) {
        mCardsColumnsLayout->removeWidget(w);
        w->deleteLater();
    }

    mColumnWidgets.clear();
    mColumnLayouts.clear();
}

void DlgKitchenInProgressFine::ensureColumnCount(int count)
{
    while(mColumnLayouts.size() < count) {
        auto *colWidget = new QWidget(mCardsRoot);
        colWidget->setObjectName(QStringLiteral("kitchenFineColumn"));
        colWidget->setFixedWidth(kKitchenFineCardWidth);

        auto *col = new QVBoxLayout(colWidget);
        col->setContentsMargins(0, 0, 0, 0);
        col->setSpacing(kFineColumnSpacing);
        col->addStretch(1);

        mColumnWidgets.append(colWidget);
        mColumnLayouts.append(col);
        mCardsColumnsLayout->addWidget(colWidget, 0, Qt::AlignTop);
    }
}

void DlgKitchenInProgressFine::trimColumnsTo(int count)
{
    while(mColumnLayouts.size() > count) {
        QWidget *w = mColumnWidgets.takeLast();
        mColumnLayouts.removeLast();
        mCardsColumnsLayout->removeWidget(w);
        w->deleteLater();
    }
}

int DlgKitchenInProgressFine::cardsViewportHeight() const
{
    const int h = ui->scrollCards->viewport()->height();

    if(h > 0) {
        return h;
    }

    return qMax(400, height() - 160);
}

void DlgKitchenInProgressFine::updateCardsScrollMetrics(int columnCount)
{
    const int w = columnCount > 0
                      ? columnCount * kKitchenFineCardWidth + qMax(0, columnCount - 1) * kFineColumnSpacing
                      : kKitchenFineCardWidth;
    const int h = cardsViewportHeight();
    mCardsRoot->setMinimumSize(w, h);
    mCardsRoot->resize(w, h);
}

void DlgKitchenInProgressFine::liveClearCards()
{
    for(const auto &entry : std::as_const(mLiveCardEntries)) {
        if(entry.widget) {
            entry.widget->deleteLater();
        }
    }

    mLiveCardEntries.clear();
    clearAllColumns();
}

void DlgKitchenInProgressFine::historyClearCards()
{
    for(const auto &entry : std::as_const(mHistoryCardEntries)) {
        if(entry.widget) {
            entry.widget->deleteLater();
        }
    }

    mHistoryCardEntries.clear();
    clearAllColumns();
}

KitchenFineOrderCard *DlgKitchenInProgressFine::createOrderCard(const QJsonObject &order,
                                                                 const QJsonArray &lines,
                                                                 const QVector<KitchenLinePick> &picks,
                                                                 bool readonly,
                                                                 int cardIndex)
{
    Q_UNUSED(picks);

    const KitchenFineCardColor color = resolveCardColor(lines, readonly);
    auto *card = new KitchenFineOrderCard(mCardsRoot);
    card->setOrderData(order, lines, color, cardStatusButtonText(color), readonly);

    connect(card, &KitchenFineOrderCard::headerClicked, this, [this, cardIndex]() {
        openDlgOrderForCard(cardIndex);
    });

    connect(card, &KitchenFineOrderCard::statusButtonClicked, this, [this, cardIndex]() {
        openStatusPickerForCard(cardIndex);
    });

    connect(card, &KitchenFineOrderCard::lineClicked, this, [this, cardIndex](const QString &lineId) {
        openStatusPickerForCard(cardIndex, lineId);
    });

    return card;
}

void DlgKitchenInProgressFine::rebuildCardsFromOrders(const QJsonArray &orders,
                                                      bool readonly,
                                                      QVector<KitchenFineCardEntry> &targetEntries)
{
    if(&targetEntries == &mLiveCardEntries) {
        liveClearCards();
    } else {
        historyClearCards();
    }

    if(orders.isEmpty()) {
        updateCardsScrollMetrics(0);
        return;
    }

    const int availH = cardsViewportHeight();
    int col = 0;
    int colUsedH = 0;
    ensureColumnCount(1);

    for(const auto &ov : orders) {
        const QJsonObject order = ov.toObject();
        const QJsonArray lines = order.value(QStringLiteral("lines")).toArray();

        if(lines.isEmpty()) {
            continue;
        }

        KitchenFineCardEntry entry;
        entry.order = order;
        entry.picks = buildLinePicks(lines);
        const int cardIndex = targetEntries.size();
        entry.widget = createOrderCard(order, lines, entry.picks, readonly, cardIndex);
        targetEntries.append(entry);

        entry.widget->adjustSize();
        const int cardH = qMax(entry.widget->sizeHint().height(), entry.widget->minimumSizeHint().height());

        if(colUsedH > 0 && colUsedH + kFineColumnSpacing + cardH > availH) {
            ++col;
            colUsedH = 0;
            ensureColumnCount(col + 1);
        }

        mColumnLayouts[col]->insertWidget(mColumnLayouts[col]->count() - 1, entry.widget);
        colUsedH += (colUsedH > 0 ? kFineColumnSpacing : 0) + cardH;
    }

    trimColumnsTo(col + 1);
    updateCardsScrollMetrics(col + 1);
}

void DlgKitchenInProgressFine::liveRebuildCards()
{
    rebuildCardsFromOrders(liveOrdersForView(), false, mLiveCardEntries);
    updateLiveCountBadge(mLiveCardEntries.size());
}

void DlgKitchenInProgressFine::historyRebuildCards()
{
    rebuildCardsFromOrders(historyOrdersForView(), true, mHistoryCardEntries);
    updateLiveCountBadge(0);
}

void DlgKitchenInProgressFine::refreshLiveCardColors()
{
    for(KitchenFineCardEntry &entry : mLiveCardEntries) {
        if(!entry.widget) {
            continue;
        }

        const QJsonArray lines = entry.order.value(QStringLiteral("lines")).toArray();
        const KitchenFineCardColor color = resolveCardColor(lines, false);
        entry.widget->updateColor(color, cardStatusButtonText(color));
    }
}

QJsonArray DlgKitchenInProgressFine::liveOrdersForView() const
{
    QJsonArray filtered;

    for(const auto &v : mLiveKitchenRows) {
        const QJsonObject order = v.toObject();

        if(!order.value(QStringLiteral("lines")).toArray().isEmpty()) {
            filtered.append(order);
        }
    }

    return filtered;
}

QJsonArray DlgKitchenInProgressFine::historyOrdersForView() const
{
    QJsonArray filtered;

    for(const auto &v : mHistoryKitchenRows) {
        const QJsonObject order = v.toObject();

        if(!order.value(QStringLiteral("lines")).toArray().isEmpty()) {
            filtered.append(order);
        }
    }

    return filtered;
}

void DlgKitchenInProgressFine::liveReflowCardsFromBuffer()
{
    liveRebuildCards();
}

void DlgKitchenInProgressFine::historyReflowCardsFromBuffer()
{
    historyRebuildCards();
}

void DlgKitchenInProgressFine::ingestLiveKitchenDoc(const QJsonObject &jdoc)
{
    mLiveKitchenRows = jsonPayloadDataArray(jdoc);
    sortKitchenRowsByOpenedTime(mLiveKitchenRows);

    if(mKitchenHistoryReadonly) {
        return;
    }

    liveReflowCardsFromBuffer();
}

void DlgKitchenInProgressFine::ingestHistoryKitchenDoc(const QJsonObject &jdoc)
{
    mHistoryKitchenRows = jsonPayloadDataArray(jdoc);
    sortKitchenRowsByOpenedTime(mHistoryKitchenRows);

    if(!mKitchenHistoryReadonly) {
        return;
    }

    historyReflowCardsFromBuffer();
}

void DlgKitchenInProgressFine::reloadLiveKitchenList()
{
    if(mKitchenHistoryReadonly) {
        return;
    }

    ++mLiveKitchenRequestGen;
    const int gen = mLiveKitchenRequestGen;

    fHttp->createHttpQueryLambda(QStringLiteral("/engine/v2/waiter/in-progress/get"),
                                 {},
                                 [this, gen](const QJsonObject &jdoc) {
                                     if(gen != mLiveKitchenRequestGen) {
                                         return;
                                     }

                                     ingestLiveKitchenDoc(jdoc);
                                 },
                                 [](const QJsonObject &) {},
                                 {},
                                 false);
}

void DlgKitchenInProgressFine::loadKitchenHistoryForDate(const QString &historyDateYmd)
{
    ++mHistoryKitchenRequestGen;
    const int gen = mHistoryKitchenRequestGen;

    mHistoryKitchenRows = QJsonArray();

    if(mKitchenHistoryReadonly) {
        historyClearCards();
    }

    fHttp->createHttpQueryLambda(QStringLiteral("/engine/v2/waiter/in-progress/get-history-for-date"),
                                 QJsonObject{{QStringLiteral("date"), historyDateYmd}},
                                 [this, gen](const QJsonObject &jdoc) {
                                     if(gen != mHistoryKitchenRequestGen) {
                                         return;
                                     }

                                     ingestHistoryKitchenDoc(jdoc);
                                 },
                                 [this, gen](const QJsonObject &jerr) {
                                     if(gen != mHistoryKitchenRequestGen) {
                                         return;
                                     }

                                     mHistoryKitchenRows = QJsonArray();

                                     if(mKitchenHistoryReadonly) {
                                         historyReflowCardsFromBuffer();
                                     }

                                     const QString msg = jerr.value(QStringLiteral("errorMessage")).toString();

                                     if(!msg.isEmpty()) {
                                         C5Message::error(msg);
                                     }
                                 },
                                 {},
                                 false);
}

void DlgKitchenInProgressFine::setKitchenHistoryReadonly(bool readonly, const QString &historyDateYmd)
{
    if(readonly && !mKitchenHistoryReadonly) {
        ++mLiveKitchenRequestGen;
    }

    mKitchenHistoryReadonly = readonly;
    mKitchenHistoryDate = readonly ? historyDateYmd : QString();

    if(readonly && !historyDateYmd.isEmpty()) {
        const QDate d = QDate::fromString(historyDateYmd, QStringLiteral("yyyy-MM-dd"));
        const QString dateShown = d.isValid() ? d.toString(FORMAT_DATE_TO_STR) : historyDateYmd;
        setWindowTitle(tr("%1 — %2 (history)", "kitchen_in_progress_title").arg(mOriginalWindowTitle, dateShown));
    } else {
        setWindowTitle(mOriginalWindowTitle);
    }

    syncTopTabs();

    if(readonly) {
        liveClearCards();
    } else {
        historyClearCards();
    }
}

void DlgKitchenInProgressFine::applyKitchenLineStatusToLines(const QVector<QString> &lineIds, int status)
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

void DlgKitchenInProgressFine::runKitchenStatusUpdateChain(const QVector<QString> &lineIds, int index, int status)
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
                                         ingestLiveKitchenDoc(jdoc);
                                     } else {
                                         runKitchenStatusUpdateChain(lineIds, index + 1, status);
                                     }
                                 },
                                 [](const QJsonObject &) {},
                                 {},
                                 false);
}

void DlgKitchenInProgressFine::openStatusPickerForLineIds(const QVector<QString> &lineIds)
{
    if(mKitchenHistoryReadonly || lineIds.isEmpty()) {
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

void DlgKitchenInProgressFine::openStatusPickerForCard(int cardIndex, const QString &singleLineId)
{
    if(mKitchenHistoryReadonly) {
        return;
    }

    if(cardIndex < 0 || cardIndex >= mLiveCardEntries.size()) {
        return;
    }

    const QVector<KitchenLinePick> &picks = mLiveCardEntries.at(cardIndex).picks;

    if(picks.isEmpty()) {
        return;
    }

    if(!singleLineId.isEmpty()) {
        openStatusPickerForLineIds({singleLineId});
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

void DlgKitchenInProgressFine::openDlgOrderForCard(int cardIndex)
{
    if(mKitchenHistoryReadonly) {
        return;
    }

    if(cardIndex < 0 || cardIndex >= mLiveCardEntries.size() || !mHalls || !mTables || !mGroups || !mDishes) {
        return;
    }

    const QJsonObject &order = mLiveCardEntries.at(cardIndex).order;
    const int tableId = order.value(QStringLiteral("f_table")).toInt();
    const int hallId = order.value(QStringLiteral("f_hall")).toInt();
    const QString headerId = order.value(QStringLiteral("f_header_id")).toString().trimmed();

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

    auto *orderDlg = new DlgOrder(mUser, h, t, mGroups, mDishes);
    orderDlg->setOrderId(headerId);
    orderDlg->exec();
    orderDlg->deleteLater();
    reloadLiveKitchenList();
}

void DlgKitchenInProgressFine::showEvent(QShowEvent *e)
{
    C5WaiterDialog::showEvent(e);

    if(e->spontaneous()) {
        return;
    }

    ui->lbTime->setText(QTime::currentTime().toString(FORMAT_TIME_TO_SHORT_STR));

    if(!mKitchenHistoryReadonly && !mDidInitialShowEventLiveLoad) {
        mDidInitialShowEventLiveLoad = true;
        reloadLiveKitchenList();
    }

    mClockTickCounter = 0;
    mClockTimer.start(1000);
}

void DlgKitchenInProgressFine::hideEvent(QHideEvent *e)
{
    mClockTimer.stop();
    C5WaiterDialog::hideEvent(e);
}

void DlgKitchenInProgressFine::resizeEvent(QResizeEvent *e)
{
    C5WaiterDialog::resizeEvent(e);

    if(mKitchenHistoryReadonly) {
        if(!mHistoryCardEntries.isEmpty()) {
            historyRebuildCards();
        } else {
            updateCardsScrollMetrics(mColumnWidgets.size());
        }
    } else if(!mLiveCardEntries.isEmpty()) {
        liveRebuildCards();
    } else {
        updateCardsScrollMetrics(mColumnWidgets.size());
    }
}

void DlgKitchenInProgressFine::tickClock()
{
    ui->lbTime->setText(QTime::currentTime().toString(FORMAT_TIME_TO_SHORT_STR));

    ++mClockTickCounter;

    if(!mKitchenHistoryReadonly && mClockTickCounter % 30 == 0) {
        refreshLiveCardColors();
    }
}

void DlgKitchenInProgressFine::on_btnExit_clicked()
{
    reject();
}

void DlgKitchenInProgressFine::on_btnTabLive_clicked()
{
    if(!mKitchenHistoryReadonly) {
        return;
    }

    setKitchenHistoryReadonly(false);
    reloadLiveKitchenList();
}

void DlgKitchenInProgressFine::on_btnTabHistory_clicked()
{
    if(mKitchenHistoryReadonly) {
        return;
    }

    QDate d;

    if(!pickKitchenHistoryDate(this, d)) {
        syncTopTabs();
        return;
    }

    const QString ymd = d.toString(QStringLiteral("yyyy-MM-dd"));
    setKitchenHistoryReadonly(true, ymd);
    loadKitchenHistoryForDate(ymd);
}
