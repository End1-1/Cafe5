#include "dlgmenu.h"

#include "dlgdishdetails.h"
#include "dishcardwidget.h"
#include "ui_dlgmenu.h"

#include <QAbstractButton>
#include <QButtonGroup>
#include <QEvent>
#include <QFile>
#include <QIcon>
#include <QLayoutItem>
#include <QLineEdit>
#include <QPixmap>
#include <QPushButton>
#include <QScroller>
#include <QSettings>
#include <QToolButton>

namespace {

QPushButton *makeSidebarGroupButton(const MenuGroup &group, QWidget *parent)
{
    auto *btn = new QPushButton(parent);
    btn->setObjectName(QStringLiteral("btnMenuGroup"));
    btn->setCheckable(true);
    btn->setText(group.name);
    btn->setProperty("groupId", group.id);

    const QPixmap icon(group.iconPath);
    if (!icon.isNull()) {
        btn->setIcon(QIcon(icon));
        btn->setIconSize(QSize(40, 40));
    }
    btn->setMinimumHeight(56);
    return btn;
}

QPushButton *makeChipButton(const MenuGroup &group, QWidget *parent)
{
    auto *btn = new QPushButton(group.name, parent);
    btn->setObjectName(QStringLiteral("btnGroupChip"));
    btn->setCheckable(true);
    btn->setMinimumHeight(44);
    btn->setMinimumWidth(100);
    return btn;
}

} // namespace

DlgMenu::DlgMenu(ServiceMode mode, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgMenu)
    , m_serviceMode(mode)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setFixedSize(1080, 1920);

    m_groupsLayout = ui->groupsHostLayout;
    m_chipsLayout = ui->chipsLayout;
    m_dishGridLayout = ui->dishGridLayout;

    m_dishGridLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_dishGridLayout->setColumnStretch(MenuLayout::kListingColumns, 1);
    ui->scrollDishes->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    ui->widgetDishesHost->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    setupAppearance();
    setupTouchScroll();

    ui->lblSidebarLogo->setPixmap(QPixmap(QStringLiteral(":/res/main_logo.png")));
    ui->lblOrderQr->setPixmap(QPixmap(QStringLiteral(":/res/qr_placeholder.png")));
    ui->lblFooterLogo->setPixmap(QPixmap(QStringLiteral(":/res/footer_logo_myqr.png")));
    ui->btnYoutube->setIcon(QIcon(QStringLiteral(":/res/icon_youtube.png")));
    ui->btnFacebook->setIcon(QIcon(QStringLiteral(":/res/icon_facebook.png")));
    ui->btnYoutube->setIconSize(QSize(36, 36));
    ui->btnFacebook->setIconSize(QSize(36, 36));
    ui->btnFavorites->setText(QStringLiteral("♥"));

    updateServiceModeLabel();

    m_groupButtons = new QButtonGroup(this);
    m_groupButtons->setExclusive(true);
    connect(m_groupButtons, &QButtonGroup::idClicked, this, &DlgMenu::onGroupClicked);

    m_chipButtons = new QButtonGroup(this);
    m_chipButtons->setExclusive(true);
    connect(m_chipButtons, &QButtonGroup::idClicked, this, &DlgMenu::onGroupClicked);

    buildSidebarGroups();
    buildGroupChips();

    const QVector<MenuGroup> groups = m_menuClient.groups();
    if (!groups.isEmpty()) {
        selectGroup(groups.first().id);
    }

    updateCartSummary();

    connect(ui->btnHome, &QPushButton::clicked, this, &QDialog::reject);
    connect(ui->btnCancelOrder, &QPushButton::clicked, this, &DlgMenu::onCancelOrder);
    connect(ui->btnGoToCart, &QPushButton::clicked, this, &DlgMenu::onGoToCart);
    connect(ui->leSearch, &QLineEdit::textChanged, this, &DlgMenu::onSearchTextChanged);
}

DlgMenu::~DlgMenu()
{
    delete ui;
}

void DlgMenu::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        updateServiceModeLabel();
    }
    QDialog::changeEvent(event);
}

void DlgMenu::setupAppearance()
{
    QFile styleFile(QStringLiteral(":/res/selfboard.css"));
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setStyleSheet(QString::fromUtf8(styleFile.readAll()));
    }

    QSettings settings(QStringLiteral("Jazzve"), QStringLiteral("SelfBoard"));
    const QString locale = settings.value(QStringLiteral("locale"), QStringLiteral("en")).toString();
    if (locale == QStringLiteral("ru")) {
        ui->btnLanguage->setText(tr("Русский"));
    } else if (locale == QStringLiteral("hy")) {
        ui->btnLanguage->setText(tr("Հայերեն"));
    } else {
        ui->btnLanguage->setText(tr("English"));
    }
}

void DlgMenu::setupTouchScroll()
{
    QScroller::grabGesture(ui->scrollGroupChips->viewport(), QScroller::LeftMouseButtonGesture);
    QScroller::grabGesture(ui->scrollDishes->viewport(), QScroller::LeftMouseButtonGesture);
}

