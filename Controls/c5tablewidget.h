#ifndef C5TABLEWIDGET_H
#define C5TABLEWIDGET_H

#include <QTableWidget>

class C5LineEdit;
class C5ComboBox;
class C5CheckBox;

class C5TableWidget : public QTableWidget
{
    Q_OBJECT

public:
    C5TableWidget(QWidget *parent = 0);

    void setColumnWidths(int count, ...);

    void fitColumnsToWidth(int dec = 5);

    void fitRowToHeight(int dec = 5);

    C5LineEdit *createLineEdit(int row, int column);

    C5LineEdit *lineEdit(int row, int column);

    C5ComboBox *createComboBox(int row, int column);

    C5ComboBox *comboBox(int row, int column);

    C5CheckBox *createCheckbox(int row, int column);

    C5CheckBox *checkBox(int row, int column);

    bool findWidget(QWidget *w, int &row, int &column);

    QVariant getData(int row, int column);

    void setData(int row, int column, const QVariant &value);

    int getInteger(int row, int column);

    void setInteger(int row, int column, int value);

    QString getString(int row, int column);

    void setString(int row, int column, const QString &str);

    double getDouble(int row, int column);

    void setDouble(int row, int column, double value);

    int addEmptyRow();

    void exportToExcel();

    void search(const QString &txt);

private slots:
    void lineEditTextChanged(const QString arg1);

    void comboTextChanged(const QString &text);

    void checkBoxChecked(bool v);
};

#endif // C5TABLEWIDGET_H
