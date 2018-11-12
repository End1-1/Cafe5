#include "c5graphicsview.h"

C5GraphicsView::C5GraphicsView(QWidget *parent) :
    QGraphicsView(parent)
{

}

void C5GraphicsView::scrollContentsBy(int dx, int dy)
{
    Q_UNUSED(dx);
    Q_UNUSED(dy);
}
