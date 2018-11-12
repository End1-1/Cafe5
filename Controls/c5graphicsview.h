#ifndef C5GRAPHICSVIEW_H
#define C5GRAPHICSVIEW_H

#include <QGraphicsView>

class C5GraphicsView : public QGraphicsView
{
public:
    C5GraphicsView(QWidget *parent = 0);

    void scrollContentsBy(int dx, int dy);
};

#endif // C5GRAPHICSVIEW_H
