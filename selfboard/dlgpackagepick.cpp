#include "dlgpackagepick.h"

#include "dishimageutils.h"
#include "menucache.h"
#include "menuhelpers.h"

#include <QAbstractButton>
#include <QButtonGroup>
#include <QCheckBox>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QSizePolicy>
#include <QStackedWidget>
#include <QStyle>
#include <QVBoxLayout>

namespace {

constexpr int kRelatedCardHeight = 260;
constexpr int kRelatedGridColumns = 3;
constexpr int kRelatedGridVisibleRows = 3;
constexpr int kRelatedGridSpacing = 16;
constexpr int kPkgPickImageSize = 220;
constexpr int kRelatedScrollHeight = kRelatedGridVisibleRows * kRelatedCardHeight
    + (kRelatedGridVisibleRows - 1) * kRelatedGridSpacing;
constexpr int kPkgPickTopInset = 262;
constexpr int kPkgPickSideMargin = 24;
constexpr int kPkgPickBottomMargin = 24;

void clearLayout(QLayout *layout)
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

void addBadgeIcon(QWidget *host, QHBoxLayout *layout, bool enabled, const char *iconPath)
{
    if (!enabled) {
        return;
    }
    auto *icon = new QLabel(host);
    icon->setObjectName(QStringLiteral("pkgPickBadgeIcon"));
    icon->setFixedSize(44, 44);
    icon->setAlignment(Qt::AlignCenter);
    icon->setScaledContents(true);
    icon->setPixmap(QPixmap(QString::fromUtf8(iconPath)));
    layout->addWidget(icon, 0, Qt::AlignVCenter);
}

QFrame *addBjuBox(QWidget *host, QHBoxLayout *layout, const QString &valueText, const QString &title)
{
    auto *box = new QFrame(host);
    box->setObjectName(QStringLiteral("pkgPickBjuBox"));
    auto *boxLayout = new QVBoxLayout(box);
    boxLayout->setContentsMargins(10, 8, 10, 8);
    boxLayout->setSpacing(6);

    auto *valueLbl = new QLabel(valueText, box);
    valueLbl->setObjectName(QStringLiteral("pkgPickBjuValue"));
    valueLbl->setAlignment(Qt::AlignCenter);

    auto *divider = new QFrame(box);
    divider->setObjectName(QStringLiteral("pkgPickBjuDivider"));
    divider->setFixedHeight(1);

    auto *titleLbl = new QLabel(title, box);
    titleLbl->setObjectName(QStringLiteral("pkgPickBjuTitle"));
    titleLbl->setAlignment(Qt::AlignCenter);

    boxLayout->addWidget(valueLbl);
    boxLayout->addWidget(divider);
    boxLayout->addWidget(titleLbl);
    layout->addWidget(box, 0, Qt::AlignVCenter);
    return box;
}

class AttrOptionRow : public QWidget
{
public:
    explicit AttrOptionRow(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        setObjectName(QStringLiteral("pkgPickAttrRow"));
        setCursor(Qt::PointingHandCursor);
    }

    void setRadio(QRadioButton *radio) { m_radio = radio; }

protected:
    void mousePressEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton && m_radio) {
            m_radio->setChecked(true);
        }
        QWidget::mousePressEvent(event);
    }

private:
    QRadioButton *m_radio = nullptr;
};

class ModOptionRow : public QWidget
{
public:
    explicit ModOptionRow(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        setObjectName(QStringLiteral("pkgPickModRow"));
        setCursor(Qt::PointingHandCursor);
    }

    void setCheckBox(QCheckBox *checkBox) { m_checkBox = checkBox; }

protected:
    void mousePressEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton && m_checkBox) {
            m_checkBox->setChecked(!m_checkBox->isChecked());
        }
        QWidget::mousePressEvent(event);
    }

private:
    QCheckBox *m_checkBox = nullptr;
};

} // namespace

DlgPackagePick::DlgPackagePick(const MenuDish &package, QWidget *parent)
    : QWidget(parent)
    , m_package(package)
{
    setObjectName(QStringLiteral("DlgPackagePick"));
    setAttribute(Qt::WA_StyledBackground, true);
    setFocusPolicy(Qt::StrongFocus);

    m_card = new QFrame(this);
    m_card->setObjectName(QStringLiteral("pkgPickCard"));
    m_card->setAttribute(Qt::WA_StyledBackground, true);

    buildUi();
    rebuildHeaderInfo();

    const bool hasModificators = !MenuHelpers::pickerModificators(m_package).isEmpty();
    const bool hasRelated = !m_package.relatedDrinks.isEmpty() || !m_package.relatedOther.isEmpty();
    if (m_package.isPackage()
        && m_package.packageComponents.isEmpty()
        && !hasModificators
        && !hasRelated) {
        if (m_lblEmpty) {
            m_lblEmpty->setText(tr("This package has no configured options."));
            m_lblEmpty->show();
        }
        if (m_btnNext) {
            m_btnNext->setEnabled(false);
        }
        return;
    }

    if (m_package.isPackage()
        && !m_package.packageComponents.isEmpty()
        && MenuHelpers::packageNeedsAttributePicker(m_package.packageComponents)) {
        buildAttributeGroups();
    }

    buildModificatorGroups();
    setStep(PickStep::Personalize);
    updatePreview();
}

void DlgPackagePick::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(rect(), QColor(0, 0, 0, 140));
    QWidget::paintEvent(event);
}

void DlgPackagePick::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (!m_card) {
        return;
    }
    m_card->setGeometry(kPkgPickSideMargin,
                        kPkgPickTopInset,
                        qMax(0, width() - kPkgPickSideMargin * 2),
                        qMax(0, height() - kPkgPickTopInset - kPkgPickBottomMargin));
}

void DlgPackagePick::mousePressEvent(QMouseEvent *event)
{
    if (m_card && !m_card->geometry().contains(event->pos())) {
        onCloseClicked();
        event->accept();
        return;
    }
    QWidget::mousePressEvent(event);
}

void DlgPackagePick::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape
        || (event->key() == Qt::Key_F4 && (event->modifiers() & Qt::AltModifier))) {
        onCloseClicked();
        event->accept();
        return;
    }
    QWidget::keyPressEvent(event);
}

