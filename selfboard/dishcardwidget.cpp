#include "dishcardwidget.h"

#include "menutypes.h"

#include "dishimageutils.h"

#include <QLabel>
#include <QMouseEvent>
#include <QPixmap>
#include <QColor>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPalette>

DishCardWidget::DishCardWidget(Style style, const MenuDish &dish, QWidget *parent)
    : QFrame(parent)
    , m_dishId(dish.id)
    , m_imagePath(dish.imagePath)
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

    setAttribute(Qt::WA_StyledBackground, true);
    setAutoFillBackground(true);

    const QColor white(0xff, 0xff, 0xff);
    QPalette cardPalette = palette();
    cardPalette.setColor(QPalette::Window, white);
    cardPalette.setColor(QPalette::Base, white);
    setPalette(cardPalette);

    const QString radius = (objectName() == QStringLiteral("dishCardGrid")) ? QStringLiteral("20px")
                                                                            : QStringLiteral("16px");
    setStyleSheet(QStringLiteral(
                      "QFrame#%1 {"
                      "  background-color: #ffffff;"
                      "  border: 1px solid #e0e0e0;"
                      "  border-radius: %2;"
                      "}"
                      "QFrame#%1 QLabel {"
                      "  background: transparent;"
                      "  border: none;"
                      "}")
                      .arg(objectName(), radius));

    applyCardLabelStyles();
}

void DishCardWidget::applyCardLabelStyles()
{
    for (QLabel *label : findChildren<QLabel *>()) {
        if (!label->pixmap().isNull()) {
            styleTransparentImageLabel(label);
        } else {
            styleTransparentTextLabel(label);
        }
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
    setCenteredDishImageOnLabel(image, dish.imagePath, 150, 154);
    layout->addWidget(image, 0, Qt::AlignVCenter);
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
    const int imageSide = w - 24;
    setCenteredDishImageOnLabel(image, dish.imagePath, imageSide, imageSide);

    auto *imageRow = new QWidget(this);
    imageRow->setAutoFillBackground(false);
    imageRow->setStyleSheet(QStringLiteral("background: transparent;"));
    auto *imageRowLayout = new QHBoxLayout(imageRow);
    imageRowLayout->setContentsMargins(0, 0, 0, 0);
    imageRowLayout->setSpacing(0);
    imageRowLayout->addStretch();
    imageRowLayout->addWidget(image, 0, Qt::AlignCenter);
    imageRowLayout->addStretch();
    layout->addWidget(imageRow);

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
    badgesHost->setAutoFillBackground(false);
    badgesHost->setStyleSheet(QStringLiteral("background: transparent;"));
    auto *badgesLayout = new QHBoxLayout(badgesHost);
    badgesLayout->setContentsMargins(0, 0, 0, 0);
    badgesLayout->setSpacing(6);

    const auto addBadgeIcon = [&](bool enabled, const char *iconPath) {
        if (!enabled) {
            return;
        }
        auto *icon = new QLabel(badgesHost);
        icon->setFixedSize(24, 24);
        setTransparentPixmapOnLabel(icon, QPixmap(QString::fromUtf8(iconPath)), 24, 24);
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
    if (dish.prepTime.isEmpty()) {
        time->hide();
    }
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

int DishCardWidget::dishId() const
{
    return m_dishId;
}

QPixmap DishCardWidget::thumbnailPixmap() const
{
    const auto *tileImage = findChild<QLabel *>(QStringLiteral("dishTileImage"));
    if (tileImage && !tileImage->pixmap().isNull()) {
        return tileImage->pixmap();
    }

    const auto *gridImage = findChild<QLabel *>(QStringLiteral("dishGridImage"));
    if (gridImage && !gridImage->pixmap().isNull()) {
        return gridImage->pixmap();
    }

    return loadDishPixmap(m_imagePath);
}

QPoint DishCardWidget::flyStartGlobalPos() const
{
    const auto *tileImage = findChild<QLabel *>(QStringLiteral("dishTileImage"));
    if (tileImage) {
        return tileImage->mapToGlobal(tileImage->rect().center());
    }

    const auto *gridImage = findChild<QLabel *>(QStringLiteral("dishGridImage"));
    if (gridImage) {
        return gridImage->mapToGlobal(gridImage->rect().center());
    }

    return mapToGlobal(rect().center());
}
