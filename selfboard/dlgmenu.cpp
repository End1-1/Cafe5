#include "dlgmenu.h"

#include "cartflyanimation.h"
#include "dishcardanimhost.h"
#include "selfboardlanguage.h"
#include "selfboardbottomchrome.h"
#include "dlgcart.h"
#include "dlgdishdetails.h"
#include "dlgpackagepick.h"
#include "dlgorderdone.h"
#include "dlgpayment.h"
#include "selfboardordersubmit.h"
#include "dishcardwidget.h"
#include "dishimageutils.h"
#include "menuhelpers.h"
#include "ui_dlgmenu.h"

#include <QAbstractButton>
#include <QAction>
#include <QButtonGroup>
#include <QEasingCurve>
#include <QEvent>
#include <QFile>
#include <QIcon>
#include <QKeyEvent>
#include <QMenu>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QSequentialAnimationGroup>
#include <QTimer>
#include <QColor>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayoutItem>
#include <QPalette>
#include <QPixmap>
#include <QPushButton>
#include <QScrollBar>
#include <QScroller>
#include "selfboardsettings.h"

#include <QSettings>
#include <QMessageBox>

namespace {

constexpr int kMenuGroupButtonWidth = 240;
constexpr int kMenuGroupPaddingH = 12;
constexpr int kMenuGroupPaddingV = 12;
constexpr int kMenuGroupIconSize = 40;
constexpr int kMenuGroupGap = 10;
constexpr int kMenuGroupMinHeight = 72;
constexpr int kMenuGroupMaxTextLines = 2;

int menuGroupTextBlockHeight(const QFontMetrics &fm)
{
    return fm.lineSpacing() * kMenuGroupMaxTextLines + fm.descent();
}

constexpr int kPanelSlideDurationMs = 300;
constexpr int kCardAnimDurationMs = 420;
constexpr int kCardStaggerDelayMs = 45;
constexpr int kCardSlideOffsetPx = 28;

QString elideToTwoLines(const QString &text, const QFont &font, int width)
{
    QFontMetrics fm(font);
    const int maxHeight = menuGroupTextBlockHeight(fm);
    const QRect maxRect(0, 0, width, maxHeight);

    if (fm.boundingRect(maxRect, Qt::TextWordWrap | Qt::AlignLeft, text).height() <= maxHeight) {
        return text;
    }

    QString trial = text;
    while (trial.length() > 1) {
        trial.chop(1);
        const QString candidate = trial + QString(QChar(0x2026));
        if (fm.boundingRect(maxRect, Qt::TextWordWrap | Qt::AlignLeft, candidate).height() <= maxHeight) {
            return candidate;
        }
    }

    return fm.elidedText(text, Qt::ElideRight, width);
}

void styleTransparentGroupLabel(QLabel *label)
{
    styleTransparentTextLabel(label);
}

void styleWhiteSidebarButton(QPushButton *btn)
{
    if (!btn) {
        return;
    }
    btn->setAttribute(Qt::WA_StyledBackground, true);
    btn->setAutoFillBackground(true);
    btn->setFlat(true);

    QPalette palette = btn->palette();
    palette.setColor(QPalette::Button, QColor(0xff, 0xff, 0xff));
    palette.setColor(QPalette::Window, QColor(0xff, 0xff, 0xff));
    btn->setPalette(palette);
}

void styleWhiteSidebarGroupButton(QPushButton *btn)
{
    if (!btn) {
        return;
    }

    const QColor white(0xff, 0xff, 0xff);
    btn->setFlat(true);
    btn->setAttribute(Qt::WA_StyledBackground, true);
    btn->setAutoFillBackground(true);

    QPalette palette = btn->palette();
    for (const QPalette::ColorRole role :
         {QPalette::Button, QPalette::Window, QPalette::Base, QPalette::Light,
          QPalette::Midlight, QPalette::Mid, QPalette::Dark, QPalette::Shadow}) {
        palette.setColor(role, white);
    }
    btn->setPalette(palette);
    btn->setStyleSheet(QStringLiteral(
        "QPushButton {"
        "  background-color: #ffffff;"
        "  border: none;"
        "  border-radius: 0;"
        "  color: #2a2a2a;"
        "  text-align: left;"
        "  padding: 0;"
        "}"
        "QPushButton:hover,"
        "QPushButton:pressed,"
        "QPushButton:checked {"
        "  background-color: #ffffff;"
        "  border: none;"
        "}"
        "QPushButton QLabel {"
        "  background: transparent;"
        "  border: none;"
        "}"));
}

QPushButton *makeSidebarGroupButton(const MenuGroup &group, QWidget *parent)
{
    auto *btn = new QPushButton(parent);
    btn->setObjectName(QStringLiteral("btnMenuGroup"));
    btn->setCheckable(true);
    btn->setText(QString());
    btn->setIcon(QIcon());
    btn->setProperty("groupId", group.id);
    btn->setFixedWidth(kMenuGroupButtonWidth);
    btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);