QString DlgPackagePick::sectionTitle(const QString &key) const
{
    if (key == QStringLiteral("Type")) {
        return tr("Type");
    }
    if (key == QStringLiteral("Size")) {
        return tr("Size");
    }
    return key;
}

QString DlgPackagePick::formatOptionLabel(const QString &key, const QString &value) const
{
    const QString measurement = measurementForKey(key, value);
    const QString name = MenuHelpers::attributeDisplayLabel(value, measurement);
    const double price = MenuHelpers::fixedAttributeOptionPrice(
        m_package.packageComponents, key, value);
    if (price > 0.009) {
        return tr("%1  +%2").arg(name, QString::number(price, 'f', 0));
    }
    return name;
}

QWidget *DlgPackagePick::makeStepSidebarItem(int index, const QString &title, const QString &subtitle)
{
    auto *wrap = new QWidget();
    wrap->setObjectName(QStringLiteral("pkgPickStepWrap"));
    auto *wrapLayout = new QVBoxLayout(wrap);
    wrapLayout->setContentsMargins(4, 2, 4, 8);
    wrapLayout->setSpacing(0);

    auto *item = new QFrame(wrap);
    item->setObjectName(QStringLiteral("pkgPickStepItem"));
    item->setProperty("stepIndex", index);
    item->setProperty("personalizeStep", index == 0);
    item->setAttribute(Qt::WA_StyledBackground, true);
    item->setMinimumHeight(68);
    item->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

    auto *layout = new QHBoxLayout(item);
    layout->setContentsMargins(20, 18, 20, 18);
    layout->setSpacing(14);

    auto *indicator = new QLabel(item);
    indicator->setObjectName(QStringLiteral("pkgPickStepIndicator"));
    indicator->setFixedSize(32, 32);
    indicator->setAlignment(Qt::AlignCenter);
    indicator->setAttribute(Qt::WA_StyledBackground, true);
    indicator->setProperty("stepIndicator", true);

    auto *textCol = new QVBoxLayout();
    textCol->setSpacing(4);
    auto *titleLbl = new QLabel(title, item);
    titleLbl->setObjectName(QStringLiteral("pkgPickStepTitle"));
    titleLbl->setWordWrap(true);
    textCol->addWidget(titleLbl);
    if (!subtitle.isEmpty()) {
        auto *subLbl = new QLabel(subtitle, item);
        subLbl->setObjectName(QStringLiteral("pkgPickStepSubtitle"));
        subLbl->setWordWrap(true);
        textCol->addWidget(subLbl);
    }

    layout->addWidget(indicator, 0, Qt::AlignVCenter);
    layout->addLayout(textCol, 1);

    wrapLayout->addWidget(item);

    m_stepItems.append(wrap);
    return wrap;
}

QWidget *DlgPackagePick::buildRelatedPickPage(const QString &title, QVector<RelatedPickState> &picks)
{
    auto *page = new QWidget(m_card);
    page->setObjectName(QStringLiteral("pkgPickRelatedPage"));
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(20);

    auto *titleLbl = new QLabel(title, page);
    titleLbl->setObjectName(QStringLiteral("pkgPickContentTitle"));
    layout->addWidget(titleLbl);

    auto *scroll = new QScrollArea(page);
    scroll->setObjectName(QStringLiteral("pkgPickScroll"));
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setStyleSheet(QStringLiteral(
        "QScrollArea#pkgPickScroll { background: transparent; border: none; }"
        "QScrollArea#pkgPickScroll > QWidget > QWidget { background: transparent; }"));

    auto *gridHost = new QWidget();
    gridHost->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    auto *grid = new QGridLayout(gridHost);
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setSpacing(kRelatedGridSpacing);
    for (int col = 0; col < kRelatedGridColumns; ++col) {
        grid->setColumnStretch(col, 1);
    }

    for (int i = 0; i < picks.size(); ++i) {
        RelatedPickState &pick = picks[i];
        auto *card = new QFrame(gridHost);
        card->setObjectName(QStringLiteral("pkgPickRelatedCard"));
        card->setFixedHeight(kRelatedCardHeight);
        card->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        auto *cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(16, 16, 16, 16);
        cardLayout->setSpacing(8);

        auto *img = new QLabel(card);
        img->setObjectName(QStringLiteral("pkgPickRelatedImage"));
        img->setFixedHeight(120);
        img->setAlignment(Qt::AlignCenter);
        setDishImageOnLabel(img, pick.dish.imagePath, 120, 160);

        auto *name = new QLabel(pick.dish.name, card);
        name->setObjectName(QStringLiteral("pkgPickRelatedName"));
        name->setWordWrap(true);
        name->setAlignment(Qt::AlignHCenter);
        name->setMaximumHeight(40);

        pick.bottomStack = new QStackedWidget(card);
        pick.bottomStack->setObjectName(QStringLiteral("pkgPickRelatedBottom"));
        pick.bottomStack->setFixedHeight(52);

        auto *idleRow = new QWidget(pick.bottomStack);
        idleRow->setObjectName(QStringLiteral("pkgPickRelatedIdleRow"));
        auto *idleLayout = new QHBoxLayout(idleRow);
        idleLayout->setContentsMargins(0, 0, 0, 0);
        idleLayout->setSpacing(12);
        auto *idlePrice = new QLabel(
            tr("%1 ֏").arg(QString::number(pick.dish.price, 'f', 0)), idleRow);
        idlePrice->setObjectName(QStringLiteral("pkgPickRelatedIdlePrice"));
        idlePrice->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        auto *btnAdd = new QPushButton(QStringLiteral("+"), idleRow);
        btnAdd->setObjectName(QStringLiteral("pkgPickRelatedAddBtn"));
        btnAdd->setFixedSize(35, 35);
        btnAdd->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        idleLayout->addWidget(idlePrice, 1);
        idleLayout->addWidget(btnAdd, 0, Qt::AlignRight | Qt::AlignVCenter);

        auto *activeRow = new QWidget(pick.bottomStack);
        activeRow->setObjectName(QStringLiteral("pkgPickRelatedActiveRow"));
        auto *activeLayout = new QHBoxLayout(activeRow);
        activeLayout->setContentsMargins(0, 0, 0, 0);
        activeLayout->setSpacing(12);
        auto *btnMinus = new QPushButton(QStringLiteral("−"), activeRow);
        btnMinus->setObjectName(QStringLiteral("pkgPickRelatedMinus"));
        btnMinus->setFixedSize(35, 35);
        btnMinus->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        pick.qtySummaryLabel = new QLabel(activeRow);
        pick.qtySummaryLabel->setObjectName(QStringLiteral("pkgPickRelatedQtySummary"));
        pick.qtySummaryLabel->setAlignment(Qt::AlignCenter);
        auto *btnPlus = new QPushButton(QStringLiteral("+"), activeRow);
        btnPlus->setObjectName(QStringLiteral("pkgPickRelatedPlus"));
        btnPlus->setFixedSize(35, 35);
        btnPlus->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        activeLayout->addWidget(btnMinus, 0, Qt::AlignVCenter);
        activeLayout->addWidget(pick.qtySummaryLabel, 1);
        activeLayout->addWidget(btnPlus, 0, Qt::AlignVCenter);

        pick.bottomStack->addWidget(idleRow);
        pick.bottomStack->addWidget(activeRow);
        pick.bottomStack->setCurrentIndex(0);

        connect(btnAdd, &QPushButton::clicked, this, [this, &picks, i]() {
            changeRelatedQty(picks, i, 1);
        });
        connect(btnMinus, &QPushButton::clicked, this, [this, &picks, i]() {
            changeRelatedQty(picks, i, -1);
        });
        connect(btnPlus, &QPushButton::clicked, this, [this, &picks, i]() {
            changeRelatedQty(picks, i, 1);
        });

        cardLayout->addWidget(img);
        cardLayout->addWidget(name);
        cardLayout->addWidget(pick.bottomStack);

        const int row = i / kRelatedGridColumns;
        const int col = i % kRelatedGridColumns;
        grid->setRowStretch(row, 0);
        grid->addWidget(card, row, col);
    }

    const int rowCount = picks.isEmpty()
        ? 0
        : (picks.size() + kRelatedGridColumns - 1) / kRelatedGridColumns;
    if (rowCount > 0) {
        auto *bottomSpacer = new QWidget(gridHost);
        bottomSpacer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        grid->addWidget(bottomSpacer, rowCount, 0, 1, kRelatedGridColumns);
        grid->setRowStretch(rowCount, 1);
    }

    scroll->setWidget(gridHost);
    scroll->setFixedHeight(kRelatedScrollHeight);
    scroll->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    layout->addWidget(scroll, 0);

    auto *hintLbl = new QLabel(tr("Optional - skip if you do not want to add anything."), page);
    hintLbl->setObjectName(QStringLiteral("pkgPickStubHint"));
    hintLbl->setWordWrap(true);
    layout->addWidget(hintLbl);
    layout->addStretch();

    return page;
}

