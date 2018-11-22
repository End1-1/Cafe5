#ifndef C5TABLEWIDGET_H
#define C5TABLEWIDGET_H

#include <QTableWidget>

class C5LineEdit;
class C5ComboBox;

class C5TableWidget : public QTableWidget
{
    Q_OBJECT

public:
    C5TableWidget(QWidget *parent = 0);

    void setColumnWidths(int count, ...);

    void fitColumnsToWidth(int dec = 5);

    C5LineEdit *createLineEdit(int row, int column);

    C5LineEdit *lineEdit(int row, int column);

    C5ComboBox *createComboBox(int row, int column);

    C5ComboBox *comboBox(int row, int column);

    bool findWidget(QWidget *w, int &row, int &column);

    int getInteger(int row, int column);

    void setInteger(int row, int column, int value);

    QString getString(int row, int column);

    void setString(int row, int column, const QString &str);

    double getDouble(int row, int column);

    void setDouble(int row, int column, double value);

    int addEmptyRow();

    void exportToExcel();
};

#endif // C5TABLEWIDGET_H
