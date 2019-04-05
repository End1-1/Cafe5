#include "c5dateedit.h"
#include "c5utils.h"

QDate C5DateEditFirstDate = QDate::currentDate();
QDate C5DateEditMinDate = QDate::fromString("01/01/1900", "dd/MM/yyyy");

C5DateEdit::C5DateEdit(QWidget *parent) :
    C5LineEdit(parent)
{
    setInputMask("00/00/0000");
    setDate(C5DateEditFirstDate);
    connect(this, SIGNAL(textChanged(QString)), this, SLOT(newText(QString)));
    setMaximumWidth(100);
    fRow = 0;
    fColumn = 0;
    fDoNoCheckMinDate = false;
}

void C5DateEdit::setText(const QString &text)
{
    if (inputMask() == "00/00/0000") {
        QDate d = QDate::fromString(text, FORMAT_DATE_TO_STR);
        if (d.isValid() && d < C5DateEditMinDate && !fDoNoCheckMinDate) {
            d = C5DateEditMinDate;
        }
        QLineEdit::setText(d.toString(FORMAT_DATE_TO_STR));
    } else if (inputMask() == "00:00:00") {
        QTime t = QTime::fromString(text, FORMAT_TIME_TO_STR);
        if (t.isValid()) {
            QLineEdit::setText(t.toString(FORMAT_TIME_TO_STR));
        }
    } else {
        QLineEdit::setText(text);
    }
}

QDate C5DateEdit::date()
{
    QString t = text();
    QDate d;
    if (inputMask() == "00/00/0000") {
        d = QDate::fromString(t, FORMAT_DATE_TO_STR);
    } else {
        d = QDate::fromString(t, FORMAT_DATETIME_TO_STR);
    }
    return d;
}

QString C5DateEdit::toMySQLDate(bool ap)
{
    QString d = date().toString("yyyy-MM-dd");
    if (ap) {
        d = "'" + d + "'";
    }
    return d;
}

void C5DateEdit::setDate(const QDate &date)
{
    QDate d = date;
    if (d.isValid() && d < C5DateEditMinDate && !fDoNoCheckMinDate) {
        d = C5DateEditMinDate;
    }
    setText(d.toString(FORMAT_DATE_TO_STR));
}

void C5DateEdit::setDateTime(const QDateTime &datetime)
{
    setInputMask("00/00/0000 00:00:00");
    setText(datetime.toString(FORMAT_DATETIME_TO_STR));
}

void C5DateEdit::setTime(const QTime &time)
{
    setInputMask("00:00:00");
    setText(time.toString(FORMAT_TIME_TO_STR));
}

QString C5DateEdit::getField()
{
    return fField;
}

void C5DateEdit::setField(const QString &field)
{
    fField = field;
}

bool C5DateEdit::getCheckMinDate()
{
    return fDoNoCheckMinDate;
}

void C5DateEdit::setCheckMinDate(bool v)
{
    fDoNoCheckMinDate = v;
}

void C5DateEdit::focusInEvent(QFocusEvent *e)
{
    QLineEdit::focusInEvent(e);
    setCursorPosition(0);
}

void C5DateEdit::setBgColor(const QColor &color)
{
    QPalette palette;
    palette.setColor(QPalette::Base, color);
    if (!fIsValid) {
        palette.setColor(QPalette::Base, Qt::red);
    }
    setPalette(palette);
}

void C5DateEdit::newText(const QString &arg1)
{
    if (inputMask() == "00/00/0000") {
        QDate d = QDate::fromString(arg1, FORMAT_DATE_TO_STR);
        if (d.isValid() && d < C5DateEditMinDate) {
            if (!fDoNoCheckMinDate) {
                d = C5DateEditMinDate;
            }
        }
        fIsValid = d.isValid();
        if (d.isValid()) {
            setBgColor(Qt::white);
        } else {
            setBgColor(Qt::red);
        }
        int cp = cursorPosition();
        if (d.isValid()) {
            QLineEdit::setText(d.toString(FORMAT_DATE_TO_STR));
        }
        setCursorPosition(cp);
        emit dateChanged(d);
    } else if (inputMask() == "00:00:00") {
        QTime t = QTime::fromString(arg1, FORMAT_TIME_TO_STR);
        fIsValid = t.isValid();
        if (t.isValid()) {
            setBgColor(Qt::white);
        } else {
            setBgColor(Qt::red);
        }
        int cp = cursorPosition();
        if (t.isValid()) {
            QLineEdit::setText(t.toString(FORMAT_TIME_TO_STR));
        }
        setCursorPosition(cp);
        emit timeChanged(t);
    }
}

C5DateEdit *isDateEdit(QObject *o)
{
    C5DateEdit *de = dynamic_cast<C5DateEdit*>(o);
    return de;
}
