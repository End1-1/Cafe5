#include "kitchenfineitemrow.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QtMath>

namespace
{

QString formatKitchenQtyLabel(double qty)
{
    const double rounded = qRound(qty * 1000.0) / 1000.0;

    if(qAbs(rounded - qRound(rounded)) < 1e-6) {
        return QString::number(static_cast<int>(qRound(rounded)));
    }

    QString s = QString::number(rounded, 'f', 3);
    const int dot = s.indexOf(QLatin1Char('.'));

    if(dot >= 0) {
        while(s.endsWith(QLatin1Char('0')) && s.size() > dot + 2) {
            s.chop(1);
        }

        if(s.endsWith(QLatin1Char('.'))) {
            s.chop(1);
        }
    }

    return s;
}

} // namespace

KitchenFineItemRow::KitchenFineItemRow(double qty, const QString &name, const QString &comment, QWidget *parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("kitchenFineItemRow"));
    setCursor(Qt::PointingHandCursor);
    setAttribute(Qt::WA_StyledBackground, true);

    auto *root = new QHBoxLayout(this);
    root->setContentsMargins(10, 8, 10, 8);
    root->setSpacing(10);

    mLbQty = new QLabel(formatKitchenQtyLabel(qty), this);
    mLbQty->setObjectName(QStringLiteral("kitchenFineItemQty"));
    mLbQty->setAlignment(Qt::AlignCenter);
    mLbQty->setMinimumWidth(36);
    mLbQty->setMaximumWidth(36);
    root->addWidget(mLbQty, 0, Qt::AlignTop);

    auto *textCol = new QVBoxLayout();
    textCol->setContentsMargins(0, 0, 0, 0);
    textCol->setSpacing(2);

    mLbName = new QLabel(name.trimmed(), this);
    mLbName->setObjectName(QStringLiteral("kitchenFineItemName"));
    mLbName->setWordWrap(true);
    textCol->addWidget(mLbName);

    mLbComment = new QLabel(comment.trimmed(), this);
    mLbComment->setObjectName(QStringLiteral("kitchenFineItemComment"));
    mLbComment->setWordWrap(true);

    if(comment.trimmed().isEmpty()) {
        mLbComment->hide();
    } else {
        textCol->addWidget(mLbComment);
    }

    root->addLayout(textCol, 1);
}

void KitchenFineItemRow::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);

    if(event->button() == Qt::LeftButton) {
        emit clicked();
    }
}
