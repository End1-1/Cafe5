#pragma once

#include <QPixmap>
#include <QPoint>

#include <functional>

namespace CartFlyAnimation {

void run(const QPixmap &pixmap,
         const QPoint &startGlobal,
         const QPoint &endGlobal,
         std::function<void()> onFinished = {});

} // namespace CartFlyAnimation
