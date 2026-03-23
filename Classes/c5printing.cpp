#include "c5printing.h"
#include <QAbstractTextDocumentLayout>
#include <QFontMetrics>
#include <QGraphicsItem>
#include <QJsonObject>
#include <QPainter>
#include <QPrinter>
#include <QTextDocument>
#include <algorithm>

C5Printing::C5Printing()
    : QObject()
{
    fCanvas = new QGraphicsScene();
    fNormalWidth = 500;
    fLogicalDpiX = 96;
    fMM = fLogicalDpiX / 25.4;
    reset();
}

C5Printing::~C5Printing()
{
    delete fCanvas;
}

void C5Printing::setSceneParams(qreal width, qreal height, qreal logicalDpiX)
{
    fLogicalDpiX = logicalDpiX;
    fMM = fLogicalDpiX / 25.4;
    fNormalWidth = qRound(width) - 4;
    fCanvas->setSceneRect(0, 0, (qreal) fNormalWidth, height > 0 ? height : 20000.0);
}

void C5Printing::reset()
{
    fCanvas->clear();
    fLinePen.setWidth(1);
    fTop = 0;
    fTempTop = 0;
    fCurrentPageIndex = 0;
    fJsonData = QJsonArray();
    setLineHeight();
}

void C5Printing::setFont(const QFont &font)
{
    fFont = font;
    fFont.setStyleStrategy(QFont::NoAntialias);
    fFont.setHintingPreference(QFont::PreferFullHinting);
    fFont.setLetterSpacing(QFont::PercentageSpacing, 100);
    fFont.setStyleStrategy(QFont::NoAntialias);
    fFont.setKerning(false);
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
void C5Printing::setFontStrike(bool strike)
{
    fFont.setStrikeOut(strike);
    setLineHeight();
}
void C5Printing::setFontSize(int size)
{
    fFont.setPointSize(size);
    setLineHeight();
}
void C5Printing::setPen(const QPen &p)
{
    fLinePen = p;
}

void C5Printing::line(qreal x1, qreal y1, qreal x2, qreal y2, int lineWidth)
{
    if (lineWidth > 0) {
        fLinePen.setWidth(lineWidth);
    }
    fCanvas->addLine((qreal) qRound(x1), (qreal) qRound(y1), (qreal) qRound(x2), (qreal) qRound(y2), fLinePen);
}

void C5Printing::line(int lineWidth)
{
    line(0.0, (qreal) fTop, (qreal) fNormalWidth, (qreal) fTop, lineWidth);
}

void C5Printing::ltext(const QString &text, qreal x, qreal textWidth)
{
    QGraphicsTextItem *item = fCanvas->addText(text, fFont);
    // x — это отступ в мм от левого края
    int posX = qRound(x * fMM);
    item->setPos((qreal) posX, (qreal) fTop);

    // Передаем textWidth (если > 0, то в пикселях)
    setTemptop(item, textWidth > 0 ? qRound(textWidth * fMM) : -1);
}

void C5Printing::lrtext(const QString &leftText, const QString &rightText, qreal textWidth)
{
    ltext(leftText, 0, textWidth);
    rtext(rightText);
}

void C5Printing::ctext(const QString &text)
{
    QGraphicsTextItem *item = fCanvas->addText(text, fFont);
    item->setTextWidth((qreal) fNormalWidth);
    QTextOption opt(Qt::AlignCenter);
    item->document()->setDefaultTextOption(opt);
    item->document()->setDocumentMargin(0);
    item->setPos(0, (qreal) fTop);

    int itemHeight = qRound(item->document()->size().height());
    fTempTop = std::max<int>(fTempTop, itemHeight);
}

void C5Printing::rtext(const QString text)
{
    int w = QFontMetrics(fFont).horizontalAdvance(text);
    QGraphicsTextItem *item = fCanvas->addText(text, fFont);

    int itemHeight = qRound(item->document()->size().height());
    fTempTop = std::max<int>(fTempTop, itemHeight);
    int safetyMargin = 5;
    int posX = std::max<int>(0, fNormalWidth - w - safetyMargin);
    item->setPos((qreal) posX, (qreal) fTop);
}

void C5Printing::image(const QPixmap &image, Qt::Alignment align)
{
    fTempTop = std::max<int>(fTempTop, image.height());
    QGraphicsPixmapItem *pi = fCanvas->addPixmap(image);
    int x = 0;
    int safetyMargin = 5;
    if (align == Qt::AlignRight) {
        x = fNormalWidth - image.width() - safetyMargin;
    } else if (align == Qt::AlignHCenter) {
        x = (fNormalWidth - image.width()) / 2;
    }
    pi->setPos((qreal) std::max<int>(0, x), (qreal) fTop);
}

void C5Printing::image(const QString &fileName, Qt::Alignment align)
{
    image(QPixmap(fileName).scaled(100, 100, Qt::KeepAspectRatio), align);
}

bool C5Printing::br(qreal height)
{
    if (height <= 0) {
        fTop += std::max<int>(fLineHeight, fTempTop);
    } else {
        fTop += qRound(height * fMM);
    }
    fTempTop = 0;
    return false;
}

int C5Printing::currentPageIndex()
{
    return fCurrentPageIndex;
}

bool C5Printing::print(QPrinter &prn)
{
    prn.setFullPage(true);
    QPainter painter(&prn);

    // Вместо painter.setTransform(painter.transform().scale(1, 1));
    // Просто сбрасываем трансформацию в дефолт (единичную матрицу)
    painter.setWorldTransform(QTransform());

    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setRenderHint(QPainter::TextAntialiasing, false);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter.setRenderHint(QPainter::LosslessImageRendering, true);

    QRect targetRect = prn.pageRect(QPrinter::DevicePixel).toRect();

    // Округляем до целого через (int) или qRound, чтобы не было дробных пикселей в координатах
    QRect sourceRect(0, 0, fNormalWidth, (int) fTop + 10);

    fCanvas->render(&painter, targetRect, sourceRect);
    return prn.printerState() != QPrinter::Error;
}

QJsonArray C5Printing::jsonData()
{
    return QJsonArray();
}

void C5Printing::setLineHeight()
{
    fLineHeight = QFontMetrics(fFont).height();
}

void C5Printing::setTemptop(QGraphicsTextItem *item, int textwidth)
{
    int itemX = qRound(item->x());
    int finalWidth = (textwidth <= 0) ? (fNormalWidth - itemX) : textwidth;

    if (itemX + finalWidth > fNormalWidth) {
        finalWidth = fNormalWidth - itemX;
    }

    item->setTextWidth((qreal) finalWidth);

    QTextDocument *doc = item->document();
    doc->setDocumentMargin(0);

    // ВАЖНО: Применяем настройки шрифта к документу еще раз
    item->setFont(fFont);

    QTextOption opt = doc->defaultTextOption();
    opt.setWrapMode(QTextOption::WordWrap);

    // ЭТО КЛЮЧЕВОЙ МОМЕНТ:
    // UseDesignMetrics заставляет Qt считать позиции символов целыми числами,
    // как они описаны в файле шрифта, без учета "умного" экранного сглаживания.
    opt.setUseDesignMetrics(false);
    doc->setDefaultTextOption(opt);

    // Добавляем небольшой фикс для межстрочного интервала,
    // чтобы вторая строка не "залипала" на дробную координату
    QTextCursor cursor(doc);
    cursor.select(QTextCursor::Document);
    QTextBlockFormat blockFormat;
    // Устанавливаем фиксированную высоту строки, равную высоте шрифта
    blockFormat.setLineHeight(fLineHeight, QTextBlockFormat::FixedHeight);
    cursor.setBlockFormat(blockFormat);

    // Округляем высоту до большего целого
    int itemHeight = qCeil(doc->documentLayout()->documentSize().height());

    fTempTop = std::max<int>(fTempTop, itemHeight);
}
