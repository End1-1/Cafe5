#include "dishcardwidget.h"

#include "menutypes.h"

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
    image->setFixedSize(150, 154);
    image->setScaledContents(true);
    image->setPixmap(QPixmap(dish.imagePath));
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
    image->setFixedSize(212, 212);
    image->setScaledContents(true);
    image->setPixmap(QPixmap(dish.imagePath));
    layout->addWidget(image, 0, Qt::AlignHCenter);

    auto *title = new QLabel(dish.name, this);
    title->setObjectName(QStringLiteral("dishPopularTitle"));
    layout->addWidget(title);

    auto *subtitle = new QLabel(dish.groupName, this);
    subtitle->setObjectName(QStringLiteral("dishPopularSubtitle"));
    layout->addWidget(subtitle);

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
