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

    void setDBValues(const QString &sql, const QVariant &defaultValue = QVariant());

    void setCache(int cacheid, int colId = 0, int colName = 1);

    void setValues(const QList<int> values, const QStringList &names);

private:
    int fTag;

};

#endif // C5COMBOBOX_H
