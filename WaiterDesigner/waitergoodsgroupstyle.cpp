#include "waitergoodsgroupstyle.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QRegularExpression>
#include <QTextStream>

namespace {

constexpr char kConfigMarker[] = "waiter-goodsgroup-config:";

QString colorToCss(const QColor &c)
{
    return QStringLiteral("#%1%2%3")
        .arg(c.red(), 2, 16, QChar('0'))
        .arg(c.green(), 2, 16, QChar('0'))
        .arg(c.blue(), 2, 16, QChar('0'));
}

QColor colorFromCss(const QString &value)
{
    QColor c(value.trimmed());
    return c.isValid() ? c : QColor(QStringLiteral("#85a4b8"));
}

QString borderRule(int borderWidth)
{
    if(borderWidth <= 0) {
        return QStringLiteral("border: none;");
    }
    return QStringLiteral("border: %1px solid %2;")
        .arg(borderWidth)
        .arg(QLatin1String(WaiterGoodsGroupStyle::borderColorCss));
}

QString fontWeightCss(bool bold)
{
    return bold ? QStringLiteral("bold") : QStringLiteral("normal");
}

QString buildConfigComment(const WaiterGoodsGroupStyle &style)
{
    return QStringLiteral("/* %1 spacing=%2 border=%3 bg=%4 rows=%5 font=%6 bold=%7 color=%8 */")
        .arg(QLatin1String(kConfigMarker),
             QString::number(style.spacing),
             QString::number(style.borderWidth),
             colorToCss(style.colorDefault),
             QString::number(style.visibleRows),
             QString::number(style.fontSize),
             style.fontBold ? QStringLiteral("1") : QStringLiteral("0"),
             colorToCss(style.fontColor));
}

bool parseConfigComment(const QString &line, WaiterGoodsGroupStyle *style)
{
    if(!line.contains(QLatin1String(kConfigMarker))) {
        return false;
    }

    static const QRegularExpression re(
        QStringLiteral("spacing\\s*=\\s*(\\d+).*"
                       "border\\s*=\\s*(\\d+).*"
                       "bg\\s*=\\s*(#[0-9a-fA-F]{6}).*"
                       "rows\\s*=\\s*(\\d+)"
                       "(?:.*font\\s*=\\s*(\\d+))?"
                       "(?:.*bold\\s*=\\s*([01]))?"
                       "(?:.*color\\s*=\\s*(#[0-9a-fA-F]{6}))?"));

    const QRegularExpressionMatch m = re.match(line);
    if(!m.hasMatch()) {
        return false;
    }

    style->spacing = qBound(0, m.captured(1).toInt(), 32);
    style->borderWidth = qBound(0, m.captured(2).toInt(), 12);
    style->colorDefault = colorFromCss(m.captured(3));
    style->visibleRows = qBound(1, m.captured(4).toInt(), 12);
    if(!m.captured(5).isEmpty()) {
        style->fontSize = qBound(7, m.captured(5).toInt(), 48);
    }
    if(!m.captured(6).isEmpty()) {
        style->fontBold = m.captured(6) == QLatin1String("1");
    }
    if(!m.captured(7).isEmpty()) {
        style->fontColor = colorFromCss(m.captured(7));
    }
    return true;
}

QString patchFrameBlock(const QString &selector, const QString &background, int borderWidth)
{
    return QStringLiteral("%1 {\n"
                          "    background: %2;\n"
                          "    %3\n"
                          "}")
        .arg(selector, background, borderRule(borderWidth));
}

QString patchLabelBlock(int fontSize, bool fontBold, const QColor &fontColor)
{
    return QStringLiteral("QLabel#goodsGroupName {\n"
                          "    background: transparent;\n"
                          "    font-size: %1pt;\n"
                          "    font-weight: %2;\n"
                          "    color: %3;\n"
                          "}")
        .arg(fontSize)
        .arg(fontWeightCss(fontBold))
        .arg(colorToCss(fontColor));
}

QString patchCssContent(const QString &content, const WaiterGoodsGroupStyle &style)
{
    QString result = content;
    const QString configLine = buildConfigComment(style);
    const QRegularExpression configRe(
        QStringLiteral("/\\*\\s*waiter-goodsgroup-config:[^*]*\\*/"));

    if(configRe.match(result).hasMatch()) {
        result.replace(configRe, configLine);
    } else {
        const int anchor = result.indexOf(QStringLiteral("QFrame#goodsGroupFrame"));
        if(anchor >= 0) {
            result.insert(anchor, configLine + QStringLiteral("\n\n"));
        } else {
            result.append(QStringLiteral("\n") + configLine + QStringLiteral("\n"));
        }
    }

    const QString bg = colorToCss(style.colorDefault);
    const QString hoverBg = colorToCss(style.colorDefault.darker(115));
    const QString pressedBg = colorToCss(style.colorDefault.darker(115));

    const auto patchSelector = [&](const QString &selectorPattern, const QString &selectorLiteral, const QString &background) {
        const QString block = patchFrameBlock(selectorLiteral, background, style.borderWidth);
        const QRegularExpression re(
            selectorPattern + QStringLiteral("\\s*\\{[^}]*\\}"),
            QRegularExpression::DotMatchesEverythingOption);
        if(re.match(result).hasMatch()) {
            result.replace(re, block);
        } else {
            result.append(QStringLiteral("\n") + block + QStringLiteral("\n"));
        }
    };

    patchSelector(QStringLiteral("QFrame#goodsGroupFrame(?![:\\[])"),
                  QStringLiteral("QFrame#goodsGroupFrame"),
                  bg);
    patchSelector(QStringLiteral("QFrame#goodsGroupFrame:hover"),
                  QStringLiteral("QFrame#goodsGroupFrame:hover"),
                  hoverBg);
    patchSelector(QStringLiteral("QFrame#goodsGroupFrame\\[pressed=\"true\"\\]"),
                  QStringLiteral("QFrame#goodsGroupFrame[pressed=\"true\"]"),
                  pressedBg);

    const QString labelBlock = patchLabelBlock(style.fontSize, style.fontBold, style.fontColor);
    const QRegularExpression labelRe(
        QStringLiteral("QLabel#goodsGroupName\\s*\\{[^}]*\\}"),
        QRegularExpression::DotMatchesEverythingOption);
    if(labelRe.match(result).hasMatch()) {
        result.replace(labelRe, labelBlock);
    } else {
        result.append(QStringLiteral("\n") + labelBlock + QStringLiteral("\n"));
    }

    return result;
}

bool parseCssContent(const QString &content, WaiterGoodsGroupStyle *style)
{
    const QStringList lines = content.split(QLatin1Char('\n'));
    for(const QString &line : lines) {
        if(parseConfigComment(line, style)) {
            return true;
        }
    }

    static const QRegularExpression frameRe(
        QStringLiteral("QFrame#goodsGroupFrame\\s*\\{[^}]*background:\\s*(#[0-9a-fA-F]{6})"),
        QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpressionMatch m = frameRe.match(content);
    if(m.hasMatch()) {
        style->colorDefault = colorFromCss(m.captured(1));
    }

    static const QRegularExpression borderRe(
        QStringLiteral("QFrame#goodsGroupFrame\\s*\\{[^}]*border:\\s*(\\d+)px"),
        QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpressionMatch bm = borderRe.match(content);
    if(bm.hasMatch()) {
        style->borderWidth = qBound(0, bm.captured(1).toInt(), 12);
    }

    static const QRegularExpression fontRe(
        QStringLiteral("QLabel#goodsGroupName\\s*\\{[^}]*font-size:\\s*(\\d+)(?:pt|px)"),
        QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpressionMatch fm = fontRe.match(content);
    if(fm.hasMatch()) {
        style->fontSize = qBound(7, fm.captured(1).toInt(), 48);
    }

    static const QRegularExpression boldRe(
        QStringLiteral("QLabel#goodsGroupName\\s*\\{[^}]*font-weight:\\s*(bold|normal|\\d+)"),
        QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpressionMatch wm = boldRe.match(content);
    if(wm.hasMatch()) {
        const QString w = wm.captured(1).toLower();
        if(w == QLatin1String("bold")) {
            style->fontBold = true;
        } else if(w == QLatin1String("normal")) {
            style->fontBold = false;
        } else {
            style->fontBold = w.toInt() >= 600;
        }
    }

    static const QRegularExpression colorRe(
        QStringLiteral("QLabel#goodsGroupName\\s*\\{[^}]*color:\\s*(#[0-9a-fA-F]{6}|black|white)"),
        QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpressionMatch cm = colorRe.match(content);
    if(cm.hasMatch()) {
        const QString c = cm.captured(1).toLower();
        if(c == QLatin1String("black")) {
            style->fontColor = QColor(Qt::black);
        } else if(c == QLatin1String("white")) {
            style->fontColor = QColor(Qt::white);
        } else {
            style->fontColor = colorFromCss(c);
        }
    }

    return true;
}

} // namespace

static WaiterGoodsGroupStyle gCachedGroupStyle;

QString WaiterGoodsGroupStyle::cssFilePath()
{
    return QDir::cleanPath(QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("waiter.css")));
}

void WaiterGoodsGroupStyle::updateCacheFromCss(const QString &css)
{
    gCachedGroupStyle = WaiterGoodsGroupStyle();
    parseCssContent(css, &gCachedGroupStyle);
}

bool WaiterGoodsGroupStyle::loadAtStartup(QString *error)
{
    QFile file(cssFilePath());
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if(error) {
            *error = QStringLiteral("Cannot read %1").arg(cssFilePath());
        }
        return false;
    }

    const QString css = QTextStream(&file).readAll();
    file.close();
    updateCacheFromCss(css);
    return true;
}

const WaiterGoodsGroupStyle &WaiterGoodsGroupStyle::cachedStyle()
{
    return gCachedGroupStyle;
}

int WaiterGoodsGroupStyle::stripMaxHeight() const
{
    const int rows = qMax(1, visibleRows);
    return rows * buttonHeight + (rows - 1) * qMax(0, spacing);
}

QString WaiterGoodsGroupStyle::groupStylesheet() const
{
    const QString bg = colorDefault.name(QColor::HexRgb);
    const QString hover = colorDefault.darker(115).name(QColor::HexRgb);
    const QString border = borderRule(borderWidth);
    return QStringLiteral(
               "QFrame#goodsGroupFrame { background: %1; %2 }"
               "QFrame#goodsGroupFrame:hover { background: %3; %2 }"
               "QFrame#goodsGroupFrame[pressed=\"true\"] { background: %3; %2 }"
               "QLabel#goodsGroupName { background: transparent; font-size: %4pt; font-weight: %5; color: %6; }")
        .arg(bg, border, hover)
        .arg(fontSize)
        .arg(fontWeightCss(fontBold))
        .arg(colorToCss(fontColor));
}

bool WaiterGoodsGroupStyle::loadFromCssFile(const QString &path, QString *error)
{
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if(error) {
            *error = QStringLiteral("Cannot read %1").arg(path);
        }
        return false;
    }

    const QString content = QTextStream(&file).readAll();
    file.close();
    return parseCssContent(content, this);
}

bool WaiterGoodsGroupStyle::saveToCssFile(const QString &path, QString *error) const
{
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if(error) {
            *error = QStringLiteral("Cannot read %1").arg(path);
        }
        return false;
    }

    const QString content = QTextStream(&file).readAll();
    file.close();

    const QString patched = patchCssContent(content, *this);

    if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        if(error) {
            *error = QStringLiteral("Cannot write %1").arg(path);
        }
        return false;
    }

    QTextStream out(&file);
    out << patched;
    return true;
}
