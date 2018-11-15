#include "c5tablewidget.h"
#include "c5lineedit.h"
#include "c5combobox.h"

C5TableWidget::C5TableWidget(QWidget *parent) :
    QTableWidget(parent)
{

}

void C5TableWidget::setColumnWidths(int count, ...)
{
    setColumnCount(count);
    va_list vl;
    va_start(vl, count);
    for (int i = 0; i < count; i++) {
        setColumnWidth(i, va_arg(vl, int));
    }
    va_end(vl);
}

C5LineEdit *C5TableWidget::createLineEdit(int row, int column)
{
    C5LineEdit *l = new C5LineEdit(this);
    l->setFrame(false);
    setCellWidget(row, column, l);
    return l;
}

C5LineEdit *C5TableWidget::lineEdit(int row, int column)
{
    return static_cast<C5LineEdit*>(cellWidget(row, column));
}

C5ComboBox *C5TableWidget::createComboBox(int row, int column)
{
    C5ComboBox *c = new C5ComboBox(this);
    c->setFrame(false);
    setCellWidget(row, column, c);
    return c;
}

C5ComboBox *C5TableWidget::comboBox(int row, int column)
{
    return static_cast<C5ComboBox*>(cellWidget(row, column));
}

bool C5TableWidget::findWidget(QWidget *w, int &row, int &column)
{
    for (int r = 0; r < rowCount(); r++) {
        for (int c = 0; c < columnCount(); c++) {
            if (cellWidget(r, c) == w) {
                row = r;
                column = c;
                return true;
            }
        }
    }
    return false;
}

int C5TableWidget::getInteger(int row, int column)
{
    return item(row, column)->text().toInt();
}

void C5TableWidget::setInteger(int row, int column, int value)
{
    item(row, column)->setText(QString::number(value));
}

QString C5TableWidget::getString(int row, int column)
{
    return item(row, column)->text();
}

void C5TableWidget::setString(int row, int column, const QString &str)
{
    item(row, column)->setText(str);
}

double C5TableWidget::getDouble(int row, int column)
{
    return item(row, column)->text().toDouble();
}

void C5TableWidget::setDouble(int row, int column, double value)
{
    item(row, column)->setText(QString::number(value, 'f', 2));
}

int C5TableWidget::addEmptyRow()
{
    int row = rowCount();
    setRowCount(row + 1);
    for (int i = 0; i < columnCount(); i++) {
        setItem(row, i, new QTableWidgetItem());
    }
    return row;
}
