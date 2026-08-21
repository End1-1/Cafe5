#include "waitertablestyle.h"
#include "waitergoodsgroupstyle.h"
#include "waitergoodsdishstyle.h"

#include "tablecelldelegate.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QRegularExpression>
#include <QTableWidget>
#include <QTextStream>
#include <QApplication>

namespace {

constexpr char kConfigMarker[] = "waiter-tbltables-config:";

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

QString buildConfigComment(const WaiterTblTablesStyle &style)
{
    return QStringLiteral("/* %1 border=%2 grid=%3 empty=%4 order=%5 precheck=%6 empty_text=%7 order_text=%8 precheck_text=%9 */")
        .arg(QLatin1String(kConfigMarker),
             QString::number(style.borderWidth),
             colorToCss(style.gridColor),
             colorToCss(style.colorEmpty),
             colorToCss(style.colorOrder),
             colorToCss(style.colorPrecheck),
             colorToCss(style.colorEmptyText),
             colorToCss(style.colorOrderText),
             colorToCss(style.colorPrecheckText));
}

bool parseConfigComment(const QString &line, WaiterTblTablesStyle *style)
{
    if (!line.contains(QLatin1String(kConfigMarker))) {
        return false;
    }

    static const QRegularExpression re(
        QStringLiteral("border\\s*=\\s*(\\d+).*"
                       "grid\\s*=\\s*(#[0-9a-fA-F]{6}).*"
                       "empty\\s*=\\s*(#[0-9a-fA-F]{6}).*"
                       "order\\s*=\\s*(#[0-9a-fA-F]{6}).*"
                       "precheck\\s*=\\s*(#[0-9a-fA-F]{6})"
                       "(?:.*empty_text\\s*=\\s*(#[0-9a-fA-F]{6}))?"
                       "(?:.*order_text\\s*=\\s*(#[0-9a-fA-F]{6}))?"
                       "(?:.*precheck_text\\s*=\\s*(#[0-9a-fA-F]{6}))?"));

    const QRegularExpressionMatch m = re.match(line);
    if (!m.hasMatch()) {
        return false;
    }

    style->borderWidth = qBound(0, m.captured(1).toInt(), 12);
    style->gridColor = colorFromCss(m.captured(2));
    style->colorEmpty = colorFromCss(m.captured(3));
    style->colorOrder = colorFromCss(m.captured(4));
    style->colorPrecheck = colorFromCss(m.captured(5));
    if (!m.captured(6).isEmpty()) {
        style->colorEmptyText = colorFromCss(m.captured(6));
    }
    if (!m.captured(7).isEmpty()) {
        style->colorOrderText = colorFromCss(m.captured(7));
    }
    if (!m.captured(8).isEmpty()) {
        style->colorPrecheckText = colorFromCss(m.captured(8));
    }
    return true;
}

QString patchCssContent(const QString &content, const WaiterTblTablesStyle &style)
{
    QString result = content;
    const QString configLine = buildConfigComment(style);
    const QRegularExpression configRe(
        QStringLiteral("/\\*\\s*waiter-tbltables-config:[^*]*\\*/"));

    if (configRe.match(result).hasMatch()) {
        result.replace(configRe, configLine);
    } else {
        const int anchor = result.indexOf(QStringLiteral("QTableWidget#tblTables::item"));
        if (anchor >= 0) {
            const int blockEnd = result.indexOf(QLatin1Char('}'), anchor);
            if (blockEnd >= 0) {
                result.insert(blockEnd + 1, QStringLiteral("\n\n") + configLine);
            } else {
                result.append(QStringLiteral("\n") + configLine + QStringLiteral("\n"));
            }
        } else {
            result.append(QStringLiteral("\n") + configLine + QStringLiteral("\n"));
        }
    }

    const QString gridColor = colorToCss(style.gridColor);
    const QString borderRule = QStringLiteral("border: %1px solid %2;")
                                 .arg(style.borderWidth)
                                 .arg(gridColor);

    result.replace(
        QRegularExpression(
            QStringLiteral("(QTableWidget#tblTables\\s*\\{[^}]*?)gridline-color:\\s*[^;]+;"),
            QRegularExpression::DotMatchesEverythingOption),
        QStringLiteral("\\1gridline-color: %1;").arg(gridColor));

    result.replace(
        QRegularExpression(
            QStringLiteral("(QTableWidget#tblTables\\s*\\{[^}]*?)border:\\s*[^;]+;"),
            QRegularExpression::DotMatchesEverythingOption),
        QStringLiteral("\\1border: %1px solid %2;").arg(style.borderWidth).arg(gridColor));

    result.replace(
        QRegularExpression(
            QStringLiteral("(QTableWidget#tblTables::item\\s*\\{[^}]*?)border:\\s*[^;]+;"),
            QRegularExpression::DotMatchesEverythingOption),
        QStringLiteral("\\1") + borderRule);

    const auto patchFrameState = [&](int state, const QColor &bgColor) {
        const QString frameBlock = QStringLiteral(
                                       "QFrame[t1_state=\"%1\"] {\n"
                                       "    background: %2;\n"
                                       "    border: %3px solid %4;\n"
                                       "}")
                                       .arg(state)
                                       .arg(colorToCss(bgColor))
                                       .arg(style.borderWidth)
                                       .arg(gridColor);
        const QRegularExpression frameRe(
            QStringLiteral("QFrame\\[t1_state=\"%1\"\\]\\s*\\{[^}]*\\}")
                .arg(state),
            QRegularExpression::DotMatchesEverythingOption);
        if (frameRe.match(result).hasMatch()) {
            result.replace(frameRe, frameBlock);
        } else {
            result.append(QStringLiteral("\n") + frameBlock + QStringLiteral("\n"));
        }
    };
    const auto patchLabelState = [&](int state, const QColor &textColor) {
        const QString labelBlock = QStringLiteral(
                                       "QFrame[t1_state=\"%1\"] QLabel {\n"
                                       "    color: %2;\n"
                                       "    background: transparent;\n"
                                       "}")
                                       .arg(state)
                                       .arg(colorToCss(textColor));
        const QRegularExpression labelRe(
            QStringLiteral("QFrame\\[t1_state=\"%1\"\\]\\s*QLabel\\s*\\{[^}]*\\}")
                .arg(state),
            QRegularExpression::DotMatchesEverythingOption);
        if (labelRe.match(result).hasMatch()) {
            result.replace(labelRe, labelBlock);
        } else {
            result.append(QStringLiteral("\n") + labelBlock + QStringLiteral("\n"));
        }
    };
    patchFrameState(1, style.colorEmpty);
    patchFrameState(2, style.colorOrder);
    patchFrameState(3, style.colorPrecheck);

    static const QRegularExpression genericLabelRe(
        QStringLiteral("QFrame\\[t1_state\\]\\s*QLabel\\s*\\{[^}]*\\}"),
        QRegularExpression::DotMatchesEverythingOption);
    result.remove(genericLabelRe);

    patchLabelState(1, style.colorEmptyText);
    patchLabelState(2, style.colorOrderText);
    patchLabelState(3, style.colorPrecheckText);

    return result;
}

bool parseCssContent(const QString &content, WaiterTblTablesStyle *style)
{
    bool parsed = false;
    const QStringList lines = content.split(QLatin1Char('\n'));
    for (const QString &line : lines) {
        if (parseConfigComment(line, style)) {
            parsed = true;
            break;
        }
    }

    if (!parsed) {
        static const QRegularExpression itemBorderRe(
            QStringLiteral("QTableWidget#tblTables::item\\s*\\{[^}]*border:\\s*(\\d+)px\\s+solid\\s+(#[0-9a-fA-F]{6})"),
            QRegularExpression::DotMatchesEverythingOption);
        const QRegularExpressionMatch m = itemBorderRe.match(content);
        if (m.hasMatch()) {
            style->borderWidth = m.captured(1).toInt();
            style->gridColor = colorFromCss(m.captured(2));
        }

        static const QRegularExpression frameRe(
            QStringLiteral("QFrame\\[t1_state=\"([123])\"\\]\\s*\\{[^}]*background:\\s*(#[0-9a-fA-F]{6})"),
            QRegularExpression::DotMatchesEverythingOption);
        QRegularExpressionMatchIterator it = frameRe.globalMatch(content);
        while (it.hasNext()) {
            const QRegularExpressionMatch fm = it.next();
            const int state = fm.captured(1).toInt();
            const QColor color = colorFromCss(fm.captured(2));
            if (state == 1) {
                style->colorEmpty = color;
            } else if (state == 2) {
                style->colorOrder = color;
            } else if (state == 3) {
                style->colorPrecheck = color;
            }
        }

        static const QRegularExpression labelRe(
            QStringLiteral("QFrame\\[t1_state=\"([123])\"\\]\\s*QLabel\\s*\\{[^}]*color:\\s*(#[0-9a-fA-F]{6})"),
            QRegularExpression::DotMatchesEverythingOption);
        QRegularExpressionMatchIterator lit = labelRe.globalMatch(content);
        while (lit.hasNext()) {
            const QRegularExpressionMatch fm = lit.next();
            const int state = fm.captured(1).toInt();
            const QColor color = colorFromCss(fm.captured(2));
            if (state == 1) {
                style->colorEmptyText = color;
            } else if (state == 2) {
                style->colorOrderText = color;
            } else if (state == 3) {
                style->colorPrecheckText = color;
            }
        }
    }

    return true;
}

} // namespace

