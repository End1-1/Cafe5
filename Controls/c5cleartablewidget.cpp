#include "c5cleartablewidget.h"
#include "c5message.h"
#include "c5utils.h"
#include "c5checkbox.h"
#include "c5lineedit.h"
#include <QHeaderView>
#include <QFileDialog>
#include <QDesktopServices>
#include <QApplication>
#include <QXlsx/header/xlsxdocument.h>

C5TableWidgetItem::C5TableWidgetItem(int type) :
    QTableWidgetItem (type)
{
    fDecimals = 2;
}

C5TableWidgetItem::C5TableWidgetItem(const QString &text, int type) :
    QTableWidgetItem (text, type)
{
}

C5ClearTableWidget::C5ClearTableWidget(QWidget *parent) :
    QTableWidget(parent)
{
    setEditTriggers(NoEditTriggers);
    if (!property("columns").toString().isEmpty()) {
        QStringList cols = property("columns").toString().split(",", Qt::SkipEmptyParts);
        for (int i = 0; i < cols.count(); i++) {
            setColumnWidth(i, cols.at(i).toInt());
        }
    }
}

C5TableWidgetItem *C5ClearTableWidget::item(int row, int column) const
{
    return static_cast<C5TableWidgetItem *>(QTableWidget::item(row, column));
}

void C5ClearTableWidget::setColumnWidths(int count, ...)
{
    setColumnCount(count);
    va_list vl;
    va_start(vl, count);
    for (int i = 0; i < count; i++) {
        setColumnWidth(i, va_arg(vl, int));
    }
    va_end(vl);
}

void C5ClearTableWidget::fitColumnsToWidth(int dec)
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

void C5ClearTableWidget::fitRowToHeight(int dec)
{
    int rh = verticalHeader()->defaultSectionSize();
    int visibleRows = (height() - dec) / rh;
    int delta = (height() - dec) - (visibleRows *rh);
    verticalHeader()->setDefaultSectionSize(rh + (delta / visibleRows));
}

bool C5ClearTableWidget::findWidget(QWidget *w, int &row, int &column)
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

QVariant C5ClearTableWidget::getData(int row, int column, int role)
{
    C5TableWidgetItem *i = item(row, column);
    if (!i) {
        i = new C5TableWidgetItem();
    }
    return i->data(role);
}

void C5ClearTableWidget::setData(int row, int column, const QVariant &value)
{
    C5TableWidgetItem *i = item(row, column);
    if (!i) {
        i = new C5TableWidgetItem();
        QTableWidget::setItem(row, column, i);
        if (fColumnsDecimals.contains(column)) {
            i->fDecimals = fColumnsDecimals[column];
        }
    }
    i->setData(Qt::EditRole, value);
}

int C5ClearTableWidget::getInteger(int row, int column)
{
    return getData(row, column).toInt();
}

int C5ClearTableWidget::getInteger(int row, const QString &columnName)
{
    return getInteger(row, columnIndexOfName(columnName));
}

void C5ClearTableWidget::setInteger(int row, int column, int value)
{
    setData(row, column, value);
}

QString C5ClearTableWidget::getString(int row, int column)
{
    C5TableWidgetItem *i = item(row, column);
    if (!i) {
        return "NO ITEM!";
    }
    return i->text();
}

void C5ClearTableWidget::setString(int row, int column, const QString &str)
{
    setData(row, column, str);
}

double C5ClearTableWidget::getDouble(int row, int column)
{
    return getData(row, column, Qt::EditRole).toDouble();
}

double C5ClearTableWidget::getDouble(int row, const QString &columnName)
{
    return getDouble(row, columnIndexOfName(columnName));
}

void C5ClearTableWidget::setDouble(int row, int column, double value)
{
    setData(row, column, value);
}

int C5ClearTableWidget::addEmptyRow()
{
    int row = rowCount();
    setRowCount(row + 1);
    for (int i = 0; i < columnCount(); i++) {
        setItem(row, i, new C5TableWidgetItem());
    }
    return row;
}

void C5ClearTableWidget::removeRow(int row)
{
    QTableWidget::removeRow(row);
    for (int r = row; r < rowCount(); r++) {
        for (int c = 0; c < columnCount(); c++) {
            QWidget *w = cellWidget(r, c);
            if (w) {
                w->setProperty("row", r);
            }
        }
    }
}

void C5ClearTableWidget::exportToExcel()
{
    if (columnCount() == 0 || rowCount() == 0) {
        C5Message::info(tr("Empty report"));
        return;
    }
    QXlsx::Document d;
    d.addSheet("Sheet1");
    /* HEADER */
    QColor color = QColor::fromRgb(200, 200, 250);
    QFont headerFont(qApp->font());
    QXlsx::Format  hf;
    hf.setFont(headerFont);
    hf.setFontBold(true);
    hf.setPatternBackgroundColor(color);
    for (int i = 0; i < columnCount(); i++) {
        d.write(1, i + 1, horizontalHeaderItem(i)->data(Qt::DisplayRole).toString(), hf);
        d.setColumnWidth(i + 1, columnWidth(i) / 7);
    }
    //e.setHorizontalAlignment(e.address(0, 0), e.address(0, colCount - 1), Excel::hCenter);
    /* BODY */
    QFont bodyFont(qApp->font());
    QXlsx::Format bf;
    bf.setFont(bodyFont);
    for (int j = 0; j < rowCount(); j++) {
        for (int i = 0; i < columnCount(); i++) {
            d.write(j + 2, i + 1,  item(j, i)->data(Qt::EditRole), bf);
        }
    }
    QString filename = QFileDialog::getSaveFileName(nullptr, "", "", "*.xlsx");
    if (filename.isEmpty()) {
        return;
    }
    d.saveAs(filename);
    QDesktopServices::openUrl(filename);
}

