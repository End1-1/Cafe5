#ifndef C5LINEEDIT_H
#define C5LINEEDIT_H

#include <QLineEdit>

class C5LineEdit : public QLineEdit
{
    Q_OBJECT

    Q_PROPERTY(int Tag READ getTag WRITE setTag)

public:
    C5LineEdit(QWidget *parent);

    QString fOldValue;

    int fDecimalPlaces;

    void setText(const QString &arg);

    void setInteger(int i);

    int getInteger();

    void setDouble(double i);

    double getDouble();

    int getTag();

    void setTag(int tag);

    inline bool isEmpty() {return text().isEmpty();}

    void setColor(int c);

    int color();

protected:
    void keyPressEvent(QKeyEvent *e);

private:
    int fTag;

    int fColor;

private slots:
    void editText(const QString &arg);
};

C5LineEdit *isLineEdit(QObject *o);

#endif // C5LINEEDIT_H
