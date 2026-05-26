#pragma once

#include <QWidget>

class QScreen;

namespace SelfBoardDisplay {

constexpr int kPortraitWidth = 1080;
constexpr int kPortraitHeight = 1920;

/** True when the screen is landscape and portrait UI should be rotated for viewing. */
bool shouldRotate(const QScreen *screen = nullptr);

/** Shows @p content fullscreen, rotated 90° CW on landscape monitors when needed. */
void showFullscreen(QWidget *content);

} // namespace SelfBoardDisplay