void C5ClearTableWidget::search(const QString &txt)
{
    for (int i = 0; i < rowCount(); i++) {
        bool hidden = true;
        for (int j = 0; j < columnCount(); j++) {
            if (getString(i, j).contains(txt, Qt::CaseInsensitive)) {
                hidden = false;
            }
        }
        setRowHidden(i, hidden);
    }
}

double C5ClearTableWidget::sumOfColumn(int column)
{
    double total = 0;
    for (int i = 0; i < rowCount(); i++) {
        if (!isRowHidden(i)) {
            total += getDouble(i, column);
        }
    }
    return total;
}

int C5ClearTableWidget::visibleRows()
{
    int rows = 0;
    for (int i = 0; i < rowCount(); i++) {
        if (!isRowHidden(i)) {
            rows++;
        }
    }
    return rows;
}

void C5ClearTableWidget::setColumnDecimals(int column, int decimals)
{
    fColumnsDecimals[column] = decimals;
}

int C5ClearTableWidget::columnIndexOfName(const QString &name)
{
    int col = -1;
    for (int i = 0; i < columnCount(); i++) {
        if (horizontalHeaderItem(i)->text().toLower() == name.toLower()) {
            col = i;
            break;
        }
    }
    Q_ASSERT(col > -1);
    return col;
}

C5CheckBox *C5ClearTableWidget::createCheckbox(int row, int column)
{
    C5CheckBox *c = new C5CheckBox(this);
    c->setProperty("row", row);
    c->setProperty("column", column);
    setCellWidget(row, column, c);
    connect(c, SIGNAL(clicked(bool)), this, SLOT(checkBoxChecked(bool)));
    return c;
}

C5CheckBox *C5ClearTableWidget::checkBox(int row, int column)
{
    return static_cast<C5CheckBox *>(cellWidget(row, column));
}

C5LineEdit *C5ClearTableWidget::createLineEdit(int row, int column)
{
    C5LineEdit *l = new C5LineEdit(this);
    l->setProperty("row", row);
    l->setProperty("column", column);
    l->setFrame(false);
    l->setStyleSheet("border: none;outline: none;");
    setCellWidget(row, column, l);
    connect(l, SIGNAL(textChanged(QString)), this, SLOT(lineEditTextChanged(QString)));
    connect(l, &C5LineEdit::focusIn, [this, l, column]() {
        int r, c = column;
        if (findWidget(l, r, c)) {
            if (r != currentRow()) {
                setCurrentCell(r, c);
            }
        }
    });
    return l;
}

C5LineEdit *C5ClearTableWidget::lineEdit(int row, int column)
{
    return static_cast<C5LineEdit *>(cellWidget(row, column));
}

QVariant C5TableWidgetItem::data(int role) const
{
    QVariant v = QTableWidgetItem::data(role);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    if (role == Qt::DisplayRole) {
        if (v.userType() == QMetaType::QJsonValue) {
            v = v.toJsonValue().toVariant();
        }
        switch (v.type()) {
            case QVariant::Int:
                return v.toString();
            case QVariant::Date:
                return v.toDate().toString(FORMAT_DATE_TO_STR);
            case QVariant::DateTime:
                return v.toDateTime().toString(FORMAT_DATETIME_TO_STR);
            case QVariant::Time:
                return v.toTime().toString(FORMAT_TIME_TO_STR);
            case QVariant::Double:
                return float_str(v.toDouble(), fDecimals);
            default:
                return v.toString();
        }
    }
#else
    if (role == Qt::DisplayRole) {
        switch (v.typeId()) {
            case QMetaType::Int:
                return v.toString();
            case QMetaType::QDate:
                return v.toDate().toString(FORMAT_DATE_TO_STR);
            case QMetaType::QDateTime:
                return v.toDateTime().toString(FORMAT_DATETIME_TO_STR);
            case QMetaType::QTime:
                return v.toTime().toString(FORMAT_TIME_TO_STR);
            case QMetaType::Double:
                return float_str(v.toDouble(), fDecimals);
            default:
                return v.toString();
        }
    }
#endif
    return v;
}

void C5ClearTableWidget::lineEditTextChanged(const QString arg1)
{
    C5LineEdit *l = static_cast<C5LineEdit *>(sender());
    int row, col;
    findWidget(l, row, col);
    setString(row, col, arg1);
}

void C5ClearTableWidget::checkBoxChecked(bool v)
{
    C5CheckBox *c = static_cast<C5CheckBox *>(sender());
    int row, col;
    findWidget(c, row, col);
    setString(row, col, (v ? "1" : "0"));
}
