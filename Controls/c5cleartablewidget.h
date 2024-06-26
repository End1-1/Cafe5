#ifndef C5CLEARTABLEWIDGET_H
#define C5CLEARTABLEWIDGET_H

#include <QTableWidget>

class C5CheckBox;
class C5LineEdit;

class C5TableWidgetItem : public QTableWidgetItem {
public:
    explicit C5TableWidgetItem(int type = Type);

    explicit C5TableWidgetItem(const QString &text, int type = Type);

    virtual QVariant data(int role) const override;

    inline QString text() {return data(Qt::DisplayRole).toString(); }

    int fDecimals;
};

class C5ClearTableWidget : public QTableWidget
{
    Q_OBJECT

public:
    C5ClearTableWidget(QWidget *parent = nullptr);

    C5TableWidgetItem *item(int row, int column) const;

    void setColumnWidths(int count, ...);

    void fitColumnsToWidth(int dec = 5);

    void fitRowToHeight(int dec = 5);

    bool findWidget(QWidget *w, int &row, int &column);

    QVariant getData(int row, int column, int role = Qt::EditRole);

    void setData(int row, int column, const QVariant &value);

    int getInteger(int row, int column);

    int getInteger(int row, const QString &columnName);

    void setInteger(int row, int column, int value);

    QString getString(int row, int column);

    void setString(int row, int column, const QString &str);

    double getDouble(int row, int column);

    double getDouble(int row, const QString &columnName);

    void setDouble(int row, int column, double value);

    int addEmptyRow();

    void exportToExcel();

    void search(const QString &txt);

    double sumOfColumn(int column);

    int visibleRows();

    void setColumnDecimals(int column, int decimals);

    int columnIndexOfName(const QString &name);

    C5CheckBox *createCheckbox(int row, int column);

    C5CheckBox *checkBox(int row, int column);

    C5LineEdit *createLineEdit(int row, int column);

    C5LineEdit *lineEdit(int row, int column);

public Q_SLOTS:
    void removeRow(int row);

private:
    QMap<int, int> fColumnsDecimals;

private slots:
    void lineEditTextChanged(const QString arg1);

    void checkBoxChecked(bool v);

};

#endif // C5CLEARTABLEWIDGET_H
