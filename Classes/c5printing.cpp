#include "c5printing.h"
#include <QPainter>
#include <QGraphicsItem>
#include <QDebug>
#include <QTextDocument>
#include <QTextBlock>
#include <QPrinter>
#include <QJsonObject>
#include <QAbstractTextDocumentLayout>

#define INCH_PER_MM 0.0393

C5Printing::C5Printing()
{
    fCanvas = new QGraphicsScene();
    fCanvasList.append(fCanvas);
    fLinePen.setWidth(1);
    fLineHeight = 4;
    fTop = 0;
    fTempTop = 0;
    fCurrentPageIndex = 0;
}

C5Printing::~C5Printing()
{
    foreach (QGraphicsScene *gs, fCanvasList) {
        delete gs;
    }
}

void C5Printing::setSceneParams(qreal width, qreal height, QPrinter::Orientation orientation)
{
    fNormalHeight = height;
    fNormalWidth = width;
    fCanvas->setSceneRect(0, 0, width, height);
    fCanvasOrientation[fCanvas] = orientation;

    QJsonObject o;
    o["cmd"] = "scene";
    o["width"] = width;
    o["height"] = height;
    o["orientation"] = (int) orientation;
    fJsonData.append(o);
}

void C5Printing::setFont(const QFont &font)
{
    fFont = font;
    setLineHeight();

    QJsonObject o;
    o["cmd"] = "font";
    o["family"] = font.family();
    o["size"] = font.pointSize();
    o["bold"] = font.bold();
    fJsonData.append(o);
}

void C5Printing::setFontBold(bool bold)
{
    fFont.setBold(bold);
    setLineHeight();

    QJsonObject o;
    o["cmd"] = "fontbold";
    o["bold"] = bold;
    fJsonData.append(o);
}

void C5Printing::setFontItalic(bool italic)
{
    fFont.setItalic(italic);
    setLineHeight();

    QJsonObject o;
    o["cmd"] = "fontitalic";
    o["italic"] = italic;
    fJsonData.append(o);
}

void C5Printing::setFontSize(int size)
{
    fFont.setPointSize(size);
    setLineHeight();

    QJsonObject o;
    o["cmd"] = "fontsize";
    o["size"] = size;
    fJsonData.append(o);
}

void C5Printing::line(qreal x1, qreal y1, qreal x2, qreal y2)
{
    fCanvas->addLine(x1, y1, x2, y2, fLinePen);

    QJsonObject o;
    o["cmd"] = "line1";
    o["x1"] = x1;
    o["y1"] = y1;
    o["x2"] = x2;
    o["y2"] = y2;
    fJsonData.append(o);
}

void C5Printing::line()
{
    line(0, fTop, fNormalWidth, fTop);

    QJsonObject o;
    o["cmd"] = "line2";
    fJsonData.append(o);
}

void C5Printing::tableText(const QList<qreal> &points, const QStringList &vals, int rowHeight)
{
    if (points.count() < 2) {
        return;
    }
    qreal totalWidth = 0.0;
    foreach (qreal w, points) {
        totalWidth += w;
    }
    line(points.at(0), fTop, totalWidth, fTop);
    line(points.at(0), fTop + rowHeight, totalWidth, fTop + rowHeight);
    totalWidth = 0.0;
    for (int i = 0; i < points.count(); i++) {
        totalWidth += points.at(i);
        line (totalWidth, fTop, totalWidth, fTop + rowHeight);
        if (i < points.count() - 1) {
            ltext(vals.at(i), totalWidth + 1);
        }
    }
}

void C5Printing::ltext(const QString &text, qreal x)
{
    QGraphicsTextItem *item = fCanvas->addText(text, fFont);
    item->moveBy(x, fTop);
    setTemptop(item);

    QJsonObject o;
    o["cmd"] = "ltext";
    o["text"] = text;
    o["x"] = x;
    fJsonData.append(o);
}

