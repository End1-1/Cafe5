#ifndef C5CHECKBOX_H
#define C5CHECKBOX_H

#include <QCheckBox>

class C5CheckBox : public QCheckBox
{
    Q_OBJECT

    Q_PROPERTY(int Tag READ getTag WRITE setTag)

public:
    C5CheckBox(QWidget *parent = 0);

    ~C5CheckBox();

    int getTag();

    void setTag(int tag);

private:
    int fTag;
};

C5CheckBox *isCheckBox(QObject *o);

#endif // C5CHECKBOX_H