    auto *layout = new QHBoxLayout(btn);
    layout->setContentsMargins(kMenuGroupPaddingH, kMenuGroupPaddingV, kMenuGroupPaddingH, kMenuGroupPaddingV);
    layout->setSpacing(kMenuGroupGap);

    auto *iconLabel = new QLabel(btn);
    iconLabel->setObjectName(QStringLiteral("lblMenuGroupIcon"));
    iconLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    QPixmap icon(group.iconPath);
    if (icon.isNull()) {
        icon = QPixmap(QStringLiteral(":/res/dish_placeholder.png"));
    }
    if (!icon.isNull()) {
        setTransparentPixmapOnLabel(iconLabel, icon, kMenuGroupIconSize, kMenuGroupIconSize);
    } else {
        iconLabel->setFixedSize(kMenuGroupIconSize, kMenuGroupIconSize);
    }

    auto *textLabel = new QLabel(btn);
    textLabel->setObjectName(QStringLiteral("lblMenuGroupName"));
    textLabel->setWordWrap(true);
    textLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    textLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    styleTransparentGroupLabel(textLabel);

    const int textWidth = kMenuGroupButtonWidth - kMenuGroupPaddingH * 2 - kMenuGroupIconSize - kMenuGroupGap;
    const QFont textFont = textLabel->font();
    const QFontMetrics fm(textFont);
    textLabel->setFixedWidth(textWidth);
    textLabel->setText(elideToTwoLines(group.name, textFont, textWidth));

    layout->addWidget(iconLabel, 0, Qt::AlignVCenter);
    layout->addWidget(textLabel, 1, Qt::AlignVCenter);

    const auto updateGroupButtonHeight = [btn, iconLabel, textLabel]() {
        const QFontMetrics fm(textLabel->font());
        const int blockHeight = menuGroupTextBlockHeight(fm);
        textLabel->setFixedHeight(blockHeight);
        const int contentHeight = qMax(kMenuGroupIconSize, blockHeight);
        btn->setFixedHeight(qMax(kMenuGroupMinHeight, contentHeight + kMenuGroupPaddingV * 2));
    };

    updateGroupButtonHeight();

    QObject::connect(btn, &QPushButton::toggled, btn, [textLabel, updateGroupButtonHeight](bool checked) {
        QFont font = textLabel->font();
        font.setBold(checked);
        textLabel->setFont(font);
        updateGroupButtonHeight();
    });

    styleWhiteSidebarGroupButton(btn);
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
    m_dishGridLayout = ui->dishGridLayout;

    m_dishGridLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_dishGridLayout->setColumnStretch(MenuLayout::kListingColumns, 1);
    ui->scrollDishes->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    ui->widgetDishesHost->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    ui->widgetSidebar->setAttribute(Qt::WA_StyledBackground, true);
    ui->widgetContent->setAttribute(Qt::WA_StyledBackground, true);
    ui->widgetDishesHost->setAttribute(Qt::WA_StyledBackground, true);

    setupAppearance();
    setupServiceModeButton();
    setupTouchScroll();
    setupDishPanelHost();

