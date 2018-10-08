#include "c5printing.h"
#include <QPainter>
#include <QGraphicsItem>
#include <QDebug>

#define INCH_PER_MM 0.0393

C5Printing::C5Printing(const QString &printer)
{
    fPrinter.setPrinterName(printer);
    fCanvas = new QGraphicsScene();
    fCanvasList.append(fCanvas);
    fScaleFactor = (180 / fPrinter.resolution()) * 3.68;
    fLinePen.setWidth(1);
    fLineHeight = 4;
    fTop = 0;
}

C5Printing::~C5Printing()
{
    foreach (QGraphicsScene *gs, fCanvasList) {
        delete gs;
    }
}

void C5Printing::setSceneParams(qreal width, qreal height, QPrinter::Orientation o)
{
    fNormalHeight = height;
    fNormalWidth = width;
    fCanvas->setSceneRect(0, 0, width * fPrinter.resolution() * INCH_PER_MM, height * fPrinter.resolution() * INCH_PER_MM);
    fCanvasOrientation[fCanvas] = o;
}

void C5Printing::setFont(const QFont &font)
{
    fFont = font;
    QFontMetrics fm(fFont);
    fLineHeight = fm.height() / fScaleFactor;
}

void C5Printing::setFontBold(bool bold)
{
    fFont.setBold(bold);
}

void C5Printing::setFontItalic(bool italic)
{
    fFont.setItalic(italic);
}

void C5Printing::setFontSize(int size)
{
    fFont.setPointSize(size);
}

void C5Printing::line(qreal x1, qreal y1, qreal x2, qreal y2)
{
    fCanvas->addLine(x1 * fScaleFactor, y1 * fScaleFactor, x2 * fScaleFactor, y2 * fScaleFactor, fLinePen);
}

void C5Printing::ltext(const QString &text, qreal x)
{
    QGraphicsItem *item = fCanvas->addText(text, fFont);
    item->moveBy(x * fScaleFactor, fTop * fScaleFactor);
}

void C5Printing::ctext(const QString &text)
{
    QGraphicsItem *item = fCanvas->addText(text, fFont);
    QFontMetrics fm(fFont);
    qreal textWidth = fm.width(text);
    qreal left = ((fNormalWidth / 2) * fScaleFactor);
    left -= textWidth / 2;
    item->moveBy(left, fTop * fScaleFactor);
}

void C5Printing::rtext(const QString text)
{
    QGraphicsItem *item = fCanvas->addText(text, fFont);
    QFontMetrics fm(fFont);
    qreal textWidth = fm.width(text);
    item->moveBy((fNormalWidth * fScaleFactor) - textWidth, fTop * fScaleFactor);
}

void C5Printing::br(int height)
{
    if (height == 0) {
        height = fLineHeight;
    }

    fTop += height;

    if (fTop > fNormalHeight) {
        fTop = 0;
        QPrinter::Orientation o = fCanvasOrientation[fCanvas];
        fCanvas = new QGraphicsScene();
        fCanvasList.append(fCanvas);
        setSceneParams(fNormalWidth, fNormalHeight, o);
    }
}

bool C5Printing::checkBr(int height)
{
    return fTop + height > fNormalHeight;
}

void C5Printing::print()
{
    QPainter painter(&fPrinter);
    for (int i = 0; i < fCanvasList.count(); i++) {
        fPrinter.setOrientation(fCanvasOrientation[fCanvasList.at(i)]);
        if (i > 0) {
            fPrinter.newPage();
        }
        fCanvasList.at(i)->render(&painter);
    }
}