void DlgPackagePick::initRelatedPicks()
{
    const auto resolveRelated = [](const MenuRelatedItem &ref) {
        MenuDish dish = MenuCache::instance().dishById(ref.id);
        if (dish.id <= 0) {
            dish.id = ref.id;
            dish.name = ref.name;
            dish.price = 0.0;
        } else if (!ref.name.isEmpty()) {
            dish.name = ref.name;
        }
        return dish;
    };

    for (const MenuRelatedItem &ref : m_package.relatedDrinks) {
        RelatedPickState state;
        state.dish = resolveRelated(ref);
        m_drinkPicks.append(state);
    }
    for (const MenuRelatedItem &ref : m_package.relatedOther) {
        RelatedPickState state;
        state.dish = resolveRelated(ref);
        m_extraPicks.append(state);
    }

    m_hasDrinkStep = !m_drinkPicks.isEmpty();
    m_hasExtraStep = !m_extraPicks.isEmpty();

    if (m_stepItems.size() >= 3) {
        m_stepItems.at(1)->setVisible(m_hasDrinkStep);
        m_stepItems.at(2)->setVisible(m_hasExtraStep);
    }
}

void DlgPackagePick::updateRelatedPickUi(RelatedPickState &pick)
{
    if (pick.bottomStack) {
        pick.bottomStack->setCurrentIndex(pick.qty > 0 ? 1 : 0);
    }
    if (pick.qtySummaryLabel) {
        pick.qtySummaryLabel->setText(
            tr("%1x %2 ֏").arg(pick.qty).arg(QString::number(pick.dish.price, 'f', 0)));
    }
}

void DlgPackagePick::changeRelatedQty(QVector<RelatedPickState> &picks, int index, int delta)
{
    if (index < 0 || index >= picks.size()) {
        return;
    }
    picks[index].qty = qMax(0, picks[index].qty + delta);
    updateRelatedPickUi(picks[index]);
    updateFooter();
}

void DlgPackagePick::buildDrinkPage()
{
    m_pageDrink = buildRelatedPickPage(tr("Choose coke or juice"), m_drinkPicks);
    m_contentStack->addWidget(m_pageDrink);
}

void DlgPackagePick::buildExtraPage()
{
    m_pageExtra = buildRelatedPickPage(tr("Add extra"), m_extraPicks);
    m_contentStack->addWidget(m_pageExtra);
}

DlgPackagePick::PickStep DlgPackagePick::nextStepAfter(DlgPackagePick::PickStep step) const
{
    switch (step) {
    case PickStep::Personalize:
        if (m_hasDrinkStep) {
            return PickStep::Drink;
        }
        if (m_hasExtraStep) {
            return PickStep::Extra;
        }
        return PickStep::Extra;
    case PickStep::Drink:
        if (m_hasExtraStep) {
            return PickStep::Extra;
        }
        return PickStep::Extra;
    case PickStep::Extra:
        return PickStep::Extra;
    }
    return PickStep::Extra;
}

DlgPackagePick::PickStep DlgPackagePick::previousStepBefore(DlgPackagePick::PickStep step) const
{
    switch (step) {
    case PickStep::Extra:
        if (m_hasDrinkStep) {
            return PickStep::Drink;
        }
        return PickStep::Personalize;
    case PickStep::Drink:
        return PickStep::Personalize;
    case PickStep::Personalize:
        return PickStep::Personalize;
    }
    return PickStep::Personalize;
}

