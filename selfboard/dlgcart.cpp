#include "dlgcart.h"
#include "dlgmenu.h"
#include "dishimageutils.h"
#include "selfboardlanguage.h"
#include "ui_dlgcart.h"

#include <QEvent>
#include <QFile>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayoutItem>
#include <QMenu>
#include <QAction>
#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QColor>
#include <QPalette>
#include <QScroller>
#include <QVBoxLayout>

namespace {

void styleWhiteCartButton(QPushButton *btn)
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

} // namespace

namespace {

constexpr int kCartLineImageSize = 120;
constexpr int kCartRightColumnWidth = 200;

QString packageVariantText(const MenuDish &dish)
{
    QStringList parts;
    if (!dish.attrType.isEmpty()) {
        parts << dish.attrType;
    }
    if (!dish.attrSize.isEmpty()) {
        parts << dish.attrSize;
    }
    return parts.join(QStringLiteral(" · "));
}

QWidget *makeCartLineWidget(const CartLine &line,
                            const QString &subtitle,
                            const QString &modifiers,
                            QWidget *parent,
                            DlgCart *cartDialog)
{
    auto *card = new QFrame(parent);
    card->setObjectName(QStringLiteral("cartLineCard"));
    card->setFrameShape(QFrame::NoFrame);
    card->setAttribute(Qt::WA_StyledBackground, true);
    card->setAutoFillBackground(true);
    card->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    QPalette cardPalette = card->palette();
    cardPalette.setColor(QPalette::Window, QColor(0xff, 0xff, 0xff));
    cardPalette.setColor(QPalette::Base, QColor(0xff, 0xff, 0xff));
    card->setPalette(cardPalette);
    card->setStyleSheet(QStringLiteral(
        "QFrame#cartLineCard {"
        "  background-color: #ffffff;"
        "  border: none;"
        "  border-radius: 16px;"
        "}"
        "QFrame#cartLineCard QLabel {"
        "  background: transparent;"
        "  border: none;"
        "}"));

    auto *layout = new QHBoxLayout(card);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(20);

    auto *img = new QLabel(card);
    img->setObjectName(QStringLiteral("cartLineImage"));
    img->setFixedSize(kCartLineImageSize, kCartLineImageSize);
    setCenteredDishImageOnLabel(img, line.dish.imagePath, kCartLineImageSize, kCartLineImageSize);

    auto *centerLayout = new QVBoxLayout();
    centerLayout->setSpacing(6);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    centerLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    auto *name = new QLabel(line.dish.name, card);
    name->setObjectName(QStringLiteral("cartLineName"));
    name->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    styleTransparentTextLabel(name);
    centerLayout->addWidget(name);

    if (!subtitle.isEmpty()) {
        auto *variant = new QLabel(subtitle, card);
        variant->setObjectName(line.dish.packageId > 0 ? QStringLiteral("cartLineVariant")
                                                       : QStringLiteral("cartLineCategory"));
        variant->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        variant->setWordWrap(true);
        styleTransparentTextLabel(variant);
        centerLayout->addWidget(variant);
    }

    if (!modifiers.isEmpty()) {
        auto *mods = new QLabel(modifiers, card);
        mods->setObjectName(QStringLiteral("cartLineModifiers"));
        mods->setWordWrap(true);
        mods->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        styleTransparentTextLabel(mods);
        centerLayout->addWidget(mods);
    }

    auto *rightHost = new QWidget(card);
    rightHost->setObjectName(QStringLiteral("cartLineRightHost"));
    rightHost->setFixedWidth(kCartRightColumnWidth);
    rightHost->setAttribute(Qt::WA_StyledBackground, false);
    rightHost->setStyleSheet(QStringLiteral("background: transparent;"));

    auto *rightLayout = new QVBoxLayout(rightHost);
    rightLayout->setSpacing(4);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setAlignment(Qt::AlignTop);

    const QString unitPrice = QString::number(line.dish.price, 'f', 0);
    const QString lineTotal = QString::number(line.dish.price * line.quantity, 'f', 0);

    auto *unitPriceLabel = new QLabel(QObject::tr("%1 AMD").arg(unitPrice), rightHost);
    unitPriceLabel->setObjectName(QStringLiteral("cartLineUnitPrice"));
    unitPriceLabel->setAlignment(Qt::AlignRight | Qt::AlignTop);
    styleTransparentTextLabel(unitPriceLabel);

    auto *totalLabel = new QLabel(QObject::tr("Total: %1 AMD").arg(lineTotal), rightHost);
    totalLabel->setObjectName(QStringLiteral("cartLineTotalPrice"));
    totalLabel->setAlignment(Qt::AlignRight | Qt::AlignTop);
    styleTransparentTextLabel(totalLabel);

    rightLayout->addWidget(unitPriceLabel);
    rightLayout->addWidget(totalLabel);
    rightLayout->addStretch();

    auto *actionsRow = new QHBoxLayout();
    actionsRow->setSpacing(12);
    actionsRow->setContentsMargins(0, 0, 0, 0);

    const bool isPackageLine = line.dish.packageId > 0;

    auto *editLabel = new QLabel(QObject::tr("Edit item"), rightHost);
    editLabel->setObjectName(QStringLiteral("cartLineEditLabel"));
    editLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    styleTransparentTextLabel(editLabel);
    editLabel->setVisible(isPackageLine);
    if (isPackageLine) {
        editLabel->setCursor(Qt::PointingHandCursor);
    }

    auto *qtyWrap = new QWidget(rightHost);
    qtyWrap->setObjectName(QStringLiteral("cartQtyWrap"));
    qtyWrap->setAttribute(Qt::WA_StyledBackground, true);
    auto *qtyLayout = new QHBoxLayout(qtyWrap);
    qtyLayout->setContentsMargins(4, 4, 4, 4);
    qtyLayout->setSpacing(0);

    auto *btnMinus = new QPushButton(QStringLiteral("−"), qtyWrap);
    btnMinus->setObjectName(QStringLiteral("cartQtyMinus"));
    btnMinus->setProperty("lineKey", line.key);
    btnMinus->setCursor(Qt::PointingHandCursor);

    auto *lblQty = new QLabel(QString::number(line.quantity), qtyWrap);
    lblQty->setObjectName(QStringLiteral("cartQtyValue"));
    lblQty->setAlignment(Qt::AlignCenter);
    styleTransparentTextLabel(lblQty);
    lblQty->setMinimumWidth(36);

    auto *btnPlus = new QPushButton(QStringLiteral("+"), qtyWrap);
    btnPlus->setObjectName(QStringLiteral("cartQtyPlus"));
    btnPlus->setProperty("lineKey", line.key);
    btnPlus->setCursor(Qt::PointingHandCursor);

    qtyLayout->addWidget(btnMinus);
    qtyLayout->addWidget(lblQty);
    qtyLayout->addWidget(btnPlus);

    actionsRow->addStretch();
    if (isPackageLine) {
        actionsRow->addWidget(editLabel, 0, Qt::AlignVCenter);
    }
    actionsRow->addWidget(qtyWrap, 0, Qt::AlignVCenter);
    rightLayout->addLayout(actionsRow);

    layout->addWidget(img, 0, Qt::AlignTop);
    layout->addLayout(centerLayout, 1);
    layout->addWidget(rightHost, 0, Qt::AlignTop);

    QObject::connect(btnMinus, &QPushButton::clicked, cartDialog, [cartDialog, line, lblQty]() {
        cartDialog->onQuantityChanged(line.key, lblQty->text().toInt() - 1);
    });
    QObject::connect(btnPlus, &QPushButton::clicked, cartDialog, [cartDialog, line, lblQty]() {
        cartDialog->onQuantityChanged(line.key, lblQty->text().toInt() + 1);
    });

    auto *rowWrap = new QWidget(parent);
    rowWrap->setObjectName(QStringLiteral("cartLineRow"));
    rowWrap->setAttribute(Qt::WA_StyledBackground, true);
    rowWrap->setAutoFillBackground(false);
    rowWrap->setStyleSheet(QStringLiteral("background: transparent;"));
    rowWrap->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    auto *rowLayout = new QHBoxLayout(rowWrap);
    rowLayout->setContentsMargins(24, 0, 24, 0);
    rowLayout->setSpacing(0);
    rowLayout->addWidget(card);

    return rowWrap;
}

} // namespace

