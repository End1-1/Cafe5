#include "dlgdishdetails.h"

#include "dishimageutils.h"

#include <QFile>
#include <QFrame>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

namespace {

constexpr int kCardSideMargin = 32;
constexpr int kCardMaxWidth = 720;
constexpr int kImageSize = 160;

void addBadgeIcon(QWidget *host, QHBoxLayout *layout, bool enabled, const char *iconPath)
{
    if (!enabled) {
        return;
    }
    auto *icon = new QLabel(host);
    icon->setObjectName(QStringLiteral("dishDetailsBadgeIcon"));
    icon->setFixedSize(40, 40);
    icon->setAlignment(Qt::AlignCenter);
    icon->setScaledContents(true);
    icon->setPixmap(QPixmap(QString::fromUtf8(iconPath)));
    layout->addWidget(icon, 0, Qt::AlignVCenter);
}

QFrame *addBjuBox(QWidget *host, QHBoxLayout *layout, const QString &valueText, const QString &title)
{
    auto *box = new QFrame(host);
    box->setObjectName(QStringLiteral("dishDetailsBjuBox"));
    auto *boxLayout = new QVBoxLayout(box);
    boxLayout->setContentsMargins(10, 8, 10, 8);
    boxLayout->setSpacing(6);

    auto *valueLbl = new QLabel(valueText, box);
    valueLbl->setObjectName(QStringLiteral("dishDetailsBjuValue"));
    valueLbl->setAlignment(Qt::AlignCenter);

    auto *divider = new QFrame(box);
    divider->setObjectName(QStringLiteral("dishDetailsBjuDivider"));
    divider->setFixedHeight(1);

    auto *titleLbl = new QLabel(title, box);
    titleLbl->setObjectName(QStringLiteral("dishDetailsBjuTitle"));
    titleLbl->setAlignment(Qt::AlignCenter);

    boxLayout->addWidget(valueLbl);
    boxLayout->addWidget(divider);
    boxLayout->addWidget(titleLbl);
    layout->addWidget(box, 0, Qt::AlignVCenter);
    return box;
}

} // namespace

DlgDishDetails::DlgDishDetails(const MenuDish &dish, QWidget *parent)
    : QWidget(parent)
    , m_dish(dish)
{
    setObjectName(QStringLiteral("DlgDishDetails"));
    setAttribute(Qt::WA_StyledBackground, true);
    setFocusPolicy(Qt::StrongFocus);

    QFile styleFile(QStringLiteral(":/res/selfboard.css"));
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setStyleSheet(QString::fromUtf8(styleFile.readAll()));
    }

    buildUi();
    syncCardGeometry();
}

