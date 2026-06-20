#include "dishcardwidget.h"

#include "menutypes.h"

#include "dishimageutils.h"

#include <QLabel>
#include <QMouseEvent>
#include <QPixmap>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

DishCardWidget::DishCardWidget(Style style, const MenuDish &dish, QWidget *parent)
    : QFrame(parent)
    , m_dishId(dish.id)
{
    if (style == Style::Grid) {
        setObjectName(QStringLiteral("dishCardGrid"));
        buildGridCard(dish);
    } else if (style == Style::Listing) {
        setObjectName(QStringLiteral("dishCardListing"));
        buildTileCard(dish, "dishCardListing");
    } else {
        setObjectName(QStringLiteral("dishCardPopular"));
        buildTileCard(dish, "dishCardPopular");
    }

    if (style == Style::Listing || style == Style::Popular) {
        setCursor(Qt::PointingHandCursor);
    }
}

void DishCardWidget::buildGridCard(const MenuDish &dish)
{
    setFixedSize(MenuLayout::kGridCardWidth, MenuLayout::kGridCardHeight);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(20, 16, 20, 16);
    layout->setSpacing(16);

    auto *name = new QLabel(dish.name, this);
    name->setObjectName(QStringLiteral("dishGridName"));
    name->setWordWrap(true);
    name->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    layout->addWidget(name, 1);

    auto *image = new QLabel(this);
    image->setObjectName(QStringLiteral("dishGridImage"));
    setDishImageOnLabel(image, dish.imagePath, 154, 150);
    layout->addWidget(image);
}

void DishCardWidget::buildTileCard(const MenuDish &dish, const char *cardObjectName)
{
    Q_UNUSED(cardObjectName);

    const int w = (objectName() == QStringLiteral("dishCardListing"))
                      ? MenuLayout::kListingCardWidth
                      : MenuLayout::kPopularCardWidth;
    const int h = (objectName() == QStringLiteral("dishCardListing"))
                      ? MenuLayout::kListingCardHeight
                      : MenuLayout::kPopularCardHeight;

    setFixedSize(w, h);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(8);

    auto *image = new QLabel(this);
    image->setObjectName(QStringLiteral("dishTileImage"));
    setDishImageOnLabel(image, dish.imagePath, 212, 212);
    layout->addWidget(image, 0, Qt::AlignHCenter);

    auto *title = new QLabel(dish.name, this);
    title->setObjectName(QStringLiteral("dishPopularTitle"));
    layout->addWidget(title);

    QString subtitleText = dish.groupName;
    if (!dish.attrType.isEmpty() || !dish.attrSize.isEmpty()) {
        QStringList parts;
        if (!dish.attrType.isEmpty()) {
            parts << dish.attrType;
        }
        if (!dish.attrSize.isEmpty()) {
            QString sizeText = dish.attrSize;
            if (!dish.attrMeasurement.isEmpty()) {
                sizeText += QLatin1Char(' ') + dish.attrMeasurement;
            }
            parts << sizeText;
        }
        subtitleText = parts.join(QStringLiteral(" · "));
    }
    auto *subtitle = new QLabel(subtitleText, this);
    subtitle->setObjectName(QStringLiteral("dishPopularSubtitle"));
    layout->addWidget(subtitle);

    // Dietary / allergen badges
    int badgeCount = 0;
    auto *badgesHost = new QWidget(this);
    auto *badgesLayout = new QHBoxLayout(badgesHost);
    badgesLayout->setContentsMargins(0, 0, 0, 0);
    badgesLayout->setSpacing(6);

    const auto addBadgeIcon = [&](bool enabled, const char *iconPath) {
        if (!enabled) {
            return;
        }
        auto *icon = new QLabel(badgesHost);
        icon->setFixedSize(24, 24);
        icon->setScaledContents(true);
        icon->setPixmap(QPixmap(QString::fromUtf8(iconPath)));
        badgesLayout->addWidget(icon);
        ++badgeCount;
    };

    addBadgeIcon(dish.glutenFree, ":/dietary/gluten-free.png");
    addBadgeIcon(dish.vegetarian, ":/dietary/vegitarian.png");
    addBadgeIcon(dish.vegan, ":/dietary/vegan.png");
    addBadgeIcon(dish.noGmo, ":/dietary/no-gmo.png");
    addBadgeIcon(dish.noLactose, ":/dietary/lactose-free.png");
    addBadgeIcon(dish.noSugar, ":/dietary/sugar-free.png");
    addBadgeIcon(dish.containsNuts, ":/dietary/contain-nuts.png");
    if (dish.halalKosher) {
        addBadgeIcon(true, ":/dietary/halal.png");
        addBadgeIcon(true, ":/dietary/kosher.png");
    }

    if (badgeCount > 0) {
        layout->addWidget(badgesHost);
    } else {
        badgesHost->hide();
    }

    auto *bottom = new QHBoxLayout();
    auto *time = new QLabel(dish.prepTime, this);
    time->setObjectName(QStringLiteral("dishPopularTime"));
    bottom->addWidget(time, 1);

    auto *price = new QLabel(tr("%1 ֏").arg(QString::number(dish.price, 'f', 0)), this);
    price->setObjectName(QStringLiteral("dishPopularPrice"));
    bottom->addWidget(price);

    m_infoButton = new QPushButton(QStringLiteral("i"), this);
    m_infoButton->setObjectName(QStringLiteral("dishPopularInfo"));
    m_infoButton->setFixedSize(28, 28);
    m_infoButton->setCursor(Qt::ArrowCursor);
    connect(m_infoButton, &QPushButton::clicked, this, [this]() { emit infoClicked(m_dishId); });
    bottom->addWidget(m_infoButton);

    layout->addLayout(bottom);
}

void DishCardWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_infoButton) {
        const QPoint local = m_infoButton->mapFrom(this, event->pos());
        if (m_infoButton->rect().contains(local)) {
            QFrame::mousePressEvent(event);
            return;
        }
        emit addToCartClicked(m_dishId);
    } else if (event->button() == Qt::LeftButton) {
        emit addToCartClicked(m_dishId);
    }
    QFrame::mousePressEvent(event);
}
