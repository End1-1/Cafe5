#include "dlgpayment.h"
#include "appsettings.h"
#include "selfboardlanguage.h"
#include "dlgpaymentstub.h"
#include "ui_dlgpayment.h"

#include <QApplication>
#include <QCursor>
#include <QEvent>
#include <QFile>
#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>
#include <QScreen>
#include <QShowEvent>
#include <QTimer>

DlgPayment::DlgPayment(OrderCart *cart, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DlgPayment)
    , m_cart(cart)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground, true);
    setAutoFillBackground(true);
    if (!parent) {
        setFixedSize(1080, 1920);
    }

    setupAppearance();

    ui->lblLogo->setPixmap(QPixmap(QStringLiteral(":/res/main_logo.png")));
    ui->lblFooterLogo->setPixmap(QPixmap(QStringLiteral(":/res/footer_logo_myqr.png")));
    ui->lblCardImage->setPixmap(QPixmap(QStringLiteral(":/res/dish_placeholder.png"))
                                     .scaled(360, 220, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    updateTotal();
}

DlgPayment::~DlgPayment()
{
    delete ui;
}

void DlgPayment::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        updateTotal();
    }
    QWidget::changeEvent(event);
}

void DlgPayment::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(rect(), QColor(0xf3, 0xf3, 0xf3));
}

void DlgPayment::setupAppearance()
{
    QFile styleFile(QStringLiteral(":/res/selfboard.css"));
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setStyleSheet(QString::fromUtf8(styleFile.readAll()));
    }
    SelfboardLanguage::instance().bindPickerButton(ui->btnLanguage);
}

void DlgPayment::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    if (m_paymentStarted) {
        return;
    }
    m_paymentStarted = true;
    QTimer::singleShot(200, this, &DlgPayment::startPayment);
}

void DlgPayment::updateTotal()
{
    ui->lblTotal->setText(tr("Total: %1 AMD").arg(QString::number(m_cart->totalAmount(), 'f', 0)));
}

void DlgPayment::showPaymentError(const QString &message)
{
    ui->lblPaymentStatus->setStyleSheet(QStringLiteral("color: #c0392b; font-size: 20px; font-weight: 600;"));
    ui->lblPaymentStatus->setText(message);
}

void DlgPayment::runPaymentStub()
{
    DlgPaymentStub stub(nullptr);
    stub.setWindowModality(Qt::ApplicationModal);
    stub.setWindowFlag(Qt::Window, true);
    stub.setWindowFlag(Qt::WindowStaysOnTopHint, true);
    stub.adjustSize();
    if (QScreen *screen = QApplication::screenAt(QCursor::pos())) {
        const QRect area = screen->availableGeometry();
        stub.move(area.center() - QPoint(stub.width() / 2, stub.height() / 2));
    }
    if (stub.exec() != QDialog::Accepted) {
        return;
    }

    switch (stub.stubResult()) {
    case PaymentStubResult::Paid:
        emit finished(1);
        return;
    case PaymentStubResult::InsufficientFunds:
        showPaymentError(tr("Insufficient funds. Please try another card."));
        return;
    case PaymentStubResult::OtherError:
        showPaymentError(tr("Payment failed. Please try again or contact staff."));
        return;
    case PaymentStubResult::None:
        break;
    }
}

void DlgPayment::startPayment()
{
    ui->lblPaymentStatus->clear();

    if (AppSettings::paymentTestStub()) {
        runPaymentStub();
        return;
    }

    showPaymentError(tr("Card terminal is not configured. Enable the payment test stub in settings."));
}

void DlgPayment::on_btnBackToCart_clicked()
{
    m_backToCart = true;
    emit finished(0);
}