    ui->lblSidebarLogo->setPixmap(QPixmap(QStringLiteral(":/res/main_logo.png")));

    m_bottomChrome = new SelfboardBottomChrome(this);
    auto *chromeHostLayout = new QVBoxLayout(ui->widgetBottomChromeHost);
    chromeHostLayout->setContentsMargins(0, 0, 0, 0);
    chromeHostLayout->addWidget(m_bottomChrome);

    updateServiceModeLabel();
    ui->btnServiceMode->setCursor(Qt::PointingHandCursor);

    m_groupButtons = new QButtonGroup(this);
    m_groupButtons->setExclusive(true);
    connect(m_groupButtons, &QButtonGroup::idClicked, this, &DlgMenu::onGroupClicked);

    buildSidebarGroups();

    const QVector<MenuGroup> groups = m_menuClient.groups();
    if (!groups.isEmpty()) {
        selectGroup(groups.first().id, false);
    }

    updateCartSummary();

    connect(ui->btnServiceMode, &QPushButton::clicked, this, &DlgMenu::onServiceModeClicked);
    connect(m_bottomChrome, &SelfboardBottomChrome::cancelOrderClicked, this, &DlgMenu::onCancelOrder);
    connect(m_bottomChrome, &SelfboardBottomChrome::goToCartClicked, this, &DlgMenu::onGoToCart);
    connect(m_bottomChrome, &SelfboardBottomChrome::cartSummaryClicked, this, &DlgMenu::onGoToCart);
}

DlgMenu::~DlgMenu()
{
    stopContentAnimations();
    closeOrderDone();
    closePaymentOverlay();
    closeCartOverlay();
    closePackagePicker();
    closeDishDetails();
    delete ui;
}

void DlgMenu::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        updateServiceModeLabel();
        updateCartSummary();
    }
    QDialog::changeEvent(event);
}

void DlgMenu::setupAppearance()
{
    QFile styleFile(QStringLiteral(":/res/selfboard.css"));
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setStyleSheet(QString::fromUtf8(styleFile.readAll()));
    }

    SelfboardLanguage::instance().bindPickerButton(ui->btnLanguage);

    ui->lblSidebarLogo->setFixedSize(240, 225);
    ui->lblBanner->setFixedHeight(225);
    ui->lblBanner->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->bodyLayout->setAlignment(Qt::AlignTop);
    ui->sidebarLayout->setAlignment(Qt::AlignTop);
    ui->contentLayout->setAlignment(Qt::AlignTop);
}

void DlgMenu::setServiceMode(ServiceMode mode)
{
    if (m_serviceMode == mode) {
        return;
    }

    m_serviceMode = mode;
    updateServiceModeLabel();
}

void DlgMenu::setupServiceModeButton()
{
    QPushButton *btn = ui->btnServiceMode;
    if (btn->findChild<QLabel *>(QStringLiteral("lblServiceModeReload"))) {
        return;
    }

    btn->setAttribute(Qt::WA_StyledBackground, true);
    btn->setAutoFillBackground(true);
    styleWhiteSidebarButton(btn);

    btn->setText(QString());
    btn->setIcon(QIcon());

    auto *layout = new QHBoxLayout(btn);
    layout->setContentsMargins(4, 8, 6, 8);
    layout->setSpacing(8);

    auto *modeIcon = new QLabel(btn);
    modeIcon->setObjectName(QStringLiteral("lblServiceModeIcon"));
    modeIcon->setFixedSize(28, 28);
    modeIcon->setScaledContents(true);
    modeIcon->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    auto *modeText = new QLabel(btn);
    modeText->setObjectName(QStringLiteral("lblServiceModeText"));
    modeText->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    auto *reloadIcon = new QLabel(btn);
    reloadIcon->setObjectName(QStringLiteral("lblServiceModeReload"));
    reloadIcon->setFixedSize(22, 22);
    reloadIcon->setScaledContents(true);
    reloadIcon->setPixmap(QPixmap(QStringLiteral(":/res/reload.png")));
    reloadIcon->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    layout->addWidget(modeIcon, 0, Qt::AlignVCenter);
    layout->addWidget(modeText, 1, Qt::AlignVCenter);
    layout->addWidget(reloadIcon, 0, Qt::AlignVCenter);
}