DlgCart::DlgCart(OrderCart *cart, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DlgCart)
    , m_cart(cart)
    , m_menu(qobject_cast<DlgMenu *>(parent))
{
    ui->setupUi(this);
    setObjectName(QStringLiteral("DlgCart"));
    setAttribute(Qt::WA_StyledBackground, true);
    setAutoFillBackground(true);
    if (!parent) {
        setFixedSize(1080, 1920);
    }

    setupAppearance();
    setupHeaderLayout();
    setupServiceModeButton();
    setupOrderBarSummary();
    setupTouchScroll();

    ui->lblLogo->setPixmap(QPixmap(QStringLiteral(":/res/main_logo.png")));
    ui->lblFooterLogo->setPixmap(QPixmap(QStringLiteral(":/res/footer_logo_myqr.png")));

    updateServiceModeLabel();
    ui->btnServiceMode->setCursor(Qt::PointingHandCursor);
    connect(ui->btnServiceMode, &QPushButton::clicked, this, &DlgCart::onServiceModeClicked);

    rebuildCartList();
    updateSummary();
}

DlgCart::~DlgCart()
{
    delete ui;
}

void DlgCart::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        updateServiceModeLabel();
        rebuildCartList();
        updateSummary();
    }
    QWidget::changeEvent(event);
}

