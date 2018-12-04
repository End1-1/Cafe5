#ifndef EDATEEDIT_H
#define EDATEEDIT_H

#include "c5lineedit.h"
#include <QDate>

class C5DateEdit : public C5LineEdit
{
    Q_OBJECT

    Q_PROPERTY(QString Field READ getField WRITE setField)

    Q_PROPERTY(bool DoNotCheckMinDate READ getCheckMinDate WRITE setCheckMinDate)

public:
    C5DateEdit(QWidget *parent = 0);

    void setText(const QString &text);

    QDate date();

    QString toMySQLDate(bool ap = true);

    void setDate(const QDate &date);

    QString getField();

    void setField(const QString &field);

    bool getCheckMinDate();

    void setCheckMinDate(bool v);

    int fRow;

    int fColumn;

protected:
    virtual void focusInEvent(QFocusEvent *e);

private:
    bool fIsValid;

    bool fDoNoCheckMinDate;

    QString fField;

    void setBgColor(const QColor &color);

private slots:
    void newText(const QString &arg1);

signals:
    void dateChanged(const QDate &date);
};

C5DateEdit *isDateEdit(QObject *o);

#endif // EDATEEDIT_H
