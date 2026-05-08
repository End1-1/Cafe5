#include "c5printing.h"
#include <QBuffer>
#include <QFontMetrics>
#include <QPrinter>

C5Printing::C5Printing()
    : QObject()
{
    fLogicalDpiX = 96;
    fNormalWidth = 500;
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
    fJsonData = QJsonArray();
    setSceneParams(fNormalWidth, 20000, fLogicalDpiX);

    fFont = QFont("Arial", 10);
    fLineHeight = QFontMetrics(fFont).height();
}

void C5Printing::setSceneParams(qreal width, qreal height, qreal logicalDpiX)
{
    // Устанавливаем 203 или 300 для плотности, близкой к физической головке принтера
    fLogicalDpiX = 203;
    fMM = fLogicalDpiX / 25.4;
    // Ширина теперь должна соответствовать новому DPI
    // Если раньше было 500 при 96dpi, то для 203dpi это ~1050 пикселей
    fNormalWidth = qRound(width * (fLogicalDpiX / 96.0));

    if (fPainter.isActive())
        fPainter.end();

    // Оставляем Format_RGB32, но холст стал в 2-3 раза плотнее
    fImage = QImage(fNormalWidth, qRound(height * (fLogicalDpiX / 96.0)), QImage::Format_RGB32);
    fImage.fill(Qt::white);

    fPainter.begin(&fImage);

    // ВКЛЮЧАЕМ сглаживание, иначе на мелком шрифте будут "лесенки"
    fPainter.setRenderHint(QPainter::Antialiasing, true);
    fPainter.setRenderHint(QPainter::TextAntialiasing, true);
    fPainter.setRenderHint(QPainter::SmoothPixmapTransform, true);
}

void C5Printing::addToJson(const QString &type, const QVariantMap &params)
{
    QJsonObject obj;
    obj["cmd"] = type;

    // Делаем плоский JSON как в Dart: {'cmd': '...', 'param1': 'val'}
    QMapIterator<QString, QVariant> i(params);
    while (i.hasNext()) {
        i.next();
        obj[i.key()] = QJsonValue::fromVariant(i.value());
    }
    fJsonData.append(obj);
}

void C5Printing::setFont(const QFont &font)
{
    fFont = font;
    fLineHeight = QFontMetrics(fFont).height();
}

void C5Printing::setFontSize(int size)
{
    fFont.setPointSize(size);
    fLineHeight = QFontMetrics(fFont).height();
    addToJson("fontsize", {{"size", size}});
}

void C5Printing::setFontBold(bool bold)
{
    fFont.setBold(bold);
    // В Dart нет отдельной команды bold, обычно это часть стилей,
    // но добавим для синхронизации, если нужно
    addToJson("fontbold", {{"bold", bold}});
}

void C5Printing::ltext(const QString &text, qreal x, qreal textWidth)
{
    fPainter.setFont(fFont);
    int posX = qRound(x * fMM);

    // Считаем ширину в пикселях
    int width = (textWidth > 0) ? qRound(textWidth * fMM) : (fNormalWidth - posX);

    // Отрисовка с переносом
    // Используем высоту 10000, чтобы тексту было куда расти вниз
    QRect rect(posX, fTop, width, 10000);
    QRect bound = fPainter.boundingRect(rect, Qt::AlignLeft | Qt::TextWordWrap, text);
    fPainter.drawText(rect, Qt::AlignLeft | Qt::TextWordWrap, text);

    // Запоминаем высоту самой высокой колонки в строке
    fTempTop = qMax(fTempTop, bound.height());

    // --- JSON ЧАСТЬ (синхронизируем с Dart классом PosPrint) ---
    QVariantMap p;
    p["text"] = text;
    p["x"] = (int) x;
    p["textwidth"] = (int) textWidth;
    // В Dart классе нет поля 'align' и 'bold' в ltext,
    // там просто 'cmd': 'ltext'. Оставляем только нужное:
    addToJson("ltext", p);
}

void C5Printing::ctext(const QString &text)
{
    fPainter.setFont(fFont);
    QRect rect(0, fTop, fNormalWidth, 10000);
    QRect bound = fPainter.boundingRect(rect, Qt::AlignHCenter | Qt::AlignTop | Qt::TextWordWrap, text);
    fPainter.drawText(rect, Qt::AlignHCenter | Qt::AlignTop | Qt::TextWordWrap, text);

    fTempTop = qMax(fTempTop, bound.height());
    addToJson("ctext", {{"text", text}});
}