QVector<QPair<MenuDish, int>> DlgPackagePick::selectedExtraLines() const
{
    QVector<QPair<MenuDish, int>> lines;
    for (const RelatedPickState &pick : m_drinkPicks) {
        if (pick.qty > 0) {
            lines.append(qMakePair(pick.dish, pick.qty));
        }
    }
    for (const RelatedPickState &pick : m_extraPicks) {
        if (pick.qty > 0) {
            lines.append(qMakePair(pick.dish, pick.qty));
        }
    }
    return lines;
}

double DlgPackagePick::relatedSelectionTotal() const
{
    double sum = 0.0;
    for (const RelatedPickState &pick : m_drinkPicks) {
        sum += pick.dish.price * pick.qty;
    }
    for (const RelatedPickState &pick : m_extraPicks) {
        sum += pick.dish.price * pick.qty;
    }
    return sum;
}

void DlgPackagePick::buildUi()
{
    auto *cardLayout = new QVBoxLayout(m_card);
    cardLayout->setContentsMargins(24, 20, 24, 20);
    cardLayout->setSpacing(0);

    buildHeader();

    auto *headerDivider = new QFrame(m_card);
    headerDivider->setObjectName(QStringLiteral("pkgPickDivider"));
    headerDivider->setFixedHeight(1);
    cardLayout->addSpacing(16);
    cardLayout->addWidget(headerDivider);
    cardLayout->addSpacing(16);

    auto *body = new QHBoxLayout();
    body->setContentsMargins(0, 0, 0, 0);
    body->setSpacing(24);

    auto *sidebar = new QFrame(m_card);
    sidebar->setObjectName(QStringLiteral("pkgPickSidebar"));
    sidebar->setAttribute(Qt::WA_StyledBackground, true);
    sidebar->setFixedWidth(300);
    m_sidebarLayout = new QVBoxLayout(sidebar);
    m_sidebarLayout->setContentsMargins(14, 18, 14, 18);
    m_sidebarLayout->setSpacing(14);

    auto *sidebarTitle = new QLabel(tr("Your choice"), sidebar);
    sidebarTitle->setObjectName(QStringLiteral("pkgPickSidebarTitle"));
    sidebarTitle->setAlignment(Qt::AlignCenter);
    m_sidebarLayout->addWidget(sidebarTitle);

    m_sidebarLayout->addWidget(makeStepSidebarItem(0, tr("Personalize"), m_package.name));
    m_sidebarLayout->addWidget(makeStepSidebarItem(1, tr("Choose drink")));
    m_sidebarLayout->addWidget(makeStepSidebarItem(2, tr("Add extra")));
    m_sidebarLayout->addStretch();
    body->addWidget(sidebar);

    m_contentStack = new QStackedWidget(m_card);
    m_contentStack->setObjectName(QStringLiteral("pkgPickContentStack"));
    buildPersonalizePage();
    initRelatedPicks();
    buildDrinkPage();
    buildExtraPage();
    body->addWidget(m_contentStack, 1);

    cardLayout->addLayout(body, 1);

    auto *footerDivider = new QFrame(m_card);
    footerDivider->setObjectName(QStringLiteral("pkgPickDivider"));
    footerDivider->setFixedHeight(1);
    cardLayout->addSpacing(16);
    cardLayout->addWidget(footerDivider);
    cardLayout->addSpacing(12);

    buildFooter();
}

void DlgPackagePick::buildHeader()
{
    auto *header = new QHBoxLayout();
    header->setContentsMargins(0, 0, 0, 0);
    header->setSpacing(24);

    m_lblImage = new QLabel(m_card);
    m_lblImage->setObjectName(QStringLiteral("pkgPickImage"));
    m_lblImage->setFixedSize(kPkgPickImageSize, kPkgPickImageSize);
    m_lblImage->setAlignment(Qt::AlignCenter);
    setDishImageOnLabel(m_lblImage, m_package.imagePath, kPkgPickImageSize, kPkgPickImageSize);
    header->addWidget(m_lblImage, 0, Qt::AlignTop);

    auto *info = new QVBoxLayout();
    info->setSpacing(10);
    info->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    m_lblTitle = new QLabel(m_package.name, m_card);
    m_lblTitle->setObjectName(QStringLiteral("pkgPickTitle"));
    m_lblTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    info->addWidget(m_lblTitle, 0, Qt::AlignLeft);

    m_lblDescription = new QLabel(m_package.description, m_card);
    m_lblDescription->setObjectName(QStringLiteral("pkgPickDescription"));
    m_lblDescription->setWordWrap(true);
    m_lblDescription->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    info->addWidget(m_lblDescription, 0, Qt::AlignLeft);

    m_badgesRow = new QWidget(m_card);
    m_badgesRow->setObjectName(QStringLiteral("pkgPickBadgesRow"));
    m_badgesRow->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    m_badgesLayout = new QHBoxLayout(m_badgesRow);
    m_badgesLayout->setContentsMargins(0, 0, 0, 0);
    m_badgesLayout->setSpacing(8);
    m_badgesLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    info->addWidget(m_badgesRow, 0, Qt::AlignLeft);

    m_bjuRow = new QWidget(m_card);
    m_bjuRow->setObjectName(QStringLiteral("pkgPickBjuRow"));
    m_bjuRow->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    m_bjuLayout = new QHBoxLayout(m_bjuRow);
    m_bjuLayout->setContentsMargins(0, 0, 0, 0);
    m_bjuLayout->setSpacing(8);
    m_bjuLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    info->addWidget(m_bjuRow, 0, Qt::AlignLeft);

    info->addStretch();
    header->addLayout(info, 1);

    m_btnClose = new QPushButton(QStringLiteral("×"), m_card);
    m_btnClose->setObjectName(QStringLiteral("pkgPickCloseBtn"));
    connect(m_btnClose, &QPushButton::clicked, this, &DlgPackagePick::onCloseClicked);
    header->addWidget(m_btnClose, 0, Qt::AlignTop);

    static_cast<QVBoxLayout *>(m_card->layout())->insertLayout(0, header);
}

