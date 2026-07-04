#include "selfboardbottomchrome.h"

#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QParallelAnimationGroup>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QResizeEvent>
#include <QSequentialAnimationGroup>
#include <QToolButton>
#include <QVBoxLayout>

namespace {

constexpr int kQrWidth = 190;
constexpr int kQrHeight = 215;
constexpr int kRightMargin = 20;
constexpr int kOrderBarHeight = 100;
constexpr int kTransparentRowHeight = kQrHeight - kOrderBarHeight;
constexpr int kFooterHeight = 72;
constexpr int kQrReserveRight = kQrWidth + kRightMargin;

} // namespace

SelfboardBottomChrome::SelfboardBottomChrome(QWidget *parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("SelfboardBottomChrome"));
    setAttribute(Qt::WA_StyledBackground, true);
    setFixedHeight(kTransparentRowHeight + kOrderBarHeight + kFooterHeight);
    buildUi();
    positionQrPanel();
}

void SelfboardBottomChrome::buildUi()
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    m_upperHost = new QWidget(this);
    m_upperHost->setObjectName(QStringLiteral("chromeUpperHost"));
    m_upperHost->setFixedHeight(kQrHeight);
    auto *upperLayout = new QVBoxLayout(m_upperHost);
    upperLayout->setContentsMargins(0, 0, 0, 0);
    upperLayout->setSpacing(0);

    auto *row0 = new QWidget(m_upperHost);
    row0->setObjectName(QStringLiteral("chromeRowTransparent"));
    row0->setFixedHeight(kTransparentRowHeight);
    row0->setAttribute(Qt::WA_StyledBackground, true);
    upperLayout->addWidget(row0);

    auto *row1 = new QWidget(m_upperHost);
    row1->setObjectName(QStringLiteral("chromeRowWhite"));
    row1->setFixedHeight(kOrderBarHeight);
    row1->setAttribute(Qt::WA_StyledBackground, true);
    auto *orderLayout = new QHBoxLayout(row1);
    orderLayout->setContentsMargins(24, 16, kQrReserveRight + 16, 16);
    orderLayout->setSpacing(16);

    m_btnCancel = new QPushButton(tr("CANCEL ORDER"), row1);
    m_btnCancel->setObjectName(QStringLiteral("btnCancelOrder"));
    connect(m_btnCancel, &QPushButton::clicked, this, &SelfboardBottomChrome::cancelOrderClicked);

    m_cartSummary = new QWidget(row1);
    m_cartSummary->setObjectName(QStringLiteral("widgetCartSummary"));
    m_cartSummary->setCursor(Qt::PointingHandCursor);
    m_cartSummary->installEventFilter(this);
    auto *cartLayout = new QHBoxLayout(m_cartSummary);
    cartLayout->setContentsMargins(0, 0, 0, 0);
    cartLayout->setSpacing(10);
    auto *cartIcon = new QLabel(m_cartSummary);
    cartIcon->setObjectName(QStringLiteral("lblCartIcon"));
    cartIcon->setText(QStringLiteral("🛒"));
    cartIcon->setAlignment(Qt::AlignCenter);
    m_lblCartBadge = new QLabel(QStringLiteral("0"), m_cartSummary);
    m_lblCartBadge->setObjectName(QStringLiteral("lblCartBadge"));
    m_lblCartBadge->setAlignment(Qt::AlignCenter);
    m_lblCartAmount = new QLabel(tr("0 ֏"), m_cartSummary);
    m_lblCartAmount->setObjectName(QStringLiteral("lblCartAmount"));
    cartLayout->addStretch();
    cartLayout->addWidget(cartIcon);
    cartLayout->addWidget(m_lblCartBadge);
    cartLayout->addWidget(m_lblCartAmount);
    cartLayout->addStretch();

    m_btnGoToCart = new QPushButton(tr("GO TO CART"), row1);
    m_btnGoToCart->setObjectName(QStringLiteral("btnGoToCart"));
    m_btnGoToCart->setMinimumWidth(280);
    connect(m_btnGoToCart, &QPushButton::clicked, this, &SelfboardBottomChrome::goToCartClicked);

    orderLayout->addWidget(m_btnCancel, 0, Qt::AlignLeft | Qt::AlignVCenter);
    orderLayout->addWidget(m_cartSummary, 1);
    orderLayout->addWidget(m_btnGoToCart, 0, Qt::AlignRight | Qt::AlignVCenter);
    upperLayout->addWidget(row1);

    m_qrPanel = new QLabel(m_upperHost);
    m_qrPanel->setObjectName(QStringLiteral("widgetQrPanel"));
    m_qrPanel->setFixedSize(kQrWidth, kQrHeight);
    m_qrPanel->setPixmap(QPixmap(QStringLiteral(":/res/qr_placeholder.png")));
    m_qrPanel->setScaledContents(true);
    m_qrPanel->setAlignment(Qt::AlignCenter);
    m_qrPanel->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    m_qrPanel->raise();

    root->addWidget(m_upperHost);

    auto *brandFooter = new QWidget(this);
    brandFooter->setObjectName(QStringLiteral("widgetBrandFooter"));
    brandFooter->setFixedHeight(kFooterHeight);
    brandFooter->setAttribute(Qt::WA_StyledBackground, true);
    auto *footerLayout = new QHBoxLayout(brandFooter);
    footerLayout->setContentsMargins(32, 8, 32, 8);
    footerLayout->setSpacing(16);

    auto *logo = new QLabel(brandFooter);
    logo->setObjectName(QStringLiteral("lblFooterLogo"));
    logo->setFixedSize(100, 48);
    logo->setScaledContents(true);
    logo->setPixmap(QPixmap(QStringLiteral(":/res/footer_logo_myqr.png")));

    auto *powered = new QLabel(tr("Powered by Keyiosk Technologies"), brandFooter);
    powered->setObjectName(QStringLiteral("lblPowered"));
    powered->setAlignment(Qt::AlignCenter);

    auto *socialHost = new QWidget(brandFooter);
    auto *socialLayout = new QHBoxLayout(socialHost);
    socialLayout->setContentsMargins(0, 0, 0, 0);
    socialLayout->setSpacing(8);
    auto *btnYoutube = new QToolButton(socialHost);
    btnYoutube->setObjectName(QStringLiteral("btnYoutube"));
    btnYoutube->setFixedSize(40, 40);
    btnYoutube->setIcon(QIcon(QStringLiteral(":/res/icon_youtube.png")));
    btnYoutube->setIconSize(QSize(36, 36));
    auto *btnFacebook = new QToolButton(socialHost);
    btnFacebook->setObjectName(QStringLiteral("btnFacebook"));
    btnFacebook->setFixedSize(40, 40);
    btnFacebook->setIcon(QIcon(QStringLiteral(":/res/icon_facebook.png")));
    btnFacebook->setIconSize(QSize(36, 36));
    socialLayout->addWidget(btnYoutube);
    socialLayout->addWidget(btnFacebook);

    footerLayout->addWidget(logo, 0, Qt::AlignVCenter);
    footerLayout->addWidget(powered, 1);
    footerLayout->addWidget(socialHost, 0, Qt::AlignVCenter);
    root->addWidget(brandFooter);
}