void DlgCart::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(rect(), QColor(0xf3, 0xf3, 0xf3));
}

void DlgCart::setupAppearance()
{
    QFile styleFile(QStringLiteral(":/res/selfboard.css"));
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setStyleSheet(QString::fromUtf8(styleFile.readAll()));
    }

    ui->widgetHeader->setAttribute(Qt::WA_StyledBackground, true);
    ui->widgetHeaderLeft->setAttribute(Qt::WA_StyledBackground, true);
    ui->widgetOrderBar->setAttribute(Qt::WA_StyledBackground, true);
    ui->widgetFooter->setAttribute(Qt::WA_StyledBackground, true);

    SelfboardLanguage::instance().bindPickerButton(ui->btnLanguage);
}

void DlgCart::setupHeaderLayout()
{
    ui->lblLogo->setFixedSize(248, 225);
    ui->lblLogo->setScaledContents(true);
    ui->lblBanner->setFixedHeight(225);
    ui->lblBanner->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->headerTopRow->setAlignment(Qt::AlignTop);
    ui->headerTopRow->setStretch(0, 0);
    ui->headerTopRow->setStretch(1, 1);
    ui->headerLayout->setAlignment(Qt::AlignTop);
    ui->headerLeftLayout->setAlignment(Qt::AlignTop);
    ui->widgetHeader->setMinimumHeight(0);
    ui->widgetHeader->setMaximumHeight(QWIDGETSIZE_MAX);
    ui->widgetHeader->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
}

