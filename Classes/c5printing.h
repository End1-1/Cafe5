#ifndef C5PRINTING_H
#define C5PRINTING_H

#include <QFont>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QPen>
#include <QPrinter>
#include <algorithm> // Для std::max

class C5Printing : public QObject
{
    Q_OBJECT
public:
    C5Printing();
    ~C5Printing();

    void setSceneParams(qreal width, qreal height, qreal logicalDpiX);
    void reset();
    void setFont(const QFont &font);
    void setFontBold(bool bold);
    void setFontItalic(bool italic);
    void setFontStrike(bool strike);
    void setFontSize(int size);
    void setPen(const QPen &p);

    void line(qreal x1, qreal y1, qreal x2, qreal y2, int lineWidth = -1);
    void line(int lineWidth = -1);
    void ltext(const QString &text, qreal x = 0, qreal textWidth = -1);
    void lrtext(const QString &leftText, const QString &rightText, qreal textWidth = -1);
    void ctext(const QString &text);
    void rtext(const QString text);

    void image(const QString &fileName, Qt::Alignment align);
    void image(const QPixmap &image, Qt::Alignment align);

    bool br(qreal height = 0);
    int currentPageIndex();
    bool print(QPrinter &prn);
    QJsonArray jsonData();

    QGraphicsScene *fCanvas;
    int fTop;
    int fLineHeight;
    int fNormalWidth;
    QString fErrorString;

private:
    qreal fLogicalDpiX;
    qreal fMM;
    int fTempTop;
    QPen fLinePen;
    QFont fFont;
    int fCurrentPageIndex;
    QJsonArray fJsonData;

    void setLineHeight();
    void setTemptop(QGraphicsTextItem *item, int textwidth);
};

#endif // C5PRINTING_H
