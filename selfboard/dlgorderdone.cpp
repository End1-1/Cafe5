#include "dlgorderdone.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPaintEvent>
#include <QPainter>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

DlgOrderDone::DlgOrderDone(const QString &orderNumber, QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose, false);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(48, 48, 48, 48);
    root->setSpacing(24);
    root->setAlignment(Qt::AlignCenter);

    m_lblTitle = new QLabel(tr("ORDER ACCEPTED"), this);
    m_lblTitle->setObjectName(QStringLiteral("orderDoneTitle"));
    m_lblTitle->setAlignment(Qt::AlignCenter);
    root->addWidget(m_lblTitle);

    m_lblOrderNumber = new QLabel(orderNumber, this);
    m_lblOrderNumber->setObjectName(QStringLiteral("orderDoneNumber"));
    m_lblOrderNumber->setAlignment(Qt::AlignCenter);
    root->addWidget(m_lblOrderNumber);

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

    QTimer::singleShot(12000, this, &DlgOrderDone::onAcknowledge);
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
