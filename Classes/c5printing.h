#ifndef C5PRINTING_H
#define C5PRINTING_H

#include <QPrinter>
#include <QGraphicsScene>
#include <QPen>
#include <QMap>

class C5Printing
{
public:
    C5Printing(const QString &printer);

    ~C5Printing();

    void setSceneParams(qreal width, qreal height, QPrinter::Orientation o);

    void setFont(const QFont &font);

    void setFontBold(bool bold);

    void setFontItalic(bool italic);

    void setFontSize(int size);

    void line(qreal x1, qreal y1, qreal x2, qreal y2);

    void ltext(const QString &text, qreal x);

    void ctext(const QString &text);

    void rtext(const QString text);

    void br(int height = 0);

    bool checkBr(int height);

    void print();

    qreal fTop;

    qreal fLineHeight;

    qreal fNormalHeight;

    qreal fNormalWidth;

private:
    qreal fTempTop;

    QPrinter fPrinter;

    QGraphicsScene *fCanvas;

    QList<QGraphicsScene*> fCanvasList;

    qreal fScaleFactor;

    QPen fLinePen;

    QFont fFont;

    QMap<QGraphicsScene*, QPrinter::Orientation> fCanvasOrientation;

    void setLineHeight();

    void setTemptop(QGraphicsTextItem *item);
};

#endif // C5PRINTING_H
