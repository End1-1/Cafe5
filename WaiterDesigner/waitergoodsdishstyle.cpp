#include "waitergoodsdishstyle.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QRegularExpression>
#include <QTextStream>

namespace {

constexpr char kConfigMarker[] = "waiter-goodsdish-config:";

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
    return c.isValid() ? c : QColor(Qt::white);
}

QString borderRule(int borderWidth)
{
    if(borderWidth <= 0) {
        return QStringLiteral("border: none;");
    }
    return QStringLiteral("border: %1px solid %2;")
        .arg(borderWidth)
        .arg(QLatin1String(WaiterGoodsDishStyle::borderColorCss));
}

QString fontWeightCss(bool bold)
{
    return bold ? QStringLiteral("bold") : QStringLiteral("normal");
}

QString buildConfigComment(const WaiterGoodsDishStyle &style)
{
    return QStringLiteral("/* %1 spacing=%2 border=%3 bg=%4 height=%5 font=%6 bold=%7 color=%8 */")
        .arg(QLatin1String(kConfigMarker),
             QString::number(style.spacing),
             QString::number(style.borderWidth),
             colorToCss(style.colorDefault),
             QString::number(style.height),
             QString::number(style.fontSize),
             style.fontBold ? QStringLiteral("1") : QStringLiteral("0"),
             colorToCss(style.fontColor));
}

bool parseConfigComment(const QString &line, WaiterGoodsDishStyle *style)
{
    if(!line.contains(QLatin1String(kConfigMarker))) {
        return false;
    }

    static const QRegularExpression re(
        QStringLiteral("spacing\\s*=\\s*(\\d+).*"
                       "border\\s*=\\s*(\\d+).*"
                       "bg\\s*=\\s*(#[0-9a-fA-F]{6}).*"
                       "height\\s*=\\s*(\\d+)"
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
    style->height = qBound(40, m.captured(4).toInt(), 200);
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
    return QStringLiteral("QLabel#goodsDishName {\n"
                          "    background: transparent;\n"
                          "    font-size: %1pt;\n"
                          "    font-weight: %2;\n"
                          "    color: %3;\n"
                          "}")
        .arg(fontSize)
        .arg(fontWeightCss(fontBold))
        .arg(colorToCss(fontColor));
}

QString patchCssContent(const QString &content, const WaiterGoodsDishStyle &style)
{
    QString result = content;
    const QString configLine = buildConfigComment(style);
    const QRegularExpression configRe(
        QStringLiteral("/\\*\\s*waiter-goodsdish-config:[^*]*\\*/"));

    if(configRe.match(result).hasMatch()) {
        result.replace(configRe, configLine);
    } else {
        const int groupAnchor = result.indexOf(QStringLiteral("/* waiter-goodsgroup-config:"));
        if(groupAnchor >= 0) {
            const int insertAt = result.indexOf(QLatin1Char('\n'), groupAnchor);
            result.insert(insertAt >= 0 ? insertAt + 1 : result.size(),
                          QStringLiteral("\n") + configLine + QStringLiteral("\n"));
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

    patchSelector(QStringLiteral("QFrame#goodsDishFrame(?![:\\[])"),
                  QStringLiteral("QFrame#goodsDishFrame"),
                  bg);
    patchSelector(QStringLiteral("QFrame#goodsDishFrame:hover"),
                  QStringLiteral("QFrame#goodsDishFrame:hover"),
                  hoverBg);
    patchSelector(QStringLiteral("QFrame#goodsDishFrame\\[pressed=\"true\"\\]"),
                  QStringLiteral("QFrame#goodsDishFrame[pressed=\"true\"]"),
                  pressedBg);

    const QString labelBlock = patchLabelBlock(style.fontSize, style.fontBold, style.fontColor);
    const QRegularExpression labelRe(
        QStringLiteral("QLabel#goodsDishName\\s*\\{[^}]*\\}"),
        QRegularExpression::DotMatchesEverythingOption);
    if(labelRe.match(result).hasMatch()) {
        result.replace(labelRe, labelBlock);
    } else {
        result.append(QStringLiteral("\n") + labelBlock + QStringLiteral("\n"));
    }

    return result;
}

bool parseCssContent(const QString &content, WaiterGoodsDishStyle *style)
{
    const QStringList lines = content.split(QLatin1Char('\n'));
    for(const QString &line : lines) {
        if(parseConfigComment(line, style)) {
            return true;
        }
    }

    static const QRegularExpression frameRe(
        QStringLiteral("QFrame#goodsDishFrame\\s*\\{[^}]*background:\\s*(#[0-9a-fA-F]{6})"),
        QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpressionMatch m = frameRe.match(content);
    if(m.hasMatch()) {
        style->colorDefault = colorFromCss(m.captured(1));
    }

    static const QRegularExpression borderRe(
        QStringLiteral("QFrame#goodsDishFrame\\s*\\{[^}]*border:\\s*(\\d+)px"),
        QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpressionMatch bm = borderRe.match(content);
    if(bm.hasMatch()) {
        style->borderWidth = qBound(0, bm.captured(1).toInt(), 12);
    }

    static const QRegularExpression fontRe(
        QStringLiteral("QLabel#goodsDishName\\s*\\{[^}]*font-size:\\s*(\\d+)(?:pt|px)"),
        QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpressionMatch fm = fontRe.match(content);
    if(fm.hasMatch()) {
        style->fontSize = qBound(7, fm.captured(1).toInt(), 48);
    }

    static const QRegularExpression boldRe(
        QStringLiteral("QLabel#goodsDishName\\s*\\{[^}]*font-weight:\\s*(bold|normal|\\d+)"),
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
        QStringLiteral("QLabel#goodsDishName\\s*\\{[^}]*color:\\s*(#[0-9a-fA-F]{6}|black|white)"),
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

static WaiterGoodsDishStyle gCachedDishStyle;

QString WaiterGoodsDishStyle::cssFilePath()
{
    return QDir::cleanPath(QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("waiter.css")));
}

void WaiterGoodsDishStyle::updateCacheFromCss(const QString &css)
{
    gCachedDishStyle = WaiterGoodsDishStyle();
    parseCssContent(css, &gCachedDishStyle);
}

bool WaiterGoodsDishStyle::loadAtStartup(QString *error)
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

const WaiterGoodsDishStyle &WaiterGoodsDishStyle::cachedStyle()
{
    return gCachedDishStyle;
}

QString WaiterGoodsDishStyle::dishStylesheet() const
{
    const QString bg = colorDefault.name(QColor::HexRgb);
    const QString hover = colorDefault.darker(115).name(QColor::HexRgb);
    const QString border = borderRule(borderWidth);
    return QStringLiteral(
               "QFrame#goodsDishFrame { background: %1; %2 }"
               "QFrame#goodsDishFrame:hover { background: %3; %2 }"
               "QFrame#goodsDishFrame[pressed=\"true\"] { background: %3; %2 }"
               "QLabel#goodsDishName { background: transparent; font-size: %4pt; font-weight: %5; color: %6; }")
        .arg(bg, border, hover)
        .arg(fontSize)
        .arg(fontWeightCss(fontBold))
        .arg(colorToCss(fontColor));
}

bool WaiterGoodsDishStyle::loadFromCssFile(const QString &path, QString *error)
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

bool WaiterGoodsDishStyle::saveToCssFile(const QString &path, QString *error) const
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