void DlgDishDetails::buildUi()
{
    m_card = new QFrame(this);
    m_card->setObjectName(QStringLiteral("dishDetailsCard"));
    m_card->setAttribute(Qt::WA_StyledBackground, true);

    auto *cardLayout = new QVBoxLayout(m_card);
    cardLayout->setContentsMargins(24, 20, 24, 24);
    cardLayout->setSpacing(0);

    auto *headerRow = new QHBoxLayout();
    headerRow->setContentsMargins(0, 0, 0, 0);
    headerRow->setSpacing(0);

    auto *titleSpacer = new QWidget(m_card);
    titleSpacer->setFixedSize(44, 44);
    headerRow->addWidget(titleSpacer);

    auto *title = new QLabel(tr("Dish details"), m_card);
    title->setObjectName(QStringLiteral("dishDetailsDialogTitle"));
    title->setAlignment(Qt::AlignCenter);
    headerRow->addWidget(title, 1);

    auto *btnClose = new QPushButton(QStringLiteral("×"), m_card);
    btnClose->setObjectName(QStringLiteral("dishDetailsCloseBtn"));
    btnClose->setFixedSize(44, 44);
    btnClose->setCursor(Qt::PointingHandCursor);
    connect(btnClose, &QPushButton::clicked, this, [this]() { emit closed(); });
    headerRow->addWidget(btnClose);
    cardLayout->addLayout(headerRow);

    auto *headerDivider = new QFrame(m_card);
    headerDivider->setObjectName(QStringLiteral("dishDetailsDivider"));
    headerDivider->setFixedHeight(1);
    cardLayout->addSpacing(16);
    cardLayout->addWidget(headerDivider);
    cardLayout->addSpacing(20);

    auto *scroll = new QScrollArea(m_card);
    scroll->setObjectName(QStringLiteral("dishDetailsScroll"));
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setStyleSheet(QStringLiteral(
        "QScrollArea#dishDetailsScroll { background: transparent; border: none; }"
        "QScrollArea#dishDetailsScroll > QWidget > QWidget { background: transparent; }"));

    auto *content = new QWidget();
    content->setObjectName(QStringLiteral("dishDetailsContent"));
    auto *contentLayout = new QVBoxLayout(content);
    contentLayout->setContentsMargins(0, 0, 8, 0);
    contentLayout->setSpacing(16);

    auto *heroRow = new QHBoxLayout();
    heroRow->setSpacing(20);
    heroRow->setContentsMargins(0, 0, 0, 0);

    auto *image = new QLabel(content);
    image->setObjectName(QStringLiteral("dishDetailsImage"));
    image->setFixedSize(kImageSize, kImageSize);
    image->setAlignment(Qt::AlignCenter);
    setDishImageOnLabel(image, m_dish.imagePath, kImageSize, kImageSize);
    heroRow->addWidget(image, 0, Qt::AlignTop);

    auto *infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(8);
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    auto *name = new QLabel(m_dish.name, content);
    name->setObjectName(QStringLiteral("dishDetailsName"));
    name->setWordWrap(true);
    name->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    infoLayout->addWidget(name);

    if (!m_dish.groupName.isEmpty()) {
        auto *group = new QLabel(m_dish.groupName, content);
        group->setObjectName(QStringLiteral("dishDetailsGroup"));
        group->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        infoLayout->addWidget(group);
    }

    if (!m_dish.prepTime.isEmpty()) {
        auto *prep = new QLabel(m_dish.prepTime, content);
        prep->setObjectName(QStringLiteral("dishDetailsPrepTime"));
        prep->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        infoLayout->addWidget(prep);
    }

    auto *price = new QLabel(tr("%1 AMD").arg(QString::number(m_dish.price, 'f', 0)), content);
    price->setObjectName(QStringLiteral("dishDetailsPrice"));
    price->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    infoLayout->addWidget(price);

    infoLayout->addStretch();
    heroRow->addLayout(infoLayout, 1);
    contentLayout->addLayout(heroRow);

    if (!m_dish.description.isEmpty()) {
        auto *description = new QLabel(m_dish.description, content);
        description->setObjectName(QStringLiteral("dishDetailsDescription"));
        description->setWordWrap(true);
        description->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        contentLayout->addWidget(description);
    }

    auto *badgesRow = new QWidget(content);
    badgesRow->setObjectName(QStringLiteral("dishDetailsBadgesRow"));
    auto *badgesLayout = new QHBoxLayout(badgesRow);
    badgesLayout->setContentsMargins(0, 0, 0, 0);
    badgesLayout->setSpacing(8);
    badgesLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    addBadgeIcon(badgesRow, badgesLayout, m_dish.glutenFree, ":/dietary/gluten-free.png");
    addBadgeIcon(badgesRow, badgesLayout, m_dish.vegetarian, ":/dietary/vegitarian.png");
    addBadgeIcon(badgesRow, badgesLayout, m_dish.vegan, ":/dietary/vegan.png");
    addBadgeIcon(badgesRow, badgesLayout, m_dish.noGmo, ":/dietary/no-gmo.png");
    addBadgeIcon(badgesRow, badgesLayout, m_dish.noLactose, ":/dietary/lactose-free.png");
    addBadgeIcon(badgesRow, badgesLayout, m_dish.noSugar, ":/dietary/sugar-free.png");
    addBadgeIcon(badgesRow, badgesLayout, m_dish.containsNuts, ":/dietary/contain-nuts.png");
    if (m_dish.halalKosher) {
        addBadgeIcon(badgesRow, badgesLayout, true, ":/dietary/halal.png");
        addBadgeIcon(badgesRow, badgesLayout, true, ":/dietary/kosher.png");
    }
    badgesLayout->addStretch();
    if (badgesLayout->count() > 1) {
        contentLayout->addWidget(badgesRow);
    }

    auto *bjuRow = new QWidget(content);
    bjuRow->setObjectName(QStringLiteral("dishDetailsBjuRow"));
    auto *bjuLayout = new QHBoxLayout(bjuRow);
    bjuLayout->setContentsMargins(0, 0, 0, 0);
    bjuLayout->setSpacing(8);
    bjuLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    if (m_dish.fat > 0.0) {
        addBjuBox(bjuRow, bjuLayout,
                  tr("%1 gr").arg(QString::number(m_dish.fat, 'f', 0)),
                  tr("Fats"));
    }
    if (m_dish.carbs > 0.0) {
        addBjuBox(bjuRow, bjuLayout,
                  tr("%1 gr").arg(QString::number(m_dish.carbs, 'f', 0)),
                  tr("Carbs"));
    }
    if (m_dish.protein > 0.0) {
        addBjuBox(bjuRow, bjuLayout,
                  tr("%1 gr").arg(QString::number(m_dish.protein, 'f', 0)),
                  tr("Protein"));
    }
    if (m_dish.kcal > 0.0) {
        addBjuBox(bjuRow, bjuLayout,
                  tr("%1 kcal").arg(QString::number(m_dish.kcal, 'f', m_dish.kcal >= 100 ? 0 : 1)),
                  tr("Calories"));
    }
    bjuLayout->addStretch();
    if (bjuLayout->count() > 1) {
        contentLayout->addWidget(bjuRow);
    }

    QStringList attrLines;
    if (!m_dish.attrType.isEmpty()) {
        attrLines << tr("Type: %1").arg(m_dish.attrType);
    }
    if (!m_dish.attrSize.isEmpty()) {
        QString sizeText = m_dish.attrSize;
        if (!m_dish.attrMeasurement.isEmpty()) {
            sizeText += QLatin1Char(' ') + m_dish.attrMeasurement;
        }
        attrLines << tr("Size: %1").arg(sizeText);
    }
    if (!attrLines.isEmpty()) {
        auto *attrs = new QLabel(attrLines.join(QStringLiteral("\n")), content);
        attrs->setObjectName(QStringLiteral("dishDetailsAttributes"));
        attrs->setWordWrap(true);
        attrs->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        contentLayout->addWidget(attrs);
    }

    if (!m_dish.selectedModificators.isEmpty()) {
        QStringList modLines;
        for (const MenuSelectedModificator &modificator : m_dish.selectedModificators) {
            modLines << tr("• %1").arg(modificator.name);
        }
        auto *mods = new QLabel(modLines.join(QStringLiteral("\n")), content);
        mods->setObjectName(QStringLiteral("dishDetailsModifiers"));
        mods->setWordWrap(true);
        mods->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        contentLayout->addWidget(mods);
    }

    contentLayout->addStretch();
    scroll->setWidget(content);
    cardLayout->addWidget(scroll, 1);
}

void DlgDishDetails::syncCardGeometry()
{
    if (!m_card) {
        return;
    }

    const int cardWidth = qMin(kCardMaxWidth, qMax(320, width() - kCardSideMargin * 2));
    const int cardHeight = qMax(360, height() - 120);
    const int x = (width() - cardWidth) / 2;
    const int y = qMax(48, (height() - cardHeight) / 2);
    m_card->setGeometry(x, y, cardWidth, cardHeight);
}

void DlgDishDetails::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(rect(), QColor(0, 0, 0, 140));
}

void DlgDishDetails::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    syncCardGeometry();
}

void DlgDishDetails::mousePressEvent(QMouseEvent *event)
{
    if (m_card && !m_card->geometry().contains(event->pos())) {
        emit closed();
        event->accept();
        return;
    }
    QWidget::mousePressEvent(event);
}

void DlgDishDetails::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape
        || (event->key() == Qt::Key_F4 && (event->modifiers() & Qt::AltModifier))) {
        emit closed();
        event->accept();
        return;
    }
    QWidget::keyPressEvent(event);
}
