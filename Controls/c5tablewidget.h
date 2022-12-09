#ifndef C5TABLEWIDGET_H
#define C5TABLEWIDGET_H

#include "c5cleartablewidget.h"

class C5ComboBox;


class C5TableWidget : public C5ClearTableWidget
{
    Q_OBJECT

public:
    C5TableWidget(QWidget *parent = nullptr);

    template<class T>
    T *createWidget(int row, int column) {
        T *l = new T(this);
        l->setProperty("row", row);
        l->setProperty("column", column);
        l->setFrame(false);
        connect(l, SIGNAL(textChanged(QString)), this, SLOT(lineEditTextChanged(QString)));
        setCellWidget(row, column, l);
        return l;
    }

    C5ComboBox *createComboBox(int row, int column);

    C5ComboBox *comboBox(int row, int column);


private slots:
    void comboTextChanged(const QString &text);

};

#endif // C5TABLEWIDGET_H
