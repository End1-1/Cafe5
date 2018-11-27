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

private:
    int fTag;

    int fColor;
};

#endif // C5LINEEDIT_H
