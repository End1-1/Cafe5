#ifndef C5COMBOBOX_H
#define C5COMBOBOX_H

#include <QComboBox>

class C5ComboBox : public QComboBox
{
    Q_OBJECT

public:
    C5ComboBox(QWidget *parent = 0);

    int fOldIndex;
};

#endif // C5COMBOBOX_H
