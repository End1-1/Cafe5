#include "dlgorderdone.h"

#include "QRCodeGenerator.h"

#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

namespace {

QPixmap encodeQr(const QString &payload, int pixelSize)
{
    if (payload.isEmpty() || pixelSize <= 0) {
        return {};
    }

    CQR_Encode qrEncode;
    QByteArray bytes = payload.toUtf8();
    if (!qrEncode.EncodeData(QR_LEVEL_M, 0, true, -1, bytes.data())) {
        return {};
    }

    const int modules = qrEncode.m_nSymbleSize;
    const int total = modules + QR_MARGIN * 2;
    QImage image(total, total, QImage::Format_RGB32);
    image.fill(Qt::white);
    for (int y = 0; y < modules; ++y) {
        for (int x = 0; x < modules; ++x) {
            if (qrEncode.m_byModuleData[x][y]) {
                image.setPixel(x + QR_MARGIN, y + QR_MARGIN, qRgb(0, 0, 0));
            }
        }
    }

    return QPixmap::fromImage(image.scaled(pixelSize, pixelSize, Qt::KeepAspectRatio, Qt::FastTransformation));
}

} // namespace

DlgOrderDone::DlgOrderDone(const QString &orderNumber, const QString &statusUrl, QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose, false);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(48, 48, 48, 48);
    root->setSpacing(20);
    root->setAlignment(Qt::AlignCenter);

    m_lblTitle = new QLabel(tr("ORDER ACCEPTED"), this);
    m_lblTitle->setObjectName(QStringLiteral("orderDoneTitle"));
    m_lblTitle->setAlignment(Qt::AlignCenter);
    root->addWidget(m_lblTitle);

    m_lblOrderNumber = new QLabel(orderNumber, this);
    m_lblOrderNumber->setObjectName(QStringLiteral("orderDoneNumber"));
    m_lblOrderNumber->setAlignment(Qt::AlignCenter);
    root->addWidget(m_lblOrderNumber);

    if (!statusUrl.isEmpty()) {
        m_lblQrHint = new QLabel(tr("Scan to track your order"), this);
        m_lblQrHint->setObjectName(QStringLiteral("orderDoneQrHint"));
        m_lblQrHint->setAlignment(Qt::AlignCenter);
        root->addWidget(m_lblQrHint);

        m_lblQr = new QLabel(this);
        m_lblQr->setObjectName(QStringLiteral("orderDoneQr"));
        m_lblQr->setAlignment(Qt::AlignCenter);
        m_lblQr->setFixedSize(360, 360);
        const QPixmap qr = encodeQr(statusUrl, 360);
        if (!qr.isNull()) {
            m_lblQr->setPixmap(qr);
        } else {
            m_lblQr->setText(tr("QR unavailable"));
        }
        auto *qrRow = new QHBoxLayout();
        qrRow->addStretch();
        qrRow->addWidget(m_lblQr);
        qrRow->addStretch();
        root->addLayout(qrRow);
    }

    m_lblHint = new QLabel(tr("Please take your receipt number and wait for your order."), this);
    m_lblHint->setObjectName(QStringLiteral("orderDoneHint"));
    m_lblHint->setAlignment(Qt::AlignCenter);
    m_lblHint->setWordWrap(true);
    root->addWidget(m_lblHint);

    m_btnOk = new QPushButton(tr("OK"), this);
    m_btnOk->setObjectName(QStringLiteral("orderDoneOk"));
    m_btnOk->setMinimumHeight(64);
    m_btnOk->setMinimumWidth(280);
    auto *btnRow = new QHBoxLayout();
    btnRow->addStretch();
    btnRow->addWidget(m_btnOk);
    btnRow->addStretch();
    root->addLayout(btnRow);

    connect(m_btnOk, &QPushButton::clicked, this, &DlgOrderDone::onAcknowledge);

    // Longer timeout when QR is shown so guests can scan.
    QTimer::singleShot(statusUrl.isEmpty() ? 12000 : 45000, this, &DlgOrderDone::onAcknowledge);
}

QPixmap DlgOrderDone::makeQrPixmap(const QString &payload, int pixelSize)
{
    return encodeQr(payload, pixelSize);
}

void DlgOrderDone::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(rect(), QColor(0, 0, 0, 180));
}

void DlgOrderDone::onAcknowledge()
{
    emit acknowledged();
}