void DlgMenu::setupTouchScroll()
{
    ui->scrollDishes->setStyleSheet(QStringLiteral("background: #ececec; border: none;"));
    ui->scrollDishes->viewport()->setStyleSheet(QStringLiteral("background: #ececec;"));
    ui->scrollGroups->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrollGroups->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrollGroups->setStyleSheet(QStringLiteral(
        "QScrollArea#scrollGroups { background: #ececec; border: none; }"
        "QScrollArea#scrollGroups QScrollBar:vertical { width: 0px; background: transparent; }"
        "QScrollArea#scrollGroups QScrollBar:horizontal { height: 0px; background: transparent; }"));
    ui->scrollGroups->viewport()->setStyleSheet(QStringLiteral("background: #ececec;"));
    ui->widgetGroupsHost->setMinimumWidth(kMenuGroupButtonWidth);
    ui->widgetGroupsHost->setMaximumWidth(kMenuGroupButtonWidth);
    QScroller::grabGesture(ui->scrollDishes->viewport(), QScroller::LeftMouseButtonGesture);
    QScroller::grabGesture(ui->scrollGroups->viewport(), QScroller::LeftMouseButtonGesture);
}

void DlgMenu::setupDishPanelHost()
{
    m_dishPanelHost = new QWidget(ui->widgetContent);
    m_dishPanelHost->setObjectName(QStringLiteral("dishPanelHost"));
    m_dishPanelHost->setAttribute(Qt::WA_StyledBackground, true);
    m_dishPanelHost->setStyleSheet(QStringLiteral("background: #ececec;"));
    m_dishPanelHost->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_dishSlidePanel = new QWidget(m_dishPanelHost);
    m_dishSlidePanel->setObjectName(QStringLiteral("dishSlidePanel"));
    m_dishSlidePanel->setAttribute(Qt::WA_StyledBackground, true);
    m_dishSlidePanel->setStyleSheet(QStringLiteral("background: #ececec;"));

    const int scrollIndex = ui->contentLayout->indexOf(ui->scrollDishes);
    ui->contentLayout->removeWidget(ui->scrollDishes);
    ui->scrollDishes->setParent(m_dishSlidePanel);
    ui->contentLayout->insertWidget(scrollIndex, m_dishPanelHost);

    m_dishPanelHost->installEventFilter(this);
    syncDishPanelGeometry();
}

void DlgMenu::syncDishPanelGeometry()
{
    if (!m_dishPanelHost || !m_dishSlidePanel) {
        return;
    }

    const int w = m_dishPanelHost->width();
    const int h = m_dishPanelHost->height();
    if (w <= 0 || h <= 0) {
        return;
    }

    ui->scrollDishes->setGeometry(0, 0, w, h);

    if (m_panelSlideAnim) {
        m_dishSlidePanel->resize(w, h);
        return;
    }

    m_dishSlidePanel->setGeometry(0, 0, w, h);
}

bool DlgMenu::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_dishPanelHost && event->type() == QEvent::Resize) {
        syncDishPanelGeometry();
        return false;
    }

    return QDialog::eventFilter(watched, event);
}

void DlgMenu::updateServiceModeLabel()
{
    const bool takeAway = m_serviceMode == ServiceMode::TakeAway;
    const QString text = takeAway ? tr("Take away") : tr("Dine in");
    const QString iconPath = takeAway ? QStringLiteral(":/res/icon_takeaway.png")
                                      : QStringLiteral(":/res/icon_dinein.png");

    if (auto *modeText = ui->btnServiceMode->findChild<QLabel *>(QStringLiteral("lblServiceModeText"))) {
        ui->btnServiceMode->setText(QString());
        ui->btnServiceMode->setIcon(QIcon());
        if (auto *modeIcon = ui->btnServiceMode->findChild<QLabel *>(QStringLiteral("lblServiceModeIcon"))) {
            modeIcon->setPixmap(QPixmap(iconPath));
        }
        modeText->setText(text);
        return;
    }

    ui->btnServiceMode->setText(text);
    ui->btnServiceMode->setIcon(QIcon(iconPath));
    ui->btnServiceMode->setIconSize(QSize(28, 28));
}