void DlgPackagePick::buildFooter()
{
    auto *footer = new QHBoxLayout();
    footer->setContentsMargins(0, 0, 0, 0);
    footer->setSpacing(16);

    m_btnGoToMenu = new QPushButton(tr("Go to menu"), m_card);
    m_btnGoToMenu->setObjectName(QStringLiteral("pkgPickGoToMenu"));
    connect(m_btnGoToMenu, &QPushButton::clicked, this, &DlgPackagePick::onGoToMenuClicked);
    footer->addWidget(m_btnGoToMenu, 0, Qt::AlignLeft);

    footer->addStretch();

    m_lblTotal = new QLabel(m_card);
    m_lblTotal->setObjectName(QStringLiteral("pkgPickTotal"));
    footer->addWidget(m_lblTotal, 0, Qt::AlignRight);

    m_btnBack = new QPushButton(m_card);
    m_btnBack->setObjectName(QStringLiteral("pkgPickBackBtn"));
    connect(m_btnBack, &QPushButton::clicked, this, &DlgPackagePick::onBackClicked);
    footer->addWidget(m_btnBack);

    m_btnNext = new QPushButton(m_card);
    m_btnNext->setObjectName(QStringLiteral("pkgPickNextBtn"));
    connect(m_btnNext, &QPushButton::clicked, this, &DlgPackagePick::onNextClicked);
    footer->addWidget(m_btnNext);

    static_cast<QVBoxLayout *>(m_card->layout())->addLayout(footer);
}

void DlgPackagePick::buildPersonalizePage()
{
    m_pagePersonalize = new QWidget(m_card);
    m_pagePersonalize->setObjectName(QStringLiteral("pkgPickPersonalizePage"));
    auto *pageLayout = new QVBoxLayout(m_pagePersonalize);
    pageLayout->setContentsMargins(0, 0, 0, 0);
    pageLayout->setSpacing(16);

    m_lblPersonalizeTitle = new QLabel(
        tr("Customize your %1 as you want").arg(m_package.name), m_pagePersonalize);
    m_lblPersonalizeTitle->setObjectName(QStringLiteral("pkgPickContentTitle"));
    pageLayout->addWidget(m_lblPersonalizeTitle);

    auto *scroll = new QScrollArea(m_pagePersonalize);
    scroll->setObjectName(QStringLiteral("pkgPickScroll"));
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setStyleSheet(QStringLiteral(
        "QScrollArea#pkgPickScroll { background: transparent; border: none; }"
        "QScrollArea#pkgPickScroll > QWidget > QWidget { background: transparent; }"));

    auto *scrollHost = new QWidget();
    scrollHost->setObjectName(QStringLiteral("pkgPickScrollHost"));
    m_attributesLayout = new QVBoxLayout(scrollHost);
    m_attributesLayout->setSpacing(20);
    m_attributesLayout->setContentsMargins(0, 0, 8, 0);
    m_attributesLayout->setAlignment(Qt::AlignTop);
    scroll->setWidget(scrollHost);
    pageLayout->addWidget(scroll, 1);

    m_lblEmpty = new QLabel(m_pagePersonalize);
    m_lblEmpty->setObjectName(QStringLiteral("pkgPickEmpty"));
    m_lblEmpty->setWordWrap(true);
    m_lblEmpty->hide();
    pageLayout->addWidget(m_lblEmpty);

    auto *qtyWrap = new QFrame(m_pagePersonalize);
    qtyWrap->setObjectName(QStringLiteral("pkgPickQtyWrap"));
    auto *qtyLayout = new QHBoxLayout(qtyWrap);
    qtyLayout->setContentsMargins(4, 4, 4, 4);
    qtyLayout->setSpacing(0);
    auto *btnMinus = new QPushButton(QStringLiteral("−"), qtyWrap);
    btnMinus->setObjectName(QStringLiteral("pkgPickQtyMinus"));
    m_lblQty = new QLabel(QStringLiteral("1"), qtyWrap);
    m_lblQty->setObjectName(QStringLiteral("pkgPickQtyValue"));
    m_lblQty->setAlignment(Qt::AlignCenter);
    m_lblQty->setMinimumWidth(48);
    auto *btnPlus = new QPushButton(QStringLiteral("+"), qtyWrap);
    btnPlus->setObjectName(QStringLiteral("pkgPickQtyPlus"));
    connect(btnMinus, &QPushButton::clicked, this, &DlgPackagePick::decreaseQty);
    connect(btnPlus, &QPushButton::clicked, this, &DlgPackagePick::increaseQty);
    qtyLayout->addWidget(btnMinus);
    qtyLayout->addWidget(m_lblQty, 1);
    qtyLayout->addWidget(btnPlus);
    pageLayout->addWidget(qtyWrap);

    m_lblUnavailableHint = new QLabel(m_pagePersonalize);
    m_lblUnavailableHint->setObjectName(QStringLiteral("pkgPickUnavailableHint"));
    m_lblUnavailableHint->setWordWrap(true);
    m_lblUnavailableHint->hide();
    pageLayout->addWidget(m_lblUnavailableHint);

    m_contentStack->addWidget(m_pagePersonalize);
}

