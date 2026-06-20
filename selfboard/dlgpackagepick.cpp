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
#include <QLineEdit>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QStackedWidget>
#include <QStyle>
#include <QVBoxLayout>

namespace {

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
    icon->setFixedSize(36, 36);
    icon->setScaledContents(true);
    icon->setPixmap(QPixmap(QString::fromUtf8(iconPath)));
    layout->addWidget(icon);
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
    showPackageBadges();
    showPackageBju();

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
    const int marginH = 24;
    const int marginV = 24;
    m_card->setGeometry(rect().adjusted(marginH, marginV, -marginH, -marginV));
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

QWidget *DlgPackagePick::makeStepSidebarItem(int index, const QString &title)
{
    auto *item = new QFrame(m_card);
    item->setObjectName(QStringLiteral("pkgPickStepItem"));
    item->setProperty("stepIndex", index);

    auto *layout = new QHBoxLayout(item);
    layout->setContentsMargins(16, 14, 16, 14);
    layout->setSpacing(12);

    auto *indicator = new QLabel(item);
    indicator->setObjectName(QStringLiteral("pkgPickStepIndicator"));
    indicator->setFixedSize(28, 28);
    indicator->setAlignment(Qt::AlignCenter);
    indicator->setProperty("stepIndicator", true);

    auto *label = new QLabel(title, item);
    label->setObjectName(QStringLiteral("pkgPickStepLabel"));
    label->setWordWrap(true);

    layout->addWidget(indicator, 0, Qt::AlignTop);
    layout->addWidget(label, 1);

    m_stepItems.append(item);
    return item;
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
    auto *grid = new QGridLayout(gridHost);
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setSpacing(16);

    for (int i = 0; i < picks.size(); ++i) {
        RelatedPickState &pick = picks[i];
        auto *card = new QFrame(gridHost);
        card->setObjectName(QStringLiteral("pkgPickStubCard"));
        auto *cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(16, 16, 16, 16);
        cardLayout->setSpacing(8);

        auto *img = new QLabel(card);
        img->setObjectName(QStringLiteral("pkgPickStubImage"));
        img->setFixedHeight(120);
        img->setAlignment(Qt::AlignCenter);
        setDishImageOnLabel(img, pick.dish.imagePath, 120, 120);

        auto *name = new QLabel(pick.dish.name, card);
        name->setObjectName(QStringLiteral("pkgPickStubName"));
        name->setWordWrap(true);

        auto *price = new QLabel(
            tr("%1 ֏").arg(QString::number(pick.dish.price, 'f', 0)), card);
        price->setObjectName(QStringLiteral("pkgPickStubPrice"));

        auto *qtyRow = new QHBoxLayout();
        qtyRow->setSpacing(8);
        auto *btnMinus = new QPushButton(QStringLiteral("−"), card);
        btnMinus->setObjectName(QStringLiteral("pkgPickQtyMinus"));
        btnMinus->setEnabled(pick.qty > 0);
        pick.qtyLabel = new QLabel(QString::number(pick.qty), card);
        pick.qtyLabel->setObjectName(QStringLiteral("pkgPickQtyValue"));
        pick.qtyLabel->setAlignment(Qt::AlignCenter);
        pick.qtyLabel->setMinimumWidth(36);
        auto *btnPlus = new QPushButton(QStringLiteral("+"), card);
        btnPlus->setObjectName(QStringLiteral("pkgPickQtyPlus"));
        qtyRow->addWidget(btnMinus);
        qtyRow->addWidget(pick.qtyLabel, 1);
        qtyRow->addWidget(btnPlus);

        connect(btnMinus, &QPushButton::clicked, this, [this, &picks, i]() {
            changeRelatedQty(picks, i, -1);
        });
        connect(btnPlus, &QPushButton::clicked, this, [this, &picks, i]() {
            changeRelatedQty(picks, i, 1);
        });

        cardLayout->addWidget(img);
        cardLayout->addWidget(name);
        cardLayout->addWidget(price);
        cardLayout->addLayout(qtyRow);

        grid->addWidget(card, i / 3, i % 3);
    }

    scroll->setWidget(gridHost);
    layout->addWidget(scroll, 1);

    auto *hintLbl = new QLabel(tr("Optional - skip if you do not want to add anything."), page);
    hintLbl->setObjectName(QStringLiteral("pkgPickStubHint"));
    hintLbl->setWordWrap(true);
    layout->addWidget(hintLbl);

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

void DlgPackagePick::changeRelatedQty(QVector<RelatedPickState> &picks, int index, int delta)
{
    if (index < 0 || index >= picks.size()) {
        return;
    }
    picks[index].qty = qMax(0, picks[index].qty + delta);
    if (picks[index].qtyLabel) {
        picks[index].qtyLabel->setText(QString::number(picks[index].qty));
    }
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
    sidebar->setFixedWidth(260);
    m_sidebarLayout = new QVBoxLayout(sidebar);
    m_sidebarLayout->setContentsMargins(0, 0, 0, 0);
    m_sidebarLayout->setSpacing(10);

    auto *sidebarTitle = new QLabel(tr("Your choice"), sidebar);
    sidebarTitle->setObjectName(QStringLiteral("pkgPickSidebarTitle"));
    m_sidebarLayout->addWidget(sidebarTitle);

    const QString personalizeTitle = tr("Personalize %1").arg(m_package.name);
    m_sidebarLayout->addWidget(makeStepSidebarItem(0, personalizeTitle));
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
    m_lblImage->setFixedSize(160, 160);
    m_lblImage->setAlignment(Qt::AlignCenter);
    setDishImageOnLabel(m_lblImage, m_package.imagePath, 160, 160);
    header->addWidget(m_lblImage, 0, Qt::AlignTop);

    auto *info = new QVBoxLayout();
    info->setSpacing(10);

    m_lblTitle = new QLabel(m_package.name, m_card);
    m_lblTitle->setObjectName(QStringLiteral("pkgPickTitle"));
    info->addWidget(m_lblTitle);

    m_lblDescription = new QLabel(m_package.description, m_card);
    m_lblDescription->setObjectName(QStringLiteral("pkgPickDescription"));
    m_lblDescription->setWordWrap(true);
    info->addWidget(m_lblDescription);

    m_lblBjuPills = new QLabel(m_card);
    m_lblBjuPills->setObjectName(QStringLiteral("pkgPickBjuPills"));
    m_lblBjuPills->setWordWrap(true);
    info->addWidget(m_lblBjuPills);

    m_badgesHost = new QWidget(m_card);
    m_badgesLayout = new QHBoxLayout(m_badgesHost);
    m_badgesLayout->setContentsMargins(0, 0, 0, 0);
    m_badgesLayout->setSpacing(10);
    info->addWidget(m_badgesHost);

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

    m_leInstructions = new QLineEdit(m_pagePersonalize);
    m_leInstructions->setObjectName(QStringLiteral("pkgPickInstructions"));
    m_leInstructions->setPlaceholderText(
        tr("Add any special instructions (allergies, important and details)"));
    pageLayout->addWidget(m_leInstructions);

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

void DlgPackagePick::showPackageBadges()
{
    clearLayout(m_badgesLayout);
    const MenuDish &dish = m_package;
    addBadgeIcon(m_badgesHost, m_badgesLayout, dish.glutenFree, ":/dietary/gluten-free.png");
    addBadgeIcon(m_badgesHost, m_badgesLayout, dish.vegetarian, ":/dietary/vegitarian.png");
    addBadgeIcon(m_badgesHost, m_badgesLayout, dish.vegan, ":/dietary/vegan.png");
    addBadgeIcon(m_badgesHost, m_badgesLayout, dish.noGmo, ":/dietary/no-gmo.png");
    addBadgeIcon(m_badgesHost, m_badgesLayout, dish.noLactose, ":/dietary/lactose-free.png");
    addBadgeIcon(m_badgesHost, m_badgesLayout, dish.noSugar, ":/dietary/sugar-free.png");
    addBadgeIcon(m_badgesHost, m_badgesLayout, dish.containsNuts, ":/dietary/contain-nuts.png");
    if (dish.halalKosher) {
        addBadgeIcon(m_badgesHost, m_badgesLayout, true, ":/dietary/halal.png");
        addBadgeIcon(m_badgesHost, m_badgesLayout, true, ":/dietary/kosher.png");
    }
    m_badgesLayout->addStretch();
}

void DlgPackagePick::showPackageBju()
{
    const MenuDish &dish = m_package;
    QStringList pills;
    if (dish.fat > 0.0) {
        pills << tr("%1 gr Fats").arg(QString::number(dish.fat, 'f', 0));
    }
    if (dish.carbs > 0.0) {
        pills << tr("%1 gr Carbs").arg(QString::number(dish.carbs, 'f', 0));
    }
    if (dish.protein > 0.0) {
        pills << tr("%1 gr Protein").arg(QString::number(dish.protein, 'f', 0));
    }
    if (dish.kcal > 0.0) {
        pills << tr("%1 kcal Calories").arg(QString::number(dish.kcal, 'f', dish.kcal >= 100 ? 0 : 1));
    }
    if (pills.isEmpty()) {
        m_lblBjuPills->hide();
    } else {
        m_lblBjuPills->setText(pills.join(QStringLiteral("   ")));
        m_lblBjuPills->show();
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
    for (int i = 0; i < m_stepItems.size(); ++i) {
        QFrame *item = m_stepItems.at(i);
        const int stepIndex = item->property("stepIndex").toInt();

        item->setProperty("done", stepIndex < static_cast<int>(m_step));
        item->setProperty("active", stepIndex == static_cast<int>(m_step));
        item->style()->unpolish(item);
        item->style()->polish(item);

        QLabel *indicator = nullptr;
        for (QLabel *lbl : item->findChildren<QLabel *>()) {
            if (lbl->property("stepIndicator").toBool()) {
                indicator = lbl;
                break;
            }
        }
        if (!indicator) {
            continue;
        }
        if (stepIndex < static_cast<int>(m_step)) {
            indicator->setText(QStringLiteral("✓"));
        } else {
            indicator->setText(QString());
        }
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