void C5Printing::ctext(const QString &text)
{
    QGraphicsTextItem *item = fCanvas->addText(text, fFont);
    QTextOption op;
    op.setAlignment(Qt::AlignHCenter);
    item->document()->setDefaultTextOption(op);
    item->moveBy(0, fTop);
    setTemptop(item);

    QJsonObject o;
    o["cmd"] = "ctext";
    o["text"] = text;
    fJsonData.append(o);
}

void C5Printing::rtext(const QString text)
{
    QGraphicsTextItem *item = fCanvas->addText(text, fFont);
    QTextOption op;
    op.setAlignment(Qt::AlignRight);
    item->document()->setDefaultTextOption(op);
    item->moveBy(0, fTop);
    setTemptop(item);

    QJsonObject o;
    o["cmd"] = "rtext";
    o["text"] = text;
    fJsonData.append(o);
}

void C5Printing::image(const QString &fileName, Qt::Alignment align)
{
    QPixmap p(fileName);
    image(p, align);
}

void C5Printing::image(const QPixmap &image, Qt::Alignment align)
{
    QPixmap p(image);
    //p = p.scaled(image.width(), p.height());
    fTempTop = fTempTop < (p.height()) ? (p.height()) : fTempTop;
    QGraphicsPixmapItem *pi = fCanvas->addPixmap(p);
    QPointF pos = pi->pos();
    switch (align) {
    case Qt::AlignLeft:
        break;
    case Qt::AlignRight:
        pos.setX(fNormalWidth - p.width() - 1);
        break;
    case Qt::AlignHCenter:
        pos.setX((fNormalWidth / 2) - (p.width() / 2));
        break;
    default:
        break;
    }
    pos.setY(fTop);
    pi->setPos(pos);
}

bool C5Printing::br(qreal height)
{
    QJsonObject o;
    o["cmd"] = "br";
    o["height"] = height;
    fJsonData.append(o);

    if (height == 0) {
        height = fLineHeight;
    }

    fTop += height + (fTempTop > 0 ? fTempTop - fLineHeight : 0);
    fTempTop = 0;

    if (fTop > fNormalHeight) {
        fTop = 0;
        QPrinter::Orientation o = fCanvasOrientation[fCanvas];
        fCanvas = new QGraphicsScene();
        fCanvasList.append(fCanvas);
        setSceneParams(fNormalWidth, fNormalHeight, o);
        fCurrentPageIndex++;
        return true;
    }
    return false;
}

bool C5Printing::checkBr(int height)
{
    return fTop + height >= fNormalHeight;
}

int C5Printing::currentPageIndex()
{
    return fCurrentPageIndex;
}

QGraphicsScene *C5Printing::page(int index)
{
    return fCanvasList.at(index);
}

int C5Printing::pageCount()
{
    return fCanvasList.count();
}

QPrinter::Orientation C5Printing::orientation(int index)
{
    return fCanvasOrientation[fCanvasList.at(index)];
}

void C5Printing::print(const QString &printername, QPrinter::PageSize pageSize)
{
    QPrinter printer(QPrinter::PrinterResolution);
    printer.setPrinterName(printername);
    printer.setPageSize(pageSize);
    for (int i = 0; i < fCanvasList.count(); i++) {
        if (i > 0) {
            printer.newPage();
        }
        QPrinter::Orientation o = fCanvasOrientation[fCanvasList.at(i)];
        printer.setOrientation(o);
        QPainter painter(&printer);
        fCanvasList.at(i)->render(&painter);
    }
}

QJsonArray C5Printing::jsonData()
{
    return fJsonData;
}

void C5Printing::setLineHeight()
{
    QFontMetrics fm(fFont);
    fLineHeight = (fm.height());
}

void C5Printing::setTemptop(QGraphicsTextItem *item)
{
    item->setTextWidth(fNormalWidth);
    int blocks = item->document()->documentLayout()->documentSize().height();
    qreal h = blocks;
    if (h > fLineHeight) {
        fTempTop = h < fTempTop ? fTempTop : h;
    }
}
