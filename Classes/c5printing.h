#ifndef C5PRINTING_H
#define C5PRINTING_H

#include <QGraphicsScene>
#include <QPen>
#include <QPrinter>
#include <QMap>

class C5Printing
{
public:
    C5Printing();

    C5Printing(const QString &printer);

    ~C5Printing();

    void setSceneParams(qreal width, qreal height, QPrinter::Orientation o);

    void setFont(const QFont &font);

    void setFontBold(bool bold);

    void setFontItalic(bool italic);

    void setFontSize(int size);

    void line(qreal x1, qreal y1, qreal x2, qreal y2);

    void line();

    void ltext(const QString &text, qreal x);

    void ctext(const QString &text);

    void rtext(const QString text);

    void image(const QString &fileName, Qt::Alignment align);

    void image(const QPixmap &image, Qt::Alignment align);

    bool br(qreal height = 0);

    bool checkBr(int height);

    int currentPageIndex();

    QGraphicsScene *page(int index);

    int pageCount();

    QPrinter::Orientation orientation(int index);

    void print(const QString &printername, QPagedPaintDevice::PageSize pageSize);

    qreal fTop;

    qreal fLineHeight;

    qreal fNormalHeight;

    qreal fNormalWidth;

private:
    qreal fTempTop;

    QGraphicsScene *fCanvas;

    QList<QGraphicsScene*> fCanvasList;

    QPen fLinePen;

    QFont fFont;

    int fCurrentPageIndex;

    QMap<QGraphicsScene*, QPrinter::Orientation> fCanvasOrientation;

    void setLineHeight();

    void setTemptop(QGraphicsTextItem *item);
};

#endif // C5PRINTING_H
