#ifndef C5COMBOBOX_H
#define C5COMBOBOX_H

#include <QComboBox>

class C5ComboBox : public QComboBox
{
    Q_OBJECT

    Q_PROPERTY(int Tag READ getTag WRITE setTag)

public:
    C5ComboBox(QWidget *parent = 0);

    int fOldIndex;

    int getTag();

    void setTag(int tag);

    void setIndexForValue(const QVariant &value);

    void setDBValues(const QStringList dbParams, const QString &sql, const QVariant &defaultValue = QVariant());

private:
    int fTag;

};

#endif // C5COMBOBOX_H