static WaiterTblTablesStyle gCachedTblStyle;

QString WaiterTblTablesStyle::cssFilePath()
{
    return QDir::cleanPath(QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("waiter.css")));
}

bool WaiterTblTablesStyle::loadAtStartup(QString *error)
{
    QFile file(cssFilePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (error) {
            *error = QStringLiteral("Cannot read %1").arg(cssFilePath());
        }
        return false;
    }

    const QString css = QTextStream(&file).readAll();
    file.close();

    if (QApplication *app = qobject_cast<QApplication *>(QCoreApplication::instance())) {
        app->setStyleSheet(css);
    }

    gCachedTblStyle = WaiterTblTablesStyle();
    parseCssContent(css, &gCachedTblStyle);
    WaiterGoodsGroupStyle::updateCacheFromCss(css);
    WaiterGoodsDishStyle::updateCacheFromCss(css);
    return true;
}

const WaiterTblTablesStyle &WaiterTblTablesStyle::cachedTblStyle()
{
    return gCachedTblStyle;
}

void WaiterTblTablesStyle::applyToTable(QTableWidget *table)
{
    if (table) {
        gCachedTblStyle.applyToTableWidget(table);
    }
}

QString WaiterTblTablesStyle::hallTableStylesheet() const
{
    const QString grid = gridColor.name(QColor::HexRgb);
    const auto frameRule = [&](int state, const QColor &bgColor) {
        return QStringLiteral(
                   "QFrame[t1_state=\"%1\"] { background: %2; border: %3px solid %4; }")
            .arg(state)
            .arg(bgColor.name(QColor::HexRgb))
            .arg(borderWidth)
            .arg(grid);
    };
    const auto labelRule = [&](int state, const QColor &textColor) {
        return QStringLiteral(
                   "QFrame[t1_state=\"%1\"] QLabel { color: %2; background: transparent; }")
            .arg(state)
            .arg(textColor.name(QColor::HexRgb));
    };

    return frameRule(1, colorEmpty) + frameRule(2, colorOrder) + frameRule(3, colorPrecheck)
           + labelRule(1, colorEmptyText) + labelRule(2, colorOrderText)
           + labelRule(3, colorPrecheckText);
}

