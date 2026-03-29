#include "c5printing.h"
#include <QBuffer>
#include <QJsonObject>

C5Printing::C5Printing()
    : QObject()
{
    fNormalWidth = 500;
    fLogicalDpiX = 96;
    fMM = fLogicalDpiX / 25.4;
    reset();
}

C5Printing::~C5Printing()
{
    if (fPainter.isActive())
        fPainter.end();
}

void C5Printing::reset()
{
    fTop = 0;
    fTempTop = 0;
    fJsonData = QJsonArray(); // Очищаем JSON
    setSceneParams(fNormalWidth, 20000, fLogicalDpiX);
}

void C5Printing::setFont(const QFont &font)
{
    fFont = font;

    fFont.setStyleStrategy(QFont::NoAntialias);
    fFont.setHintingPreference(QFont::PreferFullHinting);
    fFont.setFixedPitch(true);

    addToJson("font", {{"family", fFont.family()}, {"size", fFont.pointSize()}});
}

void C5Printing::setFontBold(bool bold)
{
    fFont.setBold(bold);
    addToJson("fontbold", {{"bold", fFont.bold()}});
}

void C5Printing::setFontSize(int size)
{
    fFont.setPointSize(size);
    addToJson("fontsize", {{"size", fFont.pointSize()}});
}

void C5Printing::setSceneParams(qreal width, qreal height, qreal logicalDpiX)
{
    fLogicalDpiX = logicalDpiX;
    fMM = fLogicalDpiX / 25.4;
    fNormalWidth = qRound(width);

    if (fPainter.isActive())
        fPainter.end();

    // Используем RGB32 (без прозрачности), он чуть быстрее и стабильнее для текста
    fImage = QImage(fNormalWidth, qRound(height), QImage::Format_RGB32);
    fImage.fill(Qt::white);

    fPainter.begin(&fImage);

    // ВЫКЛЮЧАЕМ все "улучшайзеры", которые делают текст "грязным"
    fPainter.setRenderHint(QPainter::Antialiasing, false);
    fPainter.setRenderHint(QPainter::TextAntialiasing, false);
}

// Вспомогательный метод для упаковки команд в JSON
void C5Printing::addToJson(const QString &type, const QVariantMap &params)
{
    QJsonObject obj;
    obj["type"] = type;
    obj["params"] = QJsonObject::fromVariantMap(params);
    fJsonData.append(obj);
}

QImage C5Printing::resultImage() const
{
    return fImage;
}

void C5Printing::ltext(const QString &text, qreal x, qreal textWidth)
{
    fPainter.setFont(fFont);
    int posX = qRound(x * fMM);
    int width = (textWidth > 0) ? qRound(textWidth * fMM) : (fNormalWidth - posX);

    QRect rect(posX, fTop, width, 1000);
    QRect bound = fPainter.boundingRect(rect, Qt::AlignLeft | Qt::TextWordWrap, text);
    fPainter.drawText(rect, Qt::AlignLeft | Qt::TextWordWrap, text);

    fTempTop = qMax(fTempTop, bound.height());

    // JSON часть
    QVariantMap p;
    p["text"] = text;
    p["x"] = x;
    p["align"] = "left";
    p["bold"] = fFont.bold();
    addToJson("text", p);
}

void C5Printing::ctext(const QString &text)
{
    fPainter.setFont(fFont);
    // Ограничиваем прямоугольник по ширине, но разрешаем ему быть "бесконечным" вниз
    QRect rect(0, fTop, fNormalWidth, 10000);

    // Используем AlignTop, чтобы текст прилип к fTop
    // boundingRect поможет узнать реальную высоту, которую занял текст
    QRect bound = fPainter.boundingRect(rect, Qt::AlignHCenter | Qt::AlignTop | Qt::TextWordWrap, text);
    fPainter.drawText(rect, Qt::AlignHCenter | Qt::AlignTop | Qt::TextWordWrap, text);

    // Сохраняем высоту контента, чтобы потом в br() прибавить её к fTop
    fTempTop = qMax(fTempTop, bound.height());

    QVariantMap p;
    p["text"] = text;
    p["align"] = "center";
    p["bold"] = fFont.bold();
    addToJson("text", p);
}

