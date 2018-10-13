#ifndef C5LINEEDIT_H
#define C5LINEEDIT_H

#include <QLineEdit>

class C5LineEdit : public QLineEdit
{
    Q_OBJECT

public:
    C5LineEdit(QWidget *parent);

    QString fOldValue;
};

#endif // C5LINEEDIT_H