void DlgMenu::onServiceModeClicked()
{
    QMenu menu(this);
    menu.setObjectName(QStringLiteral("menuServiceMode"));

    const auto addMode = [&](ServiceMode mode, const QString &label, const QString &iconPath) {
        QAction *action = menu.addAction(QIcon(iconPath), label);
        if (mode == m_serviceMode) {
            action->setCheckable(true);
            action->setChecked(true);
        }
        connect(action, &QAction::triggered, this, [this, mode]() {
            if (m_serviceMode == mode) {
                return;
            }
            setServiceMode(mode);
        });
    };

    addMode(ServiceMode::TakeAway,
            tr("Take away"),
            QStringLiteral(":/res/icon_takeaway.png"));
    addMode(ServiceMode::DineIn,
            tr("Dine in"),
            QStringLiteral(":/res/icon_dinein.png"));

    const QPoint pos = ui->btnServiceMode->mapToGlobal(QPoint(0, ui->btnServiceMode->height() + 4));
    menu.setFixedWidth(qMax(ui->btnServiceMode->width(), 188));
    menu.exec(pos);
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

void DlgMenu::selectGroup(int groupId, bool animatePanel)
{
    const bool groupChanged = groupId != m_currentGroupId;
    m_currentGroupId = groupId;

    if (QAbstractButton *btn = m_groupButtons->button(groupId)) {
        btn->setChecked(true);
    }

    stopContentAnimations();
    rebuildDishGrid(groupChanged && animatePanel);

    if (!groupChanged) {
        finalizeCardAppearance();
        return;
    }

    startGroupReveal(animatePanel);
}

void DlgMenu::startGroupReveal(bool animatePanel)
{
    QTimer::singleShot(0, this, [this, animatePanel]() {
        syncDishPanelGeometry();

        if (animatePanel && m_dishPanelHost && m_dishPanelHost->width() > 50) {
            animatePanelSlideIn();
        } else {
            if (m_dishSlidePanel) {
                m_dishSlidePanel->move(0, 0);
            }
            animateDishCardsStagger();
        }
    });
}

void DlgMenu::onGroupClicked(int groupId)
{
    selectGroup(groupId, true);
}

QVector<MenuDish> DlgMenu::filteredDishes() const
{
    return m_menuClient.dishesByGroup(m_currentGroupId);
}

void DlgMenu::rebuildDishGrid(bool prepareForAnimation)
{
    clearLayout(m_dishGridLayout);
    m_lastBuiltCardHosts.clear();

    const QVector<MenuDish> dishes = filteredDishes();
    int row = 0;
    int col = 0;

    for (const MenuDish &dish : dishes) {
        auto *card = new DishCardWidget(DishCardWidget::Style::Listing, dish, ui->widgetDishesHost);
        connect(card, &DishCardWidget::addToCartClicked, this, [this, card](int dishId) {
            onAddToCart(dishId, card);
        });
        connect(card, &DishCardWidget::infoClicked, this, &DlgMenu::onDishInfo);

        auto *host = new DishCardAnimHost(card, ui->widgetDishesHost);
        if (prepareForAnimation) {
            host->setSlideY(kCardSlideOffsetPx);
        } else {
            host->setSlideY(0.0);
        }

        m_dishGridLayout->addWidget(host, row, col);
        m_lastBuiltCardHosts.append(host);

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
    ui->scrollDishes->verticalScrollBar()->setValue(0);
}

void DlgMenu::stopContentAnimations()
{
    if (m_panelSlideAnim) {
        m_panelSlideAnim->stop();
        delete m_panelSlideAnim;
        m_panelSlideAnim = nullptr;
    }

    if (m_cardStaggerGroup) {
        m_cardStaggerGroup->stop();
        delete m_cardStaggerGroup;
        m_cardStaggerGroup = nullptr;
    }

    if (m_dishSlidePanel) {
        m_dishSlidePanel->move(0, 0);
    }

    finalizeCardAppearance();
}

void DlgMenu::finalizeCardAppearance()
{
    for (DishCardAnimHost *host : m_lastBuiltCardHosts) {
        if (!host) {
            continue;
        }
        host->setSlideY(0.0);
    }
}

void DlgMenu::animatePanelSlideIn()
{
    if (!m_dishPanelHost || !m_dishSlidePanel) {
        animateDishCardsStagger();
        return;
    }

    syncDishPanelGeometry();

    const int panelWidth = m_dishPanelHost->width();
    const int panelHeight = m_dishPanelHost->height();
    if (panelWidth <= 0 || panelHeight <= 0) {
        animateDishCardsStagger();
        return;
    }

    m_dishSlidePanel->resize(panelWidth, panelHeight);
    m_dishSlidePanel->move(-panelWidth, 0);

    auto *anim = new QPropertyAnimation(m_dishSlidePanel, "pos", this);
    anim->setDuration(kPanelSlideDurationMs);
    anim->setStartValue(QPoint(-panelWidth, 0));
    anim->setEndValue(QPoint(0, 0));
    anim->setEasingCurve(QEasingCurve::OutCubic);

    connect(anim, &QPropertyAnimation::finished, this, [this]() {
        m_panelSlideAnim = nullptr;
        if (m_dishSlidePanel) {
            m_dishSlidePanel->move(0, 0);
        }
        syncDishPanelGeometry();
        animateDishCardsStagger();
    });

    m_panelSlideAnim = anim;
    anim->start();
}

void DlgMenu::animateDishCardsStagger()
{
    if (m_lastBuiltCardHosts.isEmpty()) {
        return;
    }

    if (m_cardStaggerGroup) {
        m_cardStaggerGroup->stop();
        delete m_cardStaggerGroup;
        m_cardStaggerGroup = nullptr;
    }

    auto *parallel = new QParallelAnimationGroup(this);

    for (int i = 0; i < m_lastBuiltCardHosts.size(); ++i) {
        DishCardAnimHost *host = m_lastBuiltCardHosts.at(i);
        if (!host) {
            continue;
        }

        host->setSlideY(kCardSlideOffsetPx);

        auto *sequence = new QSequentialAnimationGroup(parallel);
        if (i > 0) {
            sequence->addPause(i * kCardStaggerDelayMs);
        }

        auto *slideAnim = new QPropertyAnimation(host, "slideY", sequence);
        slideAnim->setDuration(kCardAnimDurationMs);
        slideAnim->setStartValue(static_cast<qreal>(kCardSlideOffsetPx));
        slideAnim->setEndValue(0.0);
        slideAnim->setEasingCurve(QEasingCurve::OutBack);
        sequence->addAnimation(slideAnim);
        parallel->addAnimation(sequence);
    }

    connect(parallel, &QParallelAnimationGroup::finished, this, [this]() {
        m_cardStaggerGroup = nullptr;
        finalizeCardAppearance();
    });

    m_cardStaggerGroup = parallel;
    parallel->start();
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

void DlgMenu::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F4 && (event->modifiers() & Qt::AltModifier)) {
        if (m_packagePick && m_packagePick->isVisible()) {
            closePackagePicker();
        } else {
            reject();
        }
        event->accept();
        return;
    }
    QDialog::keyPressEvent(event);
}


void DlgMenu::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    syncDishPanelGeometry();
    if (m_packagePick && m_packagePick->isVisible()) {
        m_packagePick->setGeometry(rect());
    }
    if (m_dishDetails && m_dishDetails->isVisible()) {
        m_dishDetails->setGeometry(rect());
    }
    if (m_cartOverlay && m_cartOverlay->isVisible()) {
        m_cartOverlay->setGeometry(QRect(QPoint(0, 0), size()));
    }
    if (m_paymentOverlay && m_paymentOverlay->isVisible()) {
        m_paymentOverlay->setGeometry(QRect(QPoint(0, 0), size()));
    }
    if (m_orderDoneOverlay && m_orderDoneOverlay->isVisible()) {
        m_orderDoneOverlay->setGeometry(QRect(QPoint(0, 0), size()));
    }
}

