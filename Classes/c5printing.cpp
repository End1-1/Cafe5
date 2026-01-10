#include "c5printing.h"
#include <QPainter>
#include <QGraphicsItem>
#include <QDebug>
#include <QTextDocument>
#include <QTextBlock>
#include <QPrinterInfo>
#include <QPrinter>
#include <QJsonObject>
#include <QJsonDocument>
#include <QAbstractTextDocumentLayout>
#include <QTcpSocket>

C5Printing::C5Printing() :
    QObject()
{
    fCanvas = new QGraphicsScene();
    fLinePen.setWidth(1);
    fLineHeight = 4;
    fTop = 0;
    fTempTop = 0;
    fCurrentPageIndex = 0;
    fNoNewPage = false;
}

C5Printing::~C5Printing()
{
    delete fCanvas;
}

void C5Printing::setSceneParams(qreal width, qreal height, qreal logicalDpiX)
{
    fLogicalDpiX = logicalDpiX;
    fMM = fLogicalDpiX / 25.4;
    fNormalWidth = width;
    fCanvas->setSceneRect(0, 0, width, height);
    QJsonObject o;
    o["cmd"] = "scene";
    o["width"] = width;
    o["height"] = height;
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

void C5Printing::setFontStrike(bool strike)
{
    fFont.setStrikeOut(strike);
    setLineHeight();
    QJsonObject o;
    o["cmd"] = "fontstrikeout";
    o["strike"] = strike;
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

void C5Printing::setPen(const QPen &p)
{
    fLinePen = p;
}

void C5Printing::line(qreal x1, qreal y1, qreal x2, qreal y2, int lineWidth)
{
    if(lineWidth > 0) {
        fLinePen.setWidth(lineWidth);
    }

    fCanvas->addLine(x1, y1, x2, y2, fLinePen);
    QJsonObject o;
    o["cmd"] = "line1";
    o["x1"] = x1;
    o["y1"] = y1;
    o["x2"] = x2;
    o["y2"] = y2;
    o["width"] = lineWidth;
    fJsonData.append(o);
}

void C5Printing::line(int lineWidth)
{
    line(0, fTop, fNormalWidth, fTop, lineWidth);
    QJsonObject o;
    o["cmd"] = "line2";
    o["width"] = lineWidth;
    fJsonData.append(o);
}

void C5Printing::ltext(const QString &text, qreal x, qreal textWidth)
{
    QGraphicsTextItem *item = fCanvas->addText(text, fFont);
    item->moveBy(x * fMM, fTop);
    setTemptop(item, textWidth * fMM);
    QJsonObject o;
    o["cmd"] = "ltext";
    o["text"] = text;
    o["textwidth"] = textWidth;
    o["x"] = x;
    fJsonData.append(o);
}

void C5Printing::lrtext(const QString &leftText, const QString &rightText, qreal textWidth)
{
    ltext(leftText, 0,  textWidth);
    rtext(rightText);
}

void C5Printing::ctext(const QString &text)
{
    QFontMetrics fm(fFont);
    const int w = fm.horizontalAdvance(text);
    auto *item = fCanvas->addText(text, fFont);
    item->setTextWidth(-1);
    item->moveBy((fNormalWidth - w) / 2, fTop);
    fTempTop = qMax<qreal>(fTempTop, fm.height());
    QJsonObject o;
    o["cmd"] = "ctext";
    o["text"] = text;
    fJsonData.append(o);
}

void C5Printing::rtext(const QString text)
{
    QFontMetrics fm(fFont);
    const int w = fm.horizontalAdvance(text);
    QGraphicsTextItem *item = fCanvas->addText(text, fFont);
    item->setTextWidth(-1);
    item->moveBy(qMax<qreal>(0, fNormalWidth - w), fTop);
    setTemptop(item, -1);
    QJsonObject o;
    o["cmd"] = "rtext";
    o["text"] = text;
    fJsonData.append(o);
}

void C5Printing::image(const QString &fileName, Qt::Alignment align)
{
    QPixmap p(fileName);
    p = p.scaled(100, 100, Qt::KeepAspectRatio);
    image(p, align);
}

void C5Printing::image(const QPixmap &image, Qt::Alignment align)
{
    QPixmap p(image);
    //p = p.scaled(image.width(), p.height());
    fTempTop = fTempTop < (p.height()) ? (p.height()) : fTempTop;
    QGraphicsPixmapItem *pi = fCanvas->addPixmap(p);
    QPointF pos = pi->pos();

    switch(align) {
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

    if(height <= 0)
        height = fLineHeight;

    fTop += qMax(height, fTempTop);
    fTempTop = 0;
    return false;
}

int C5Printing::currentPageIndex()
{
    return fCurrentPageIndex;
}

bool C5Printing::print(QPrinter &prn)
{
    QPainter painter(&prn);
    fCanvas->render(&painter);

    if(prn.printerState() == QPrinter::Error) {
        fErrorString = QString("Not printed");
        return false;
    }

    return true;
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

void C5Printing::setTemptop(QGraphicsTextItem *item, qreal textwidth)
{
    item->setTextWidth(textwidth == -1 ? fNormalWidth : textwidth);
    QTextOption opt;
    opt.setWrapMode(QTextOption::WordWrap);
    item->document()->setDefaultTextOption(opt);
    // ВАЖНО: корректный расчёт высоты документа
    item->document()->adjustSize();
    qreal h = item->document()->size().height();
    fTempTop = qMax(fTempTop, h);
}