void DlgPackagePick::buildAttributeGroups()
{
    clearLayout(m_attributesLayout);
    m_attrGroups.clear();

    QWidget *attrHost = m_attributesLayout->parentWidget();
    const QStringList keys = MenuHelpers::packageAttributeKeys();
    const QHash<QString, QStringList> options = MenuHelpers::uniqueAttributeOptions(m_package.packageComponents, keys);

    for (const QString &key : keys) {
        const QStringList values = options.value(key);
        if (values.isEmpty()) {
            continue;
        }

        auto *section = new QFrame(attrHost);
        section->setObjectName(QStringLiteral("pkgPickAttrSection"));
        section->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        auto *sectionLayout = new QVBoxLayout(section);
        sectionLayout->setContentsMargins(0, 0, 0, 0);
        sectionLayout->setSpacing(8);
        sectionLayout->setAlignment(Qt::AlignTop);

        auto *header = new QLabel(sectionTitle(key), section);
        header->setObjectName(QStringLiteral("pkgPickAttrHeader"));
        sectionLayout->addWidget(header);

        auto *selectedBox = new QFrame(section);
        selectedBox->setObjectName(QStringLiteral("pkgPickAttrSelectedBox"));
        auto *selectedLayout = new QHBoxLayout(selectedBox);
        selectedLayout->setContentsMargins(16, 12, 16, 12);
        auto *selectedLabel = new QLabel(section);
        selectedLabel->setObjectName(QStringLiteral("pkgPickAttrSelectedLabel"));
        selectedLayout->addWidget(selectedLabel, 1);
        auto *arrow = new QLabel(QStringLiteral("▾"), selectedBox);
        arrow->setObjectName(QStringLiteral("pkgPickAttrSelectedArrow"));
        selectedLayout->addWidget(arrow);
        sectionLayout->addWidget(selectedBox);

        auto *optionsHost = new QFrame(section);
        optionsHost->setObjectName(QStringLiteral("pkgPickAttrOptions"));
        auto *optionsLayout = new QVBoxLayout(optionsHost);
        optionsLayout->setContentsMargins(0, 0, 0, 0);
        optionsLayout->setSpacing(0);

        auto *group = new QButtonGroup(section);
        group->setExclusive(true);
        m_attrGroups.insert(key, group);

        for (int i = 0; i < values.size(); ++i) {
            const QString &value = values.at(i);

            auto *row = new AttrOptionRow(optionsHost);
            auto *rowLayout = new QHBoxLayout(row);
            rowLayout->setContentsMargins(16, 12, 16, 12);
            rowLayout->setSpacing(12);

            auto *radio = new QRadioButton(row);
            radio->setObjectName(QStringLiteral("pkgPickAttrRadio"));
            radio->setProperty("attrKey", key);
            radio->setProperty("attrValue", value);
            radio->setProperty("selectedLabel", QVariant::fromValue<QObject *>(selectedLabel));
            radio->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            group->addButton(radio);
            row->setRadio(radio);

            rowLayout->addWidget(radio, 1);

            connect(radio, &QRadioButton::toggled, this, [this, radio](bool checked) {
                if (!checked) {
                    return;
                }
                if (auto *lbl = qobject_cast<QLabel *>(radio->property("selectedLabel").value<QObject *>())) {
                    lbl->setText(formatOptionLabel(radio->property("attrKey").toString(),
                                                   radio->property("attrValue").toString()));
                }
                onAttributeChanged();
            });

            optionsLayout->addWidget(row);

            if (i + 1 < values.size()) {
                auto *divider = new QFrame(optionsHost);
                divider->setObjectName(QStringLiteral("pkgPickAttrDivider"));
                divider->setFrameShape(QFrame::HLine);
                divider->setFixedHeight(1);
                optionsLayout->addWidget(divider);
            }
        }

        sectionLayout->addWidget(optionsHost);
        m_attributesLayout->addWidget(section, 0, Qt::AlignTop);
    }
    m_attributesLayout->addStretch();
    selectInitialSelections();
    updateOptionPrices();
    updatePreview();
}

void DlgPackagePick::buildModificatorGroups()
{
    m_modificatorChecks.clear();
    const QVector<MenuModificatorOption> options = MenuHelpers::pickerModificators(m_package);
    if (options.isEmpty() || !m_attributesLayout) {
        return;
    }

    QWidget *attrHost = m_attributesLayout->parentWidget();
    auto *section = new QFrame(attrHost);
    section->setObjectName(QStringLiteral("pkgPickAttrSection"));
    section->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    auto *sectionLayout = new QVBoxLayout(section);
    sectionLayout->setContentsMargins(0, 0, 0, 0);
    sectionLayout->setSpacing(8);
    sectionLayout->setAlignment(Qt::AlignTop);

    auto *header = new QLabel(tr("Toppings"), section);
    header->setObjectName(QStringLiteral("pkgPickAttrHeader"));
    sectionLayout->addWidget(header);

    for (int i = 0; i < options.size(); ++i) {
        const MenuModificatorOption &option = options.at(i);

        auto *row = new ModOptionRow(section);
        row->setObjectName(QStringLiteral("pkgPickModCard"));
        auto *rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(16, 12, 16, 12);
        rowLayout->setSpacing(12);

        auto *nameCol = new QVBoxLayout();
        nameCol->setSpacing(2);
        auto *nameLbl = new QLabel(option.name, row);
        nameLbl->setObjectName(QStringLiteral("pkgPickModName"));
        nameCol->addWidget(nameLbl);
        if (option.price > 0.0) {
            auto *priceLbl = new QLabel(tr("+%1").arg(QString::number(option.price, 'f', 0)), row);
            priceLbl->setObjectName(QStringLiteral("pkgPickModPrice"));
            nameCol->addWidget(priceLbl);
        }
        rowLayout->addLayout(nameCol, 1);

        auto *checkBox = new QCheckBox(row);
        checkBox->setObjectName(QStringLiteral("pkgPickModCheck"));
        checkBox->setProperty("modificatorId", option.id);
        checkBox->setChecked(option.required);
        checkBox->setText(option.required ? QStringLiteral("*") : QString());
        row->setCheckBox(checkBox);
        m_modificatorChecks.insert(option.id, checkBox);
        rowLayout->addWidget(checkBox, 0, Qt::AlignRight);

        connect(checkBox, &QCheckBox::toggled, this, &DlgPackagePick::onAttributeChanged);
        sectionLayout->addWidget(row);
    }

    m_attributesLayout->insertWidget(qMax(0, m_attributesLayout->count() - 1), section, 0, Qt::AlignTop);
}

void DlgPackagePick::selectInitialSelections()
{
    if (m_package.packageComponents.isEmpty()) {
        return;
    }
    const MenuPackageComponent &first = m_package.packageComponents.first();
    const QHash<QString, QString> values = {
        {QStringLiteral("Type"), first.attributes.type},
        {QStringLiteral("Size"), first.attributes.size},
    };

    for (auto it = m_attrGroups.constBegin(); it != m_attrGroups.constEnd(); ++it) {
        const QString value = values.value(it.key());
        if (value.isEmpty()) {
            continue;
        }
        QButtonGroup *group = it.value();
        for (QAbstractButton *btn : group->buttons()) {
            if (btn->property("attrValue").toString() == value) {
                btn->setChecked(true);
                if (auto *lbl = qobject_cast<QLabel *>(btn->property("selectedLabel").value<QObject *>())) {
                    lbl->setText(formatOptionLabel(it.key(), value));
                }
                break;
            }
        }
    }
}