void DlgMenu::closePackagePicker()
{
    if (!m_packagePick) {
        return;
    }
    m_packagePick->hide();
    m_packagePick->deleteLater();
    m_packagePick = nullptr;
}

void DlgMenu::showPackagePicker(const MenuDish &package)
{
    closePackagePicker();
    closeDishDetails();

    m_packagePick = new DlgPackagePick(package, this);
    m_packagePick->setGeometry(rect());
    connect(m_packagePick, &DlgPackagePick::accepted, this, [this, package](const MenuDish &line, int qty, const QVector<QPair<MenuDish, int>> &extras) {
        const bool editable = MenuHelpers::dishHasThreeStepPicker(package);
        m_cart.addDish(line, qty, editable);
        for (const QPair<MenuDish, int> &extra : extras) {
            if (extra.second > 0) {
                m_cart.addDish(extra.first, extra.second);
            }
        }
        updateCartSummary();
        closePackagePicker();
        showCartOverlay();
    });
    connect(m_packagePick, &DlgPackagePick::rejected, this, &DlgMenu::closePackagePicker);
    m_packagePick->raise();
    m_packagePick->show();
    m_packagePick->setFocus(Qt::OtherFocusReason);
}

void DlgMenu::onAddToCart(int dishId, DishCardWidget *card)
{
    const MenuDish dish = m_menuClient.dishById(dishId);
    if (dish.id <= 0) {
        return;
    }

    if (dish.isPackage()) {
        if (MenuHelpers::dishNeedsOptionsPicker(dish)) {
            showPackagePicker(dish);
        } else {
            const MenuPackageComponent *component = dish.packageComponents.isEmpty()
                                                        ? nullptr
                                                        : &dish.packageComponents.first();
            m_cart.addDish(MenuHelpers::resolvePackageCartLine(dish, component));
            updateCartSummary();
        }
        return;
    }

    if (MenuHelpers::dishNeedsOptionsPicker(dish)) {
        showPackagePicker(dish);
        return;
    }

    m_cart.addDish(dish);
    updateCartSummary();
    playFlyToCart(card);
}

