#include "c5lineedit.h"
#include <QKeyEvent>
#include <QTimer>
#include <QValidator>
#include "c5utils.h"

C5LineEdit::C5LineEdit(QWidget *parent) :
    QLineEdit(parent)
{
    fColor = -1;
    fDecimalPlaces = 2;
    connect(this, SIGNAL(textEdited(QString)), this, SLOT(editText(QString)));
}

void C5LineEdit::setText(const QString &arg)
{
    QString t = arg;
    QLineEdit::setText(t);
}

void C5LineEdit::appendText(const QString &arg)
{
    setText(text() + arg);
}

void C5LineEdit::setInteger(int i)
{
    setText(QString::number(i));
}

int C5LineEdit::getInteger()
{
    return text().toInt();
}

void C5LineEdit::setDouble(double i)
{
    QString si = float_str(i, fDecimalPlaces);
    setText(si);
}

double C5LineEdit::getDouble()
{
    // Accept '.', ',', Armenian ․ / ։ as decimal regardless of keyboard layout.
    return str_table_amount(text());
}

void C5LineEdit::setData(const QVariant &data)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    switch (data.type()) {
        case QVariant::Double:
            setDouble(data.toDouble());
            break;
        default:
            setText(data.toString());
            break;
    }
#else
    switch (data.typeId()) {
        case QMetaType::Double:
            setDouble(data.toDouble());
            break;
        default:
            setText(data.toString());
            break;
    }
#endif
}

int C5LineEdit::getTag()
{
    return fTag;
}

void C5LineEdit::setTag(int tag)
{
    fTag = tag;
}

void C5LineEdit::setColor(int c)
{
    fColor = c;
    if(c == -1) {
        setStyleSheet(QString());
        return;
    }
    QColor color = QColor::fromRgb(static_cast<QRgb>(c));
    if(color.alpha() == 0) {
        color.setAlpha(255);
    }
    const QColor text = color.lightness() > 128 ? QColor(Qt::black) : QColor(Qt::white);
    setStyleSheet(QStringLiteral(
                      "QLineEdit { background-color: %1; color: %2; }"
                      "QLineEdit:focus { background-color: %1; color: %2; }")
                      .arg(color.name(QColor::HexRgb), text.name(QColor::HexRgb)));
}

int C5LineEdit::color()
{
    return fColor;
}

void C5LineEdit::addEventKeys(const QString &keys)
{
    fEventKeys.append(keys);
}

void C5LineEdit::fixValue()
{
    setProperty("old", text());
}

QString C5LineEdit::old()
{
    return property("old").toString();
}

void C5LineEdit::setBgColor(const QColor &color)
{
    QPalette palette;
    palette.setColor(QPalette::Base, color);
    setPalette(palette);
}

namespace {

bool isDecimalSeparatorInput(const QKeyEvent *e)
{
    if (!e) {
        return false;
    }
    const int key = e->key();
    const QString t = e->text();
    const bool keypad = e->modifiers().testFlag(Qt::KeypadModifier);

    // Numpad Del (NumLock off) / Period / Comma → decimal
    if (keypad && (key == Qt::Key_Delete || key == Qt::Key_Period || key == Qt::Key_Comma)) {
        return true;
    }
    if (key == Qt::Key_Period || key == Qt::Key_Comma) {
        return true;
    }
    // Armenian one-dot leader U+2024 (․), full stop U+0589 (։), comma U+055D (՝)
    if (key == 0x2024 || key == 0x0589 || key == 0x055D) {
        return true;
    }
    if (t == QLatin1String(".") || t == QLatin1String(",")
        || t == QString(QChar(0x2024)) || t == QString(QChar(0x0589))
        || t == QString(QChar(0x055D))) {
        return true;
    }
    return false;
}

/** Fractional separator expected by QDoubleValidator for the current locale.
 *  Qt6: QLocale::decimalPoint() returns QString; Qt5: QChar. */
QString localeDecimalText()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const QString dec = QLocale().decimalPoint();
#else
    const QString dec = QString(QLocale().decimalPoint());
#endif
    if (dec.isEmpty() || dec == QString(QChar(0))) {
        return QStringLiteral(".");
    }
    return dec;
}

bool textAlreadyHasDecimal(const QString &text, const QString &localeDec)
{
    return text.contains(QLatin1Char('.'))
           || text.contains(QLatin1Char(','))
           || (!localeDec.isEmpty() && text.contains(localeDec))
           || text.contains(QChar(0x2024))
           || text.contains(QChar(0x0589))
           || text.contains(QChar(0x055D));
}

