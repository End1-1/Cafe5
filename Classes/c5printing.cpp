#include "c5printing.h"
#include <QPainter>
#include <QGraphicsItem>
#include <QDebug>
#include <QTextDocument>
#include <QTextBlock>
#include <QAbstractTextDocumentLayout>

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
    setLineHeight();
}

void C5Printing::setFontBold(bool bold)
{
    fFont.setBold(bold);
    setLineHeight();
}

void C5Printing::setFontItalic(bool italic)
{
    fFont.setItalic(italic);
    setLineHeight();
}

void C5Printing::setFontSize(int size)
{
    fFont.setPointSize(size);
    setLineHeight();
}

void C5Printing::line(qreal x1, qreal y1, qreal x2, qreal y2)
{
    fCanvas->addLine(x1 * fScaleFactor, y1 * fScaleFactor, x2 * fScaleFactor, y2 * fScaleFactor, fLinePen);
}

void C5Printing::ltext(const QString &text, qreal x)
{
    QGraphicsTextItem *item = fCanvas->addText(text, fFont);
    item->moveBy(x * fScaleFactor, fTop * fScaleFactor);
    setTemptop(item);
}

void C5Printing::ctext(const QString &text)
{
    QGraphicsTextItem *item = fCanvas->addText(text, fFont);
    QTextOption op;
    op.setAlignment(Qt::AlignHCenter);
    item->document()->setDefaultTextOption(op);
    item->moveBy(0, fTop * fScaleFactor);
    setTemptop(item);
}

void C5Printing::rtext(const QString text)
{
    QGraphicsTextItem *item = fCanvas->addText(text, fFont);
    QTextOption op;
    op.setAlignment(Qt::AlignRight);
    item->document()->setDefaultTextOption(op);
    item->moveBy(0, fTop * fScaleFactor);
    setTemptop(item);
}

void C5Printing::br(int height)
{
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

void C5Printing::setLineHeight()
{
    QFontMetrics fm(fFont);
    fLineHeight = fm.height() / fScaleFactor;
}

void C5Printing::setTemptop(QGraphicsTextItem *item)
{
    QFontMetrics fm(item->font());
    item->setTextWidth(fNormalWidth * fScaleFactor);
    int blocks = item->document()->documentLayout()->documentSize().height();
    qreal h = (blocks) / fScaleFactor;
    if (h > fLineHeight) {
        fTempTop = h < fTempTop ? fTempTop : h;
    }
}
