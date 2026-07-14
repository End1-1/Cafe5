#include "kitchenfineordercard.h"
#include "kitchenfineitemrow.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPixmap>
#include <QPushButton>
#include <QScrollArea>
#include <QSizePolicy>
#include <QStyle>
#include <QTime>
#include <QVBoxLayout>
#include <functional>

namespace
{

QString formatKitchenCardTime(const QJsonObject &order)
{
    const QString t = order.value(QStringLiteral("f_time_open")).toString().trimmed();

    if(t.isEmpty()) {
        return QString();
    }

    const QTime qt = QTime::fromString(t, QStringLiteral("HH:mm:ss"));

    if(qt.isValid()) {
        return qt.toString(QStringLiteral("HH:mm"));
    }

    return t.left(5);
}

class KitchenFineHeader : public QWidget
{
public:
    explicit KitchenFineHeader(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        setObjectName(QStringLiteral("kitchenFineCardHeader"));
        setAttribute(Qt::WA_StyledBackground, true);
        setCursor(Qt::PointingHandCursor);
    }

    std::function<void()> onClicked;

protected:
    void mouseReleaseEvent(QMouseEvent *event) override
    {
        QWidget::mouseReleaseEvent(event);

        if(event->button() == Qt::LeftButton && onClicked) {
            onClicked();
        }
    }
};

} // namespace

KitchenFineOrderCard::KitchenFineOrderCard(QWidget *parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("kitchenFineOrderCard"));
    setAttribute(Qt::WA_StyledBackground, true);
    setFixedWidth(kKitchenFineCardWidth);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    auto *header = new KitchenFineHeader(this);
    mHeader = header;
    header->onClicked = [this]() { emit headerClicked(); };
    auto *headerLay = new QHBoxLayout(header);
    headerLay->setContentsMargins(14, 12, 14, 12);
    headerLay->setSpacing(8);

    mLbOrderPrefix = new QLabel(this);
    mLbOrderPrefix->setObjectName(QStringLiteral("kitchenFineOrderPrefix"));
    headerLay->addWidget(mLbOrderPrefix, 0, Qt::AlignVCenter);

    headerLay->addStretch(1);

    auto *timeWrap = new QWidget(header);
    timeWrap->setObjectName(QStringLiteral("kitchenFineTimeWrap"));
    auto *timeLay = new QHBoxLayout(timeWrap);
    timeLay->setContentsMargins(10, 4, 10, 4);
    timeLay->setSpacing(6);

    auto *clockIcon = new QLabel(timeWrap);
    clockIcon->setObjectName(QStringLiteral("kitchenFineClockIcon"));
    clockIcon->setPixmap(QPixmap(QStringLiteral(":/alarm-clock.png")).scaled(18, 18, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    timeLay->addWidget(clockIcon);

    mLbTime = new QLabel(timeWrap);
    mLbTime->setObjectName(QStringLiteral("kitchenFineOrderTime"));
    timeLay->addWidget(mLbTime);

    headerLay->addWidget(timeWrap, 0, Qt::AlignVCenter);
    root->addWidget(mHeader);

    auto *itemsHost = new QWidget(this);
    itemsHost->setObjectName(QStringLiteral("kitchenFineItemsHost"));
    auto *itemsHostLay = new QVBoxLayout(itemsHost);
    itemsHostLay->setContentsMargins(10, 10, 10, 10);
    itemsHostLay->setSpacing(0);

    mScrollItems = new QScrollArea(itemsHost);
    mScrollItems->setObjectName(QStringLiteral("kitchenFineItemsScroll"));
    mScrollItems->setWidgetResizable(true);
    mScrollItems->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mScrollItems->setFrameShape(QFrame::NoFrame);

    auto *itemsBody = new QWidget();
    itemsBody->setObjectName(QStringLiteral("kitchenFineItemsBody"));
    mItemsLayout = new QVBoxLayout(itemsBody);
    mItemsLayout->setContentsMargins(0, 0, 0, 0);
    mItemsLayout->setSpacing(8);
    mItemsLayout->addStretch(1);
    mScrollItems->setWidget(itemsBody);
    itemsHostLay->addWidget(mScrollItems);

    root->addWidget(itemsHost, 1);

    mBtnStatus = new QPushButton(this);
    mBtnStatus->setObjectName(QStringLiteral("kitchenFineStatusButton"));
    mBtnStatus->setMinimumHeight(52);
    mBtnStatus->setCursor(Qt::PointingHandCursor);
    connect(mBtnStatus, &QPushButton::clicked, this, &KitchenFineOrderCard::statusButtonClicked);
    root->addWidget(mBtnStatus);
}

void KitchenFineOrderCard::setOrderData(const QJsonObject &order,
                                        const QJsonArray &lines,
                                        KitchenFineCardColor color,
                                        const QString &statusButtonText,
                                        bool readonly)
{
    mOrder = order;
    mReadonly = readonly;
    mColor = color;

    mLbOrderPrefix->setText(order.value(QStringLiteral("f_order_prefix")).toString().trimmed());
    mLbTime->setText(formatKitchenCardTime(order));
    mBtnStatus->setText(statusButtonText);
    mBtnStatus->setEnabled(!readonly);

    while(mItemsLayout->count() > 1) {
        QLayoutItem *it = mItemsLayout->takeAt(0);

        if(it->widget()) {
            it->widget()->deleteLater();
        }

        delete it;
    }

    mItemRows.clear();

    for(const auto &lv : lines) {
        const QJsonObject lo = lv.toObject();
        const QString lineId = lo.value(QStringLiteral("f_goods_row_id")).toString();
        const double qty = lo.value(QStringLiteral("f_qty")).toVariant().toDouble();
        const QString name = lo.value(QStringLiteral("f_goods_name")).toString();
        const QString comment = lo.value(QStringLiteral("f_comment")).toString();

        auto *row = new KitchenFineItemRow(qty, name, comment, this);
        row->setLineId(lineId);
        row->setEnabled(!readonly);

        connect(row, &KitchenFineItemRow::clicked, this, [this, lineId]() {
            emit lineClicked(lineId);
        });

        mItemsLayout->insertWidget(mItemsLayout->count() - 1, row);
        mItemRows.append(row);
    }

    const int itemCount = lines.size();
    const int maxVisibleItems = 4;
    const int rowHeight = 72;
    mScrollItems->setMinimumHeight(qMin(itemCount, maxVisibleItems) * rowHeight);
    mScrollItems->setMaximumHeight(maxVisibleItems * rowHeight);

    applyColorTheme(color);
}

void KitchenFineOrderCard::updateColor(KitchenFineCardColor color, const QString &statusButtonText)
{
    mColor = color;
    mBtnStatus->setText(statusButtonText);
    applyColorTheme(color);
}

void KitchenFineOrderCard::applyColorTheme(KitchenFineCardColor color)
{
    const char *colorKey = "blue";

    switch(color) {
        case KitchenFineCardColor::Yellow:
            colorKey = "yellow";
            break;
        case KitchenFineCardColor::Green:
            colorKey = "green";
            break;
        case KitchenFineCardColor::Overdue:
            colorKey = "red";
            break;
        case KitchenFineCardColor::Blue:
        default:
            colorKey = "blue";
            break;
    }

    setProperty("card_color", colorKey);
    mHeader->setProperty("card_color", colorKey);
    mBtnStatus->setProperty("card_color", colorKey);

    style()->unpolish(this);
    style()->polish(this);
    style()->unpolish(mHeader);
    style()->polish(mHeader);
    style()->unpolish(mBtnStatus);
    style()->polish(mBtnStatus);
}
