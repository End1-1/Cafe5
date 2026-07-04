#include "cartflyanimation.h"

#include <QLabel>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QVariantAnimation>
#include <QEasingCurve>

namespace {

constexpr int kFlyStartSize = 72;
constexpr int kFlyEndSize = 28;
constexpr int kDurationMs = 480;

QRect globalRectAround(const QPoint &center, int size)
{
    return QRect(center.x() - size / 2, center.y() - size / 2, size, size);
}

} // namespace

void CartFlyAnimation::run(const QPixmap &pixmap,
                           const QPoint &startGlobal,
                           const QPoint &endGlobal,
                           std::function<void()> onFinished)
{
    if (pixmap.isNull()) {
        if (onFinished) {
            onFinished();
        }
        return;
    }

    auto *fly = new QLabel(nullptr,
                          Qt::FramelessWindowHint | Qt::Tool | Qt::WindowTransparentForInput
                              | Qt::WindowDoesNotAcceptFocus);
    fly->setAttribute(Qt::WA_TranslucentBackground, true);
    fly->setAttribute(Qt::WA_ShowWithoutActivating, true);
    fly->setScaledContents(true);
    fly->setPixmap(pixmap.scaled(kFlyStartSize,
                                 kFlyStartSize,
                                 Qt::KeepAspectRatio,
                                 Qt::SmoothTransformation));
    fly->setGeometry(globalRectAround(startGlobal, kFlyStartSize));
    fly->show();
    fly->raise();

    auto *moveAnim = new QVariantAnimation(fly);
    moveAnim->setDuration(kDurationMs);
    moveAnim->setStartValue(globalRectAround(startGlobal, kFlyStartSize));
    moveAnim->setEndValue(globalRectAround(endGlobal, kFlyEndSize));
    moveAnim->setEasingCurve(QEasingCurve::InOutCubic);
    QObject::connect(moveAnim, &QVariantAnimation::valueChanged, fly, [fly](const QVariant &value) {
        fly->setGeometry(value.toRect());
    });

    auto *fadeAnim = new QPropertyAnimation(fly, "windowOpacity", fly);
    fadeAnim->setDuration(kDurationMs);
    fadeAnim->setKeyValueAt(0.0, 1.0);
    fadeAnim->setKeyValueAt(0.7, 1.0);
    fadeAnim->setKeyValueAt(1.0, 0.0);

    auto *group = new QParallelAnimationGroup(fly);
    group->addAnimation(moveAnim);
    group->addAnimation(fadeAnim);

    QObject::connect(group, &QParallelAnimationGroup::finished, fly, [fly, onFinished]() {
        fly->deleteLater();
        if (onFinished) {
            onFinished();
        }
    });
    group->start(QAbstractAnimation::DeleteWhenStopped);
}
