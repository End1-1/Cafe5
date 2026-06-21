#include "dlgcart.h"
#include "selfboardlanguage.h"
#include "ui_dlgcart.h"

#include <QEvent>
#include <QFile>
#include <QFrame>
#include <QLabel>
#include <QLayoutItem>
#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QScroller>
#include <QVBoxLayout>

namespace {

QWidget *makeCartLineWidget(const CartLine &line,
                            const QString &modifiers,
                            QWidget *parent,
                            DlgCart *cartDialog)
{
    auto *card = new QWidget(parent);
    card->setObjectName(QStringLiteral("cartLineCard"));

    auto *layout = new QHBoxLayout(card);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(16);

    auto *img = new QLabel(card);
    img->setObjectName(QStringLiteral("cartLineImage"));
    img->setFixedSize(120, 120);
    img->setScaledContents(true);
    QPixmap pix(line.dish.imagePath);
    if (pix.isNull()) {
        pix = QPixmap(QStringLiteral(":/res/dish_placeholder.png"));
    }
    img->setPixmap(pix.scaled(120, 120, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));

    auto *infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(6);

    auto *name = new QLabel(line.dish.name, card);
    name->setObjectName(QStringLiteral("cartLineName"));
    name->setWordWrap(true);
    infoLayout->addWidget(name);

    if (!modifiers.isEmpty()) {
        auto *mods = new QLabel(modifiers, card);
        mods->setObjectName(QStringLiteral("cartLineModifiers"));
        mods->setWordWrap(true);
        infoLayout->addWidget(mods);
    }

    const QString unitPrice = QString::number(line.dish.price, 'f', 0);
    const QString lineTotal = QString::number(line.dish.price * line.quantity, 'f', 0);
    auto *price = new QLabel(
        QObject::tr("%1 AMD").arg(unitPrice)
            + QStringLiteral("\n")
            + QObject::tr("Total: %1 AMD").arg(lineTotal),
        card);
    price->setObjectName(QStringLiteral("cartLinePrice"));
    infoLayout->addWidget(price);
    infoLayout->addStretch();

    auto *rightLayout = new QVBoxLayout();
    rightLayout->setSpacing(12);
    rightLayout->setAlignment(Qt::AlignTop | Qt::AlignRight);

    auto *editLabel = new QLabel(QObject::tr("Edit item"), card);
    editLabel->setObjectName(QStringLiteral("cartLineEditLabel"));
    rightLayout->addWidget(editLabel, 0, Qt::AlignRight);

    auto *qtyWrap = new QWidget(card);
    qtyWrap->setObjectName(QStringLiteral("cartQtyWrap"));
    auto *qtyLayout = new QHBoxLayout(qtyWrap);
    qtyLayout->setContentsMargins(4, 4, 4, 4);
    qtyLayout->setSpacing(0);

    auto *btnMinus = new QPushButton(QStringLiteral("−"), qtyWrap);
    btnMinus->setObjectName(QStringLiteral("cartQtyMinus"));
    btnMinus->setProperty("lineKey", line.key);

    auto *lblQty = new QLabel(QString::number(line.quantity), qtyWrap);
    lblQty->setObjectName(QStringLiteral("cartQtyValue"));
    lblQty->setAlignment(Qt::AlignCenter);
    lblQty->setMinimumWidth(48);

    auto *btnPlus = new QPushButton(QStringLiteral("+"), qtyWrap);
    btnPlus->setObjectName(QStringLiteral("cartQtyPlus"));
    btnPlus->setProperty("lineKey", line.key);

    qtyLayout->addWidget(btnMinus);
    qtyLayout->addWidget(lblQty);
    qtyLayout->addWidget(btnPlus);
    rightLayout->addWidget(qtyWrap);

    layout->addWidget(img);
    layout->addLayout(infoLayout, 1);
    layout->addLayout(rightLayout);

    QObject::connect(btnMinus, &QPushButton::clicked, cartDialog, [cartDialog, line, lblQty]() {
        cartDialog->onQuantityChanged(line.key, lblQty->text().toInt() - 1);
    });
    QObject::connect(btnPlus, &QPushButton::clicked, cartDialog, [cartDialog, line, lblQty]() {
        cartDialog->onQuantityChanged(line.key, lblQty->text().toInt() + 1);
    });

    return card;
}

} // namespace

DlgCart::DlgCart(OrderCart *cart, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DlgCart)
    , m_cart(cart)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground, true);
    setAutoFillBackground(true);
    if (!parent) {
        setFixedSize(1080, 1920);
    }

    setupAppearance();
    setupTouchScroll();

    ui->lblLogo->setPixmap(QPixmap(QStringLiteral(":/res/main_logo.png")));
    ui->lblFooterLogo->setPixmap(QPixmap(QStringLiteral(":/res/footer_logo_myqr.png")));

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
    SelfboardLanguage::instance().bindPickerButton(ui->btnLanguage);
}

void DlgCart::setupTouchScroll()
{
    ui->scrollCart->setStyleSheet(QStringLiteral("QScrollArea { background: #f3f3f3; border: none; }"));
    ui->scrollCart->viewport()->setStyleSheet(QStringLiteral("background: #f3f3f3;"));
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

QString DlgCart::modifiersText(const MenuDish &dish) const
{
    QStringList parts;
    const MenuDynamicAttributes attrs = dish.dynamicAttributes();
    if (!attrs.type.isEmpty()) {
        parts << attrs.type;
    }
    if (!attrs.displaySize().isEmpty()) {
        parts << attrs.displaySize();
    }
    for (const MenuPackageComponent &component : dish.packageComponents) {
        parts << tr("%1 added").arg(component.name);
    }
    for (const MenuSelectedModificator &modificator : dish.selectedModificators) {
        if (modificator.price > 0.0) {
            parts << tr("%1 +%2").arg(modificator.name, QString::number(modificator.price, 'f', 0));
        } else {
            parts << modificator.name;
        }
    }
    return parts.join(QStringLiteral(", "));
}

void DlgCart::rebuildCartList()
{
    clearLayout(ui->cartLinesLayout);

    const QVector<CartLine> lines = m_cart->lines();
    if (lines.isEmpty()) {
        auto *empty = new QLabel(tr("Your cart is empty"), ui->widgetCartHost);
        empty->setObjectName(QStringLiteral("cartEmptyLabel"));
        empty->setAlignment(Qt::AlignCenter);
        ui->cartLinesLayout->addWidget(empty);
        ui->cartLinesLayout->addStretch();
        return;
    }

    for (const CartLine &line : lines) {
        ui->cartLinesLayout->addWidget(makeCartLineWidget(line, modifiersText(line.dish), ui->widgetCartHost, this));
    }
    ui->cartLinesLayout->addStretch();
}

void DlgCart::updateSummary()
{
    ui->lblCartSummary->setText(
        tr("%1   %2 ֏").arg(m_cart->itemCount()).arg(QString::number(m_cart->totalAmount(), 'f', 0)));
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