void DlgCart::setupServiceModeButton()
{
    QPushButton *btn = ui->btnServiceMode;
    if (btn->findChild<QLabel *>(QStringLiteral("lblServiceModeReload"))) {
        return;
    }

    styleWhiteCartButton(btn);
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

void DlgCart::updateServiceModeLabel()
{
    if (!m_menu) {
        return;
    }

    const bool takeAway = m_menu->serviceMode() == DlgMenu::ServiceMode::TakeAway;
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

void DlgCart::onServiceModeClicked()
{
    if (!m_menu) {
        return;
    }

    QMenu menu(this);
    menu.setObjectName(QStringLiteral("menuServiceMode"));

    const auto addMode = [&](DlgMenu::ServiceMode mode, const QString &label, const QString &iconPath) {
        QAction *action = menu.addAction(QIcon(iconPath), label);
        if (mode == m_menu->serviceMode()) {
            action->setCheckable(true);
            action->setChecked(true);
        }
        connect(action, &QAction::triggered, this, [this, mode]() {
            if (m_menu->serviceMode() == mode) {
                return;
            }
            m_menu->setServiceMode(mode);
            updateServiceModeLabel();
        });
    };

    addMode(DlgMenu::ServiceMode::TakeAway,
            tr("Take away"),
            QStringLiteral(":/res/icon_takeaway.png"));
    addMode(DlgMenu::ServiceMode::DineIn,
            tr("Dine in"),
            QStringLiteral(":/res/icon_dinein.png"));

    const QPoint pos = ui->btnServiceMode->mapToGlobal(QPoint(0, ui->btnServiceMode->height() + 4));
    menu.setFixedWidth(qMax(ui->btnServiceMode->width(), 188));
    menu.exec(pos);
}

void DlgCart::setupOrderBarSummary()
{
    ui->lblCartSummary->hide();

    auto *summary = new QWidget(ui->widgetOrderBar);
    summary->setObjectName(QStringLiteral("widgetCartSummary"));
    summary->setAttribute(Qt::WA_StyledBackground, false);
    summary->setStyleSheet(QStringLiteral("background: transparent;"));

    auto *summaryLayout = new QHBoxLayout(summary);
    summaryLayout->setContentsMargins(0, 0, 0, 0);
    summaryLayout->setSpacing(10);

    auto *cartIcon = new QLabel(summary);
    cartIcon->setObjectName(QStringLiteral("lblCartIcon"));
    cartIcon->setText(QStringLiteral("🛒"));
    cartIcon->setAlignment(Qt::AlignCenter);
    styleTransparentTextLabel(cartIcon);

    m_lblCartBadge = new QLabel(QStringLiteral("0"), summary);
    m_lblCartBadge->setObjectName(QStringLiteral("lblCartBadge"));
    m_lblCartBadge->setAlignment(Qt::AlignCenter);

    m_lblCartAmount = new QLabel(tr("0 ֏"), summary);
    m_lblCartAmount->setObjectName(QStringLiteral("lblCartAmount"));
    styleTransparentTextLabel(m_lblCartAmount);

    summaryLayout->addStretch();
    summaryLayout->addWidget(cartIcon);
    summaryLayout->addWidget(m_lblCartBadge);
    summaryLayout->addWidget(m_lblCartAmount);
    summaryLayout->addStretch();

    const int summaryIndex = ui->orderBarLayout->indexOf(ui->lblCartSummary);
    if (summaryIndex >= 0) {
        ui->orderBarLayout->insertWidget(summaryIndex, summary, 1);
    } else {
        ui->orderBarLayout->insertWidget(1, summary, 1);
    }

    ui->orderBarLayout->setStretch(0, 0);
    ui->orderBarLayout->setStretch(1, 1);
    ui->orderBarLayout->setStretch(2, 0);
}

void DlgCart::setupTouchScroll()
{
    ui->scrollCart->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrollCart->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrollCart->setStyleSheet(QStringLiteral(
        "QScrollArea#scrollCart { background: #f3f3f3; border: none; }"
        "QScrollArea#scrollCart QScrollBar { width: 0px; height: 0px; }"));
    ui->scrollCart->viewport()->setStyleSheet(QStringLiteral("background: #f3f3f3;"));
    ui->widgetCartHost->setAttribute(Qt::WA_StyledBackground, true);
    ui->widgetCartHost->setStyleSheet(QStringLiteral("background: #f3f3f3;"));
    ui->cartLinesLayout->setContentsMargins(0, 8, 0, 24);
    QScroller::grabGesture(ui->scrollCart->viewport(), QScroller::TouchGesture);
}

void DlgCart::clearLayout(QLayout *layout)
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

QString DlgCart::lineSubtitle(const MenuDish &dish) const
{
    if (dish.packageId > 0) {
        const QString variant = packageVariantText(dish);
        if (!variant.isEmpty()) {
            return variant;
        }
    }
    return dish.groupName;
}

QString DlgCart::modifiersText(const MenuDish &dish) const
{
    QStringList lines;
    for (const MenuSelectedModificator &modificator : dish.selectedModificators) {
        lines << tr("%1 added").arg(modificator.name);
    }
    if (lines.isEmpty()) {
        return {};
    }
    return QStringLiteral("• ") + lines.join(QStringLiteral("\n• "));
}

void DlgCart::rebuildCartList()
{
    clearLayout(ui->cartLinesLayout);

    const QVector<CartLine> lines = m_cart->lines();
    if (lines.isEmpty()) {
        auto *empty = new QLabel(tr("Your cart is empty"), ui->widgetCartHost);
        empty->setObjectName(QStringLiteral("cartEmptyLabel"));
        empty->setAlignment(Qt::AlignCenter);
        styleTransparentTextLabel(empty);
        ui->cartLinesLayout->addWidget(empty);
        ui->cartLinesLayout->addStretch();
        return;
    }

    for (const CartLine &line : lines) {
        ui->cartLinesLayout->addWidget(makeCartLineWidget(line,
                                                         lineSubtitle(line.dish),
                                                         modifiersText(line.dish),
                                                         ui->widgetCartHost,
                                                         this));
    }
    ui->cartLinesLayout->addStretch();
}

void DlgCart::updateSummary()
{
    if (m_lblCartBadge) {
        m_lblCartBadge->setText(QString::number(m_cart->itemCount()));
    }
    if (m_lblCartAmount) {
        m_lblCartAmount->setText(tr("%1 ֏").arg(QString::number(m_cart->totalAmount(), 'f', 0)));
    }
    ui->btnGoToPay->setEnabled(!m_cart->isEmpty());
}

void DlgCart::onQuantityChanged(const QString &lineKey, int quantity)
{
    m_cart->setQuantity(lineKey, quantity);
    rebuildCartList();
    updateSummary();
}

void DlgCart::on_btnOrderMore_clicked()
{
    emit finished(0);
}

void DlgCart::on_btnGoToPay_clicked()
{
    if (m_cart->isEmpty()) {
        return;
    }
    emit finished(ResultGoToPay);
}
