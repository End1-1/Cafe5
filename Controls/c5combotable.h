#ifndef C5COMBOTABLE_H
#define C5COMBOTABLE_H

#include <QComboBox>
#include <QStandardItemModel>
#include <QObject>

class C5ComboTable : public QComboBox
{
    Q_OBJECT

public:
    C5ComboTable(QWidget *parent = nullptr);

private:
    QStandardItemModel fModel;
};

#endif // C5COMBOTABLE_H