void C5Printing::rtext(const QString text)
{
    fPainter.setFont(fFont);
    int w = QFontMetrics(fFont).horizontalAdvance(text);
    int posX = qMax(0, fNormalWidth - w - 5);
    fPainter.drawText(posX, fTop + QFontMetrics(fFont).ascent(), text);

    fTempTop = qMax(fTempTop, fLineHeight);

    QVariantMap p;
    p["text"] = text;
    p["align"] = "right";
    p["bold"] = fFont.bold();
    addToJson("text", p);
}

void C5Printing::lrtext(const QString &leftText, const QString &rightText, qreal textWidth)
{
    ltext(leftText, 0, textWidth);
    rtext(rightText);
}

void C5Printing::image(const QPixmap &img, Qt::Alignment align)
{
    if (img.isNull()) {
        return;
    }

    int x = 0;
    int safetyMargin = 5;

    // Считаем позицию X в зависимости от выравнивания
    if (align == Qt::AlignRight) {
        x = fNormalWidth - img.width() - safetyMargin;
    } else if (align == Qt::AlignHCenter) {
        x = (fNormalWidth - img.width()) / 2;
    } else {
        x = safetyMargin; // По умолчанию слева с небольшим отступом
    }

    // Рисуем картинку на нашем холсте
    fPainter.drawPixmap(qMax(0, x), fTop, img);

    // Обновляем временную высоту строки
    fTempTop = qMax(fTempTop, img.height());

    // Добавляем в JSON (конвертируем в Base64, чтобы передать данные)
    QByteArray ba;
    QBuffer bu(&ba);
    bu.open(QIODevice::WriteOnly);
    img.save(&bu, "PNG");

    QVariantMap p;
    p["data"] = QString::fromLatin1(ba.toBase64());
    p["align"] = (int) align;
    p["width"] = img.width();
    p["height"] = img.height();
    addToJson("image", p);
}

void C5Printing::image(const QString &fileName, Qt::Alignment align)
{
    QPixmap pix(fileName);
    if (pix.isNull()) {
        return;
    }
    // Масштабируем, если нужно, как в твоем старом коде
    image(pix.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation), align);
}

void C5Printing::line(int lineWidth)
{
    fLinePen.setWidth(lineWidth);
    fPainter.setPen(fLinePen);
    fPainter.drawLine(0, fTop, fNormalWidth, fTop);

    QVariantMap p;
    p["width"] = lineWidth;
    addToJson("line", p);
}

bool C5Printing::br(qreal height)
{
    int step = (height <= 0) ? qMax(fLineHeight, fTempTop) : qRound(height * fMM);
    fTop += step;
    fTempTop = 0;

    QVariantMap p;
    p["height"] = step;
    addToJson("br", p);

    return (fTop > 19000);
}

bool C5Printing::print(QPrinter &prn)
{
    if (fPainter.isActive())
        fPainter.end();

    // 1. Берем только нарисованную часть чека
    QImage finalImage = fImage.copy(0, 0, fNormalWidth, fTop + 20);

    // 2. Превращаем в жесткий ЧЕРНО-БЕЛЫЙ (1 бит)
    // Это уберет все "газетные" полутона и сделает буквы острыми
    finalImage = finalImage.convertToFormat(QImage::Format_Mono, Qt::ThresholdDither | Qt::AvoidDither);

    prn.setFullPage(true);
    QPainter p(&prn);

    // Сбрасываем все трансформации
    p.setWorldTransform(QTransform());

    QRect targetPageRect = prn.pageRect(QPrinter::DevicePixel).toRect();
    qreal scale = (qreal) targetPageRect.width() / (qreal) fNormalWidth;
    int targetHeight = qRound((fTop + 20) * scale);

    // Рисуем 1-битную картинку. Теперь она будет как на кассовом аппарате - без серости.
    p.drawImage(QRect(0, 0, targetPageRect.width(), targetHeight), finalImage);

    p.end();
    return true;
}

QJsonArray C5Printing::jsonData()
{
    return fJsonData;
}
