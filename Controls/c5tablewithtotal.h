#ifndef C5TABLEWITHTOTAL_H
#define C5TABLEWITHTOTAL_H

#include <QWidget>
#include <QMap>

namespace Ui {
class C5TableWithTotal;
}

class C5LineEdit;

class C5TableWithTotal : public QWidget
{
    Q_OBJECT

public:
    explicit C5TableWithTotal(QWidget *parent = nullptr);

    ~C5TableWithTotal();

    C5TableWithTotal &addColumn(const QString &name, int width, bool sum = false);

    C5TableWithTotal &countTotal(int index);

    void exportToExcel();

    void setRowColor(int row, const QColor &c);

    void setItemData(int row, int column, int role, const QVariant &d);

    const QVariant itemData(int row, int column, int role) const;

    int addRow();

    void removeRow(int row);

    int currentRow();

    int rowCount();

    int columnCount();

    QVariant getData(int row, int column, int role = Qt::EditRole) const;

    QString columnTitle(int column) const;

    int columnWidth(int column);

    double total(int column);

    QString totalStr(int column);

    void setRowCount(int count);

    C5TableWithTotal &setData(int &row, int column, const QVariant &data, bool newrow = false);

    C5TableWithTotal &createLineEdit(int &row, int column, const QVariant &data, QObject *obj = nullptr, const char *slot = nullptr, bool newrow = false);

    C5TableWithTotal &createLineEditDblClick(int &row, int column, QObject *obj = nullptr, const char *slot = nullptr, bool newrow = false);

    void sumColumns();

    C5LineEdit *lineEdit(int row, int column);

    void setWidget(int row, int column, QWidget *w);

    bool findWidget(int &row, int &column, QWidget *w);

    void clearTables();

    void moveRowUp(int row);

    void moveRowDown(int row);

private:
    Ui::C5TableWithTotal *ui;

    QMap<int, bool> fSumColumns;

    void replaceRows(int row1, int row2);

private slots:
    void mainHeaderResized(int index, int oldsize, int newsize);

    void totalScroll(int value);
};

#endif // C5TABLEWITHTOTAL_H