void C5Printing::rtext(const QString text)
{
    fPainter.setFont(fFont);
    int w = QFontMetrics(fFont).horizontalAdvance(text);
    int posX = qMax(0, fNormalWidth - w - 5);
    fPainter.drawText(posX, fTop + QFontMetrics(fFont).ascent(), text);

    fTempTop = qMax(fTempTop, fLineHeight);
    addToJson("rtext", {{"text", text}});
}

void C5Printing::lrtext(const QString &leftText, const QString &rightText, qreal textWidth)
{
    // Рисуем визуально
    ltext(leftText, 0, textWidth);
    rtext(rightText);

    // Удаляем два последних входа JSON (ltext и rtext), заменяем на один lrtext
    if (fJsonData.size() >= 2) {
        fJsonData.removeAt(fJsonData.size() - 1);
        fJsonData.removeAt(fJsonData.size() - 1);
    }
    addToJson("lrtext", {{"left", leftText}, {"right", rightText}});
}

void C5Printing::line(int lineWidth)
{
    fLinePen.setWidth(lineWidth);
    fPainter.setPen(fLinePen);
    fPainter.drawLine(0, fTop, fNormalWidth, fTop);

    if (lineWidth > 1) {
        addToJson("line2", {{"width", lineWidth}});
    } else {
        addToJson("line", {});
    }
}

bool C5Printing::br(qreal height)
{
    int step = (height <= 0) ? qMax(fLineHeight, fTempTop) : qRound(height * fMM);
    fTop += step;
    fTempTop = 0;

    addToJson("br", {{"height", (int) height}});
    return (fTop > 19000);
}

void C5Printing::image(const QPixmap &img, Qt::Alignment align)
{
    if (img.isNull()) {
        return;
    }

    // 1. ПОДГОТОВКА ИЗОБРАЖЕНИЯ
    // Просто берем локальную копию без изменения цветов
    QPixmap p = img;

    // 2. МАСШТАБИРОВАНИЕ
    // Если картинка шире холста fNormalWidth, пропорционально уменьшаем её
    if (p.width() > fNormalWidth) {
        p = p.scaledToWidth(fNormalWidth, Qt::SmoothTransformation);
    }

    // 3. РАСЧЕТ КООРДИНАТЫ X
    // Центрируем или прижимаем к краю, следя, чтобы X не стал отрицательным
    int posX = 0;
    if (align & Qt::AlignHCenter) {
        posX = (fNormalWidth - p.width()) / 2;
    } else if (align & Qt::AlignRight) {
        posX = fNormalWidth - p.width();
    }

    if (posX < 0) {
        posX = 0;
    }

    // 4. ОТРИСОВКА
    if (fPainter.isActive()) {
        fPainter.drawPixmap(posX, fTop, p);
    }

    // Обновляем временную высоту для корректной работы br()
    fTempTop = qMax(fTempTop, p.height());

    // 5. СОХРАНЕНИЕ В JSON ДЛЯ МОБИЛКИ
    QByteArray ba;
    QBuffer bu(&ba);
    bu.open(QIODevice::WriteOnly);
    p.save(&bu, "PNG");

    addToJson("image", {{"data", QString::fromLatin1(ba.toBase64())}, {"align", (int) align}, {"width", p.width()}, {"height", p.height()}});
}

void C5Printing::image(const QString &fileName, Qt::Alignment align)
{
    QPixmap pix(fileName);
    if (!pix.isNull())
        image(pix, align);
}

QJsonArray C5Printing::jsonData() const
{
    return fJsonData;
}

QImage C5Printing::resultImage() const
{
    return fImage.copy(0, 0, fNormalWidth, fTop + 20);
}

bool C5Printing::print(QPrinter &prn)
{
    if (fPainter.isActive())
        fPainter.end();

    QImage finalImage = resultImage().convertToFormat(QImage::Format_Mono, Qt::ThresholdDither | Qt::AvoidDither);

    prn.setFullPage(true);
    QPainter p(&prn);
    QRect targetPageRect = prn.pageRect(QPrinter::DevicePixel).toRect();
    qreal scale = (qreal) targetPageRect.width() / (qreal) fNormalWidth;

    p.drawImage(QRect(0, 0, targetPageRect.width(), qRound(finalImage.height() * scale)), finalImage);
    p.end();
    return true;
}