void DlgPackagePick::rebuildHeaderInfo()
{
    if (!m_badgesLayout || !m_bjuLayout) {
        return;
    }
    clearLayout(m_badgesLayout);
    clearLayout(m_bjuLayout);

    const MenuDish &dish = m_package;

    const auto addDietaryBadge = [&](bool enabled, const char *iconPath) {
        if (!enabled) {
            return;
        }
        addBadgeIcon(m_badgesRow, m_badgesLayout, true, iconPath);
    };

    addDietaryBadge(dish.glutenFree, ":/dietary/gluten-free.png");
    addDietaryBadge(dish.vegetarian, ":/dietary/vegitarian.png");
    addDietaryBadge(dish.vegan, ":/dietary/vegan.png");
    addDietaryBadge(dish.noGmo, ":/dietary/no-gmo.png");
    addDietaryBadge(dish.noLactose, ":/dietary/lactose-free.png");
    addDietaryBadge(dish.noSugar, ":/dietary/sugar-free.png");
    addDietaryBadge(dish.containsNuts, ":/dietary/contain-nuts.png");
    if (dish.halalKosher) {
        addDietaryBadge(true, ":/dietary/halal.png");
        addDietaryBadge(true, ":/dietary/kosher.png");
    }
    m_badgesLayout->addStretch();

    if (dish.fat > 0.0) {
        addBjuBox(m_bjuRow, m_bjuLayout,
                  tr("%1 gr").arg(QString::number(dish.fat, 'f', 0)),
                  tr("Fats"));
    }
    if (dish.carbs > 0.0) {
        addBjuBox(m_bjuRow, m_bjuLayout,
                  tr("%1 gr").arg(QString::number(dish.carbs, 'f', 0)),
                  tr("Carbs"));
    }
    if (dish.protein > 0.0) {
        addBjuBox(m_bjuRow, m_bjuLayout,
                  tr("%1 gr").arg(QString::number(dish.protein, 'f', 0)),
                  tr("Protein"));
    }
    if (dish.kcal > 0.0) {
        addBjuBox(m_bjuRow, m_bjuLayout,
                  tr("%1 kcal").arg(QString::number(dish.kcal, 'f', dish.kcal >= 100 ? 0 : 1)),
                  tr("Calories"));
    }
    m_bjuLayout->addStretch();

    if (m_badgesRow) {
        m_badgesRow->setVisible(m_badgesLayout->count() > 0);
    }
    if (m_bjuRow) {
        m_bjuRow->setVisible(m_bjuLayout->count() > 0);
    }
}

QString DlgPackagePick::measurementForKey(const QString &key, const QString &value) const
{
    for (const MenuPackageComponent &component : m_package.packageComponents) {
        QString actual;
        if (key == QStringLiteral("Type")) {
            actual = component.attributes.type;
        } else if (key == QStringLiteral("Size")) {
            actual = component.attributes.size;
        }
        if (actual == value) {
            if (key == QStringLiteral("Size")) {
                return component.attributes.measurement;
            }
            return {};
        }
    }
    return {};
}

QString DlgPackagePick::selectedValueForKey(const QString &key) const
{
    const QButtonGroup *group = m_attrGroups.value(key);
    if (!group) {
        return {};
    }
    for (QAbstractButton *btn : group->buttons()) {
        if (btn->isChecked()) {
            return btn->property("attrValue").toString();
        }
    }
    return {};
}

QHash<QString, QString> DlgPackagePick::currentSelections() const
{
    QHash<QString, QString> selections;
    for (auto it = m_attrGroups.constBegin(); it != m_attrGroups.constEnd(); ++it) {
        const QString value = selectedValueForKey(it.key());
        if (!value.isEmpty()) {
            selections.insert(it.key(), value);
        }
    }
    return selections;
}

void DlgPackagePick::updateOptionPrices()
{
    for (auto it = m_attrGroups.constBegin(); it != m_attrGroups.constEnd(); ++it) {
        const QString &key = it.key();
        QButtonGroup *group = it.value();
        for (QAbstractButton *btn : group->buttons()) {
            const QString value = btn->property("attrValue").toString();
            btn->setText(formatOptionLabel(key, value));
            if (btn->isChecked()) {
                if (auto *lbl = qobject_cast<QLabel *>(btn->property("selectedLabel").value<QObject *>())) {
                    lbl->setText(formatOptionLabel(key, value));
                }
            }
        }
    }
}

double DlgPackagePick::currentUnitPrice() const
{
    double unitPrice = m_package.price;
    if (m_package.isPackage() && !m_package.packageComponents.isEmpty()) {
        const MenuPackageComponent *component = nullptr;
        if (MenuHelpers::packageNeedsAttributePicker(m_package.packageComponents)) {
            component = MenuHelpers::findPackageComponent(
                m_package.packageComponents, currentSelections());
        } else {
            component = &m_package.packageComponents.first();
        }
        if (component) {
            unitPrice = MenuHelpers::packageLinePrice(m_package, *component);
        }
    }
    unitPrice += selectedModificatorsPrice();
    return unitPrice;
}

void DlgPackagePick::updatePreview()
{
    updateOptionPrices();
    updateFooter();

    const bool combinationValid = attributeSelectionValid();
    if (m_lblUnavailableHint) {
        if (combinationValid || m_step != PickStep::Personalize) {
            m_lblUnavailableHint->hide();
        } else {
            m_lblUnavailableHint->setText(
                tr("This combination is not available. Please change your selection."));
            m_lblUnavailableHint->show();
        }
    }
}

