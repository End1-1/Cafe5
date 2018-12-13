#include "c5tablewidget.h"
#include "c5lineedit.h"
#include "c5combobox.h"
#include "c5utils.h"
#include "excel.h"
#include <QHeaderView>

C5TableWidget::C5TableWidget(QWidget *parent) :
    QTableWidget(parent)
{
    setEditTriggers(NoEditTriggers);
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

void C5TableWidget::fitColumnsToWidth(int dec)
{
    int colWidths = 0;
    int hiddenColumns = 0;
    for (int i = 0; i < columnCount(); i++) {
        if (columnWidth(i) == 0) {
            hiddenColumns++;
        }
        colWidths += columnWidth(i);
    }
    int freeSpace = width() - colWidths - dec;
    int delta = 0;
    if (columnCount() - hiddenColumns > 0) {
        delta = freeSpace / (columnCount() - hiddenColumns);
    }
    for (int i = 0; i < columnCount(); i++) {
        if (columnWidth(i) == 0) {
            continue;
        }
        setColumnWidth(i, columnWidth(i) + delta);
    }
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

QVariant C5TableWidget::getData(int row, int column)
{
    return item(row, column)->text();
}

void C5TableWidget::setData(int row, int column, const QVariant &value)
{
    item(row, column)->setData(Qt::DisplayRole, value);
}

int C5TableWidget::getInteger(int row, int column)
{
    return item(row, column)->text().toInt();
}

void C5TableWidget::setInteger(int row, int column, int value)
{
    setString(row, column, QString::number(value));
}

QString C5TableWidget::getString(int row, int column)
{
    return item(row, column)->text();
}

void C5TableWidget::setString(int row, int column, const QString &str)
{
    if (!item(row, column)) {
        setItem(row, column, new QTableWidgetItem(str));
    } else {
        item(row, column)->setText(str);
    }
}

double C5TableWidget::getDouble(int row, int column)
{
    return QLocale().toDouble(item(row, column)->text());
}

void C5TableWidget::setDouble(int row, int column, double value)
{
    setString(row, column, float_str(value, 2));
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

void C5TableWidget::exportToExcel()
{
    int colCount = columnCount();
    if (colCount == 0 || rowCount() == 0) {
        return;
    }
    Excel e;
    for (int i = 0; i < colCount; i++) {
        e.setValue(horizontalHeaderItem(i)->text(), 1, i + 1);
        e.setColumnWidth(i + 1, columnWidth(i) / 7);
    }
    QColor color = QColor::fromRgb(200, 200, 250);
    e.setBackground(e.address(0, 0), e.address(0, colCount - 1),
                     color.red(), color.green(), color.blue());
    e.setFontBold(e.address(0, 0), e.address(0, colCount - 1));
    e.setHorizontalAlignment(e.address(0, 0), e.address(0, colCount - 1), Excel::hCenter);

    for (int j = 0; j < rowCount(); j++) {
        for (int i = 0; i < colCount; i++) {
            e.setValue(getString(j, i), j + 2, i + 1);
        }
    }

    e.setFontSize(e.address(0, 0), e.address(rowCount() , colCount ), 10);
    e.show();
}
