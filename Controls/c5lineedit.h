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

    void setInteger(int i);

    int getInteger();

    void setDouble(double i);

    double getDouble();

    int getTag();

    void setTag(int tag);

private:
    int fTag;
};

#endif // C5LINEEDIT_H
