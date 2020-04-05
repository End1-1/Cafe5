#ifndef C5PRINTING_H
#define C5PRINTING_H

#include <QGraphicsScene>
#include <QPen>
#include <QPrinter>
#include <QJsonArray>
#include <QMap>

class C5Printing : public QObject
{
    Q_OBJECT
public:
    C5Printing();

    ~C5Printing();

    void setSceneParams(qreal width, qreal height, QPrinter::Orientation orientation);

    void setFont(const QFont &font);

    void setFontBold(bool bold);

    void setFontItalic(bool italic);

    void setFontSize(int size);

    void line(qreal x1, qreal y1, qreal x2, qreal y2, int lineWidth = -1);

    void line(int lineWidth = -1);

    void tableText(const QList<qreal> &points, const QStringList &vals, int rowHeight);

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

    void print(QPainter *p);

    qreal fTop;

    qreal fLineHeight;

    qreal fNormalHeight;

    qreal fNormalWidth;

    QJsonArray jsonData();

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

    QJsonArray fJsonData;
};

#endif // C5PRINTING_H