void SelfboardBottomChrome::positionQrPanel()
{
    if (!m_upperHost || !m_qrPanel) {
        return;
    }
    const int x = m_upperHost->width() - kRightMargin - kQrWidth;
    m_qrPanel->setGeometry(x, 0, kQrWidth, kQrHeight);
    m_qrPanel->raise();
}

void SelfboardBottomChrome::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    positionQrPanel();
}

QWidget *SelfboardBottomChrome::cartSummaryWidget() const
{
    return m_cartSummary;
}

void SelfboardBottomChrome::setCartCount(int count)
{
    if (m_lblCartBadge) {
        m_lblCartBadge->setText(QString::number(count));
    }
}

void SelfboardBottomChrome::setCartTotal(double total)
{
    if (m_lblCartAmount) {
        m_lblCartAmount->setText(tr("%1 ֏").arg(QString::number(total, 'f', 0)));
    }
}

void SelfboardBottomChrome::setGoToCartEnabled(bool enabled)
{
    if (m_btnGoToCart) {
        m_btnGoToCart->setEnabled(enabled);
    }
}

QPoint SelfboardBottomChrome::cartFlyTargetGlobalPos() const
{
    if (m_lblCartBadge) {
        return m_lblCartBadge->mapToGlobal(m_lblCartBadge->rect().center());
    }
    if (m_cartSummary) {
        return m_cartSummary->mapToGlobal(m_cartSummary->rect().center());
    }
    return mapToGlobal(rect().center());
}

void SelfboardBottomChrome::playCartAddedBump()
{
    if (!m_lblCartBadge) {
        return;
    }

    constexpr int kBase = 24;
    constexpr int kPeak = 34;

    auto *growWidth = new QPropertyAnimation(m_lblCartBadge, "minimumWidth", this);
    growWidth->setDuration(120);
    growWidth->setStartValue(kBase);
    growWidth->setEndValue(kPeak);

    auto *growHeight = new QPropertyAnimation(m_lblCartBadge, "minimumHeight", this);
    growHeight->setDuration(120);
    growHeight->setStartValue(kBase);
    growHeight->setEndValue(kPeak);

    auto *shrinkWidth = new QPropertyAnimation(m_lblCartBadge, "minimumWidth", this);
    shrinkWidth->setDuration(120);
    shrinkWidth->setStartValue(kPeak);
    shrinkWidth->setEndValue(kBase);

    auto *shrinkHeight = new QPropertyAnimation(m_lblCartBadge, "minimumHeight", this);
    shrinkHeight->setDuration(120);
    shrinkHeight->setStartValue(kPeak);
    shrinkHeight->setEndValue(kBase);

    auto *grow = new QParallelAnimationGroup(this);
    grow->addAnimation(growWidth);
    grow->addAnimation(growHeight);

    auto *shrink = new QParallelAnimationGroup(this);
    shrink->addAnimation(shrinkWidth);
    shrink->addAnimation(shrinkHeight);

    auto *sequence = new QSequentialAnimationGroup(this);
    sequence->addAnimation(grow);
    sequence->addAnimation(shrink);
    connect(sequence, &QSequentialAnimationGroup::finished, sequence, &QObject::deleteLater);
    sequence->start();
}

bool SelfboardBottomChrome::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_cartSummary
        && event->type() == QEvent::MouseButtonRelease
        && m_btnGoToCart
        && m_btnGoToCart->isEnabled()) {
        emit cartSummaryClicked();
        return true;
    }
    return QWidget::eventFilter(watched, event);
}