void WaiterTblTablesStyle::applyToTableWidget(QTableWidget *table) const
{
    if (!table) {
        return;
    }

    auto *delegate = qobject_cast<TableCellDelegate *>(table->itemDelegate());
    if (!delegate) {
        delegate = new TableCellDelegate(*this, table);
        table->setItemDelegate(delegate);
    } else {
        delegate->setStyle(*this);
    }

    const QString grid = gridColor.name(QColor::HexRgb);
    table->setStyleSheet(QStringLiteral(
        "QTableWidget#tblTables { background: %1; gridline-color: %1; border: %2px solid %1; }"
        "QTableWidget#tblTables::item { border: %2px solid %1; padding: 4px; }")
                             .arg(grid)
                             .arg(borderWidth));
    table->viewport()->update();
}

bool WaiterTblTablesStyle::loadFromCssFile(const QString &path, QString *error)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (error) {
            *error = QStringLiteral("Cannot read %1").arg(path);
        }
        return false;
    }

    const QString content = QTextStream(&file).readAll();
    file.close();
    return parseCssContent(content, this);
}

bool WaiterTblTablesStyle::saveToCssFile(const QString &path, QString *error) const
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (error) {
            *error = QStringLiteral("Cannot read %1").arg(path);
        }
        return false;
    }

    const QString content = QTextStream(&file).readAll();
    file.close();

    const QString patched = patchCssContent(content, *this);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        if (error) {
            *error = QStringLiteral("Cannot write %1").arg(path);
        }
        return false;
    }

    QTextStream out(&file);
    out << patched;
    return true;
}