void DlgMenu::updateServiceModeLabel()
{
    if (m_serviceMode == ServiceMode::TakeAway) {
        ui->btnServiceMode->setText(tr("Take away"));
        ui->btnServiceMode->setIcon(QIcon(QStringLiteral(":/res/icon_takeaway.png")));
    } else {
        ui->btnServiceMode->setText(tr("Dine in"));
        ui->btnServiceMode->setIcon(QIcon(QStringLiteral(":/res/icon_dinein.png")));
    }
    ui->btnServiceMode->setIconSize(QSize(28, 28));
}

void DlgMenu::buildSidebarGroups()
{
    clearLayout(m_groupsLayout);

    for (const MenuGroup &group : m_menuClient.groups()) {
        QPushButton *btn = makeSidebarGroupButton(group, ui->widgetGroupsHost);
        m_groupButtons->addButton(btn, group.id);
        m_groupsLayout->addWidget(btn);
    }
    m_groupsLayout->addStretch();
}

void DlgMenu::buildGroupChips()
{
    clearLayout(m_chipsLayout);

    for (const MenuGroup &group : m_menuClient.groups()) {
        QPushButton *btn = makeChipButton(group, ui->widgetChipsHost);
        m_chipButtons->addButton(btn, group.id);
        m_chipsLayout->addWidget(btn);
    }
    m_chipsLayout->addStretch();
}

void DlgMenu::selectGroup(int groupId)
{
    m_currentGroupId = groupId;

    if (QAbstractButton *btn = m_groupButtons->button(groupId)) {
        btn->setChecked(true);
    }
    if (QAbstractButton *chip = m_chipButtons->button(groupId)) {
        chip->setChecked(true);
        ui->scrollGroupChips->ensureWidgetVisible(chip, 24, 0);
    }

    rebuildDishGrid();
}

void DlgMenu::onGroupClicked(int groupId)
{
    selectGroup(groupId);
}

QVector<MenuDish> DlgMenu::filteredDishes() const
{
    const QVector<MenuDish> all = m_menuClient.dishesByGroup(m_currentGroupId);
    const QString needle = ui->leSearch->text().trimmed();
    if (needle.isEmpty()) {
        return all;
    }

    QVector<MenuDish> result;
    for (const MenuDish &dish : all) {
        if (dish.name.contains(needle, Qt::CaseInsensitive)
            || dish.groupName.contains(needle, Qt::CaseInsensitive)) {
            result.append(dish);
        }
    }
    return result;
}

void DlgMenu::rebuildDishGrid()
{
    clearLayout(m_dishGridLayout);

    const QVector<MenuDish> dishes = filteredDishes();
    int row = 0;
    int col = 0;

    for (const MenuDish &dish : dishes) {
        auto *card = new DishCardWidget(DishCardWidget::Style::Listing, dish, ui->widgetDishesHost);
        connect(card, &DishCardWidget::addToCartClicked, this, &DlgMenu::onAddToCart);
        connect(card, &DishCardWidget::infoClicked, this, &DlgMenu::onDishInfo);
        m_dishGridLayout->addWidget(card, row, col);

        ++col;
        if (col >= MenuLayout::kListingColumns) {
            col = 0;
            ++row;
        }
    }

    const int itemCount = dishes.size();
    const int rows = itemCount == 0 ? 0 : (itemCount + MenuLayout::kListingColumns - 1) / MenuLayout::kListingColumns;
    const QMargins margins = m_dishGridLayout->contentsMargins();
    const int gridH = rows * MenuLayout::kListingCardHeight + qMax(0, rows - 1) * MenuLayout::kListingSpacing
                      + margins.top() + margins.bottom();
    ui->widgetDishesHost->setMinimumHeight(gridH);
    ui->widgetDishesHost->adjustSize();
}

void DlgMenu::clearLayout(QLayout *layout)
{
    if (!layout) {
        return;
    }

    while (QLayoutItem *item = layout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }
}

void DlgMenu::onAddToCart(int dishId)
{
    const MenuDish dish = m_menuClient.dishById(dishId);
    if (dish.id <= 0) {
        return;
    }
    m_cart.addDish(dish);
    updateCartSummary();
}

void DlgMenu::onDishInfo(int dishId)
{
    const MenuDish dish = m_menuClient.dishById(dishId);
    if (dish.id <= 0) {
        return;
    }

    DlgDishDetails dlg(dish, this);
    dlg.exec();
}

void DlgMenu::onSearchTextChanged(const QString &text)
{
    Q_UNUSED(text);
    rebuildDishGrid();
}

void DlgMenu::updateCartSummary()
{
    ui->lblCartSummary->setText(
        tr("Cart: %1   %2 ֏").arg(m_cart.itemCount()).arg(QString::number(m_cart.totalAmount(), 'f', 0)));
}

void DlgMenu::onCancelOrder()
{
    m_cart.clear();
    reject();
}

void DlgMenu::onGoToCart()
{
    // TODO: open cart screen
}