void DlgPackagePick::updateFooter()
{
    const double total = currentUnitPrice() * m_quantity + relatedSelectionTotal();
    if (m_lblTotal) {
        m_lblTotal->setText(tr("Total: %1 AMD").arg(QString::number(total, 'f', 0)));
    }
    if (m_btnNext) {
        m_btnNext->setEnabled(canProceedFromCurrentStep());
        if (m_step == PickStep::Personalize) {
            if (m_hasDrinkStep) {
                m_btnNext->setText(tr("Choose drink >"));
            } else if (m_hasExtraStep) {
                m_btnNext->setText(tr("Add extra >"));
            } else {
                m_btnNext->setText(tr("Add to cart >"));
            }
        } else if (m_step == PickStep::Drink) {
            m_btnNext->setText(m_hasExtraStep ? tr("Add extra >") : tr("Add to cart >"));
        } else {
            m_btnNext->setText(tr("Add to cart >"));
        }
    }

    if (m_btnBack) {
        m_btnBack->setVisible(m_step != PickStep::Personalize);
        if (m_step == PickStep::Drink) {
            m_btnBack->setText(tr("< Personalize"));
        } else if (m_step == PickStep::Extra) {
            m_btnBack->setText(m_hasDrinkStep ? tr("< Choose drink") : tr("< Personalize"));
        }
    }
}

void DlgPackagePick::updateStepUi()
{
    for (QWidget *wrap : m_stepItems) {
        auto *item = wrap->findChild<QFrame *>(QStringLiteral("pkgPickStepItem"));
        if (!item) {
            continue;
        }
        const int stepIndex = item->property("stepIndex").toInt();
        const bool isActive = stepIndex == static_cast<int>(m_step);
        const bool isDone = stepIndex < static_cast<int>(m_step);
        const bool isSelected = isActive || isDone;

        item->setProperty("done", isDone);
        item->setProperty("active", isActive);
        item->setProperty("selected", isSelected);
        item->style()->unpolish(item);
        item->style()->polish(item);

        QLabel *indicator = item->findChild<QLabel *>(QStringLiteral("pkgPickStepIndicator"));
        if (!indicator) {
            continue;
        }
        indicator->setProperty("checked", isSelected);
        if (isSelected) {
            indicator->setText(QStringLiteral("✓"));
        } else {
            indicator->setText(QString());
        }
        indicator->style()->unpolish(indicator);
        indicator->style()->polish(indicator);
    }

    if (m_btnBack) {
        m_btnBack->setVisible(m_step != PickStep::Personalize);
    }
}

void DlgPackagePick::setStep(DlgPackagePick::PickStep step)
{
    m_step = step;
    if (m_contentStack) {
        m_contentStack->setCurrentIndex(static_cast<int>(step));
    }
    updateStepUi();
    updatePreview();
}

bool DlgPackagePick::canProceedFromCurrentStep() const
{
    switch (m_step) {
    case PickStep::Personalize:
        return attributeSelectionValid() && requiredModificatorsSatisfied();
    case PickStep::Drink:
    case PickStep::Extra:
        return true;
    }
    return false;
}

void DlgPackagePick::onAttributeChanged()
{
    updatePreview();
}

void DlgPackagePick::decreaseQty()
{
    if (m_quantity > 1) {
        --m_quantity;
        m_lblQty->setText(QString::number(m_quantity));
        updatePreview();
    }
}

void DlgPackagePick::increaseQty()
{
    ++m_quantity;
    m_lblQty->setText(QString::number(m_quantity));
    updatePreview();
}

void DlgPackagePick::onCloseClicked()
{
    emit rejected();
}

void DlgPackagePick::onGoToMenuClicked()
{
    emit rejected();
}

void DlgPackagePick::onBackClicked()
{
    if (m_step == PickStep::Personalize) {
        return;
    }
    setStep(previousStepBefore(m_step));
}

void DlgPackagePick::onNextClicked()
{
    if (!canProceedFromCurrentStep()) {
        return;
    }

    const PickStep next = nextStepAfter(m_step);
    if (m_step == PickStep::Personalize && !m_hasDrinkStep && !m_hasExtraStep) {
        onAddClicked();
        return;
    }
    if (m_step == PickStep::Drink && !m_hasExtraStep) {
        onAddClicked();
        return;
    }
    if (m_step == PickStep::Extra) {
        onAddClicked();
        return;
    }
    setStep(next);
}

void DlgPackagePick::onAddClicked()
{
    if (!attributeSelectionValid() || !requiredModificatorsSatisfied()) {
        return;
    }

    MenuDish line;
    if (m_package.isPackage() && !m_package.packageComponents.isEmpty()) {
        const MenuPackageComponent *component = nullptr;
        if (MenuHelpers::packageNeedsAttributePicker(m_package.packageComponents)) {
            component = MenuHelpers::findPackageComponent(
                m_package.packageComponents, currentSelections());
        } else {
            component = &m_package.packageComponents.first();
        }
        if (!component) {
            return;
        }
        line = MenuHelpers::resolvePackageCartLine(m_package, component);
    } else {
        line = m_package;
        line.selectedModificators.clear();
    }

    MenuHelpers::applySelectedModificators(line, selectedModificators());
    emit accepted(line, m_quantity, selectedExtraLines());
}

bool DlgPackagePick::attributeSelectionValid() const
{
    if (!m_package.isPackage() || m_package.packageComponents.isEmpty()) {
        return true;
    }
    if (!MenuHelpers::packageNeedsAttributePicker(m_package.packageComponents)) {
        return true;
    }
    return MenuHelpers::findPackageComponent(m_package.packageComponents, currentSelections()) != nullptr;
}

QVector<MenuSelectedModificator> DlgPackagePick::selectedModificators() const
{
    QVector<MenuSelectedModificator> result;
    for (const MenuModificatorOption &option : MenuHelpers::pickerModificators(m_package)) {
        QCheckBox *checkBox = m_modificatorChecks.value(option.id);
        if (!checkBox || !checkBox->isChecked()) {
            continue;
        }
        MenuSelectedModificator selected;
        selected.id = option.id;
        selected.name = option.name;
        selected.price = option.price;
        result.append(selected);
    }
    return result;
}

bool DlgPackagePick::requiredModificatorsSatisfied() const
{
    for (const MenuModificatorOption &option : MenuHelpers::pickerModificators(m_package)) {
        if (!option.required) {
            continue;
        }
        QCheckBox *checkBox = m_modificatorChecks.value(option.id);
        if (!checkBox || !checkBox->isChecked()) {
            return false;
        }
    }
    return true;
}

double DlgPackagePick::selectedModificatorsPrice() const
{
    double sum = 0.0;
    for (const MenuSelectedModificator &modificator : selectedModificators()) {
        sum += modificator.price;
    }
    return sum;
}