QString normalizeDecimalSeparators(const QString &arg, const QString &localeDec)
{
    QString normalized = arg;
    const QString dec = localeDec.isEmpty() ? QStringLiteral(".") : localeDec;
    normalized.replace(QChar(0x2024), dec);
    normalized.replace(QChar(0x0589), dec);
    normalized.replace(QChar(0x055D), dec);
    if (dec != QLatin1String(".")) {
        normalized.replace(QLatin1Char('.'), dec);
    }
    if (dec != QLatin1String(",")) {
        normalized.replace(QLatin1Char(','), dec);
    }
    // Keep only the last fractional separator.
    const int lastSep = normalized.lastIndexOf(dec);
    if (lastSep < 0) {
        return normalized;
    }
    QString cleaned;
    cleaned.reserve(normalized.size());
    const int decLen = dec.size();
    for (int i = 0; i < normalized.size(); ) {
        if (decLen > 0 && i == lastSep) {
            cleaned.append(dec);
            i += decLen;
        } else if (decLen > 0 && normalized.mid(i, decLen) == dec) {
            i += decLen; // drop earlier separators
        } else {
            cleaned.append(normalized.at(i));
            ++i;
        }
    }
    return cleaned;
}

} // namespace

void C5LineEdit::keyPressEvent(QKeyEvent *e)
{
    const QValidator *v = validator();
    if (v && !strcmp(v->metaObject()->className(), "QDoubleValidator")) {
        const QDoubleValidator *dv = static_cast<const QDoubleValidator *>(v);
        const int key = e->key();

        if (key == Qt::Key_Plus || key == 43) {
            // Do not insert '+'; still forward to addEventKeys handlers (e.g. fill price from hint).
            if (!fEventKeys.isEmpty() && fEventKeys.contains(QLatin1Char('+'))) {
                emit keyPressed(QLatin1Char('+'));
            }
            return;
        }
        if (key == Qt::Key_Minus || key == 45) {
            if (dv->bottom() > 0.0000001) {
                return;
            }
        }
        if (isDecimalSeparatorInput(e)) {
            const QString dec = localeDecimalText();
            if (textAlreadyHasDecimal(text(), dec)) {
                return;
            }
            QKeyEvent synthesized(QEvent::KeyPress, Qt::Key_Period, Qt::NoModifier, dec);
            QLineEdit::keyPressEvent(&synthesized);
            return;
        }
        if (key == Qt::Key_Return || key == Qt::Key_Enter) {
            if (isEmpty()) {
                emit returnPressed();
            }
        }
    }
    if (!fEventKeys.isEmpty()) {
        int key = e->key();
        if (key >= 0x20 && key <= 0x10FFFF) {
            QChar ch(static_cast<char32_t>(key));
            if (fEventKeys.contains(ch)) {
                emit keyPressed(ch);
                e->ignore();
                return;
            }
        }
    }
    QLineEdit::keyPressEvent(e);
}

void C5LineEdit::keyReleaseEvent(QKeyEvent *event)
{
    // Event keys are handled on keyPress only to avoid firing the action twice.
    if(!fEventKeys.isEmpty()) {
        const int key = event->key();
        if(key >= 0x20 && key <= 0x10FFFF && fEventKeys.contains(QChar(key))) {
            event->ignore();
            return;
        }
    }
    QLineEdit::keyReleaseEvent(event);
}

void C5LineEdit::focusOutEvent(QFocusEvent *event)
{
    QLineEdit::focusOutEvent(event);
    auto *v = qobject_cast<const QDoubleValidator *>(validator());
    if (v) {
        QTimer::singleShot(0, this, &QLineEdit::deselect);
    }
    emit focusOut();
}

void C5LineEdit::focusInEvent(QFocusEvent *event)
{
    QLineEdit::focusInEvent(event);
    auto *v = qobject_cast<const QDoubleValidator *>(validator());
    if (v) {
        QTimer::singleShot(0, this, &QLineEdit::selectAll);
    }

    emit focusIn();
}

void C5LineEdit::editText(const QString &arg)
{
    int cursPos = cursorPosition();
    QString normalized = arg;
    if (validator() && !strcmp(validator()->metaObject()->className(), "QDoubleValidator")) {
        normalized = normalizeDecimalSeparators(arg, localeDecimalText());
    }
    if (normalized != text()) {
        setText(normalized);
        setCursorPosition(qMin(cursPos, normalized.size()));
    }
    if (property("old").isValid()) {
        if (property("old").toString().compare(text(), Qt::CaseInsensitive) == 0) {
            setBgColor(Qt::white);
        } else {
            setBgColor(Qt::yellow);
        }
    }
}

C5LineEdit *isLineEdit(QObject *o)
{
    C5LineEdit *le = dynamic_cast<C5LineEdit *>(o);
    return le;
}