void DlgMenu::playFlyToCart(DishCardWidget *card)
{
    if (!card || !m_bottomChrome) {
        return;
    }

    const QPixmap pixmap = card->thumbnailPixmap();
    if (pixmap.isNull()) {
        m_bottomChrome->playCartAddedBump();
        return;
    }

    SelfboardBottomChrome *chrome = m_bottomChrome;
    CartFlyAnimation::run(pixmap,
                          card->flyStartGlobalPos(),
                          chrome->cartFlyTargetGlobalPos(),
                          [chrome]() {
                              chrome->playCartAddedBump();
                          });
}

void DlgMenu::closeDishDetails()
{
    if (!m_dishDetails) {
        return;
    }
    DlgDishDetails *overlay = m_dishDetails;
    m_dishDetails = nullptr;
    overlay->hide();
    overlay->deleteLater();
}

void DlgMenu::onDishInfo(int dishId)
{
    const MenuDish dish = m_menuClient.dishById(dishId);
    if (dish.id <= 0) {
        return;
    }

    closeDishDetails();
    m_dishDetails = new DlgDishDetails(dish, this);
    m_dishDetails->setGeometry(rect());
    connect(m_dishDetails, &DlgDishDetails::closed, this, &DlgMenu::closeDishDetails);
    m_dishDetails->raise();
    m_dishDetails->show();
    m_dishDetails->setFocus(Qt::OtherFocusReason);
}

void DlgMenu::updateCartSummary()
{
    if (!m_bottomChrome) {
        return;
    }
    m_bottomChrome->setCartCount(m_cart.itemCount());
    m_bottomChrome->setCartTotal(m_cart.totalAmount());
    m_bottomChrome->setGoToCartEnabled(!m_cart.isEmpty());
}

void DlgMenu::onCancelOrder()
{
    m_cart.clear();
    reject();
}

void DlgMenu::closeCartOverlay()
{
    if (!m_cartOverlay) {
        return;
    }
    DlgCart *overlay = m_cartOverlay;
    m_cartOverlay = nullptr;
    overlay->hide();
    overlay->deleteLater();
    update();
    repaint();
}

void DlgMenu::closePaymentOverlay()
{
    if (!m_paymentOverlay) {
        return;
    }
    DlgPayment *overlay = m_paymentOverlay;
    m_paymentOverlay = nullptr;
    overlay->hide();
    overlay->deleteLater();
    update();
    repaint();
}

void DlgMenu::showCartOverlay()
{
    closeCartOverlay();
    closePaymentOverlay();

    m_cartOverlay = new DlgCart(&m_cart, this);
    m_cartOverlay->setGeometry(QRect(QPoint(0, 0), size()));
    m_cartOverlay->setFixedSize(size());
    connect(m_cartOverlay, &DlgCart::finished, this, &DlgMenu::onCartOverlayFinished);
    m_cartOverlay->raise();
    m_cartOverlay->show();
    m_cartOverlay->activateWindow();
}

void DlgMenu::showPaymentOverlay()
{
    closePaymentOverlay();

    m_paymentOverlay = new DlgPayment(&m_cart, this);
    m_paymentOverlay->setGeometry(QRect(QPoint(0, 0), size()));
    connect(m_paymentOverlay, &DlgPayment::finished, this, &DlgMenu::onPaymentOverlayFinished);
    m_paymentOverlay->raise();
    m_paymentOverlay->show();
}

void DlgMenu::onCartOverlayFinished(int result)
{
    if (result == DlgCart::ResultGoToPay) {
        closeCartOverlay();
        showPaymentOverlay();
        return;
    }

    updateCartSummary();
    closeCartOverlay();
}

void DlgMenu::closeOrderDone()
{
    if (!m_orderDoneOverlay) {
        return;
    }
    DlgOrderDone *overlay = m_orderDoneOverlay;
    m_orderDoneOverlay = nullptr;
    overlay->hide();
    overlay->deleteLater();
    update();
    repaint();
}

void DlgMenu::showOrderDone(const QString &orderNumber)
{
    closeOrderDone();

    m_orderDoneOverlay = new DlgOrderDone(orderNumber, this);
    m_orderDoneOverlay->setGeometry(QRect(QPoint(0, 0), size()));
    connect(m_orderDoneOverlay, &DlgOrderDone::acknowledged, this, [this]() {
        closeOrderDone();
        accept();
    });
    m_orderDoneOverlay->raise();
    m_orderDoneOverlay->show();
}

void DlgMenu::submitPaidOrder()
{
    if (m_orderSubmitInProgress || m_cart.isEmpty()) {
        return;
    }

    m_orderSubmitInProgress = true;
    const SelfBoardServiceMode serviceMode = m_serviceMode == ServiceMode::TakeAway
                                                 ? SelfBoardServiceMode::TakeAway
                                                 : SelfBoardServiceMode::DineIn;
    SelfBoardOrderSubmit::submit(
        &m_cart,
        serviceMode,
        this,
        [this](bool ok, const QString &orderNumber, const QString &error) {
            m_orderSubmitInProgress = false;
            if (!ok) {
                QMessageBox::critical(
                    nullptr,
                    tr("Order"),
                    error.isEmpty() ? tr("Failed to create order") : error);
                return;
            }

            m_cart.clear();
            updateCartSummary();
            showOrderDone(orderNumber);
        });
}

void DlgMenu::onPaymentOverlayFinished(int result)
{
    const bool backToCart = m_paymentOverlay && m_paymentOverlay->backToCartRequested();

    if (result == 1) {
        closePaymentOverlay();
        submitPaidOrder();
        return;
    }

    closePaymentOverlay();
    if (backToCart) {
        showCartOverlay();
    }
}

void DlgMenu::onGoToCart()
{
    if (m_cart.isEmpty()) {
        return;
    }

    showCartOverlay();
}
