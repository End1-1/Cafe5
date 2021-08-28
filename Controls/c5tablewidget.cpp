#include "c5tablewidget.h"
#include "c5lineedit.h"
#include "c5combobox.h"
#include "c5message.h"
#include "c5utils.h"
#include "c5lineeditwithselector.h"
#include "c5checkbox.h"
#include "xlsxall.h"
#include <QHeaderView>
#include <QApplication>

C5TableWidget::C5TableWidget(QWidget *parent) :
    QTableWidget(parent)
{
    setEditTriggers(NoEditTriggers);
}

C5TableWidgetItem *C5TableWidget::item(int row, int column) const
{
    return static_cast<C5TableWidgetItem*>(QTableWidget::item(row, column));
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

void C5TableWidget::fitRowToHeight(int dec)
{
    int rh = verticalHeader()->defaultSectionSize();
    int visibleRows = (height() - dec) / rh;
    int delta = (height() - dec) - (visibleRows * rh);
    verticalHeader()->setDefaultSectionSize(rh + (delta / visibleRows));
}

C5LineEdit *C5TableWidget::createLineEdit(int row, int column)
{
    C5LineEdit *l = new C5LineEdit(this);
    l->setProperty("row", row);
    l->setProperty("column", column);
    l->setFrame(false);
    connect(l, SIGNAL(textChanged(QString)), this, SLOT(lineEditTextChanged(QString)));
    setCellWidget(row, column, l);
    return l;
}

C5LineEdit *C5TableWidget::lineEdit(int row, int column)
{
    return static_cast<C5LineEdit*>(cellWidget(row, column));
}

C5LineEditWithSelector *C5TableWidget::createLineEditWiSelector(int row, int column)
{
    C5LineEditWithSelector *l = new C5LineEditWithSelector(this);
    l->setProperty("row", row);
    l->setProperty("column", column);
    l->setFrame(false);
    connect(l, SIGNAL(textChanged(QString)), this, SLOT(lineEditTextChanged(QString)));
    setCellWidget(row, column, l);
    return l;
}

C5LineEditWithSelector *C5TableWidget::lineEditWithSelector(int row, int column)
{
    return static_cast<C5LineEditWithSelector*>(cellWidget(row, column));
}

C5ComboBox *C5TableWidget::createComboBox(int row, int column)
{
    C5ComboBox *c = new C5ComboBox(this);
    c->setProperty("row", row);
    c->setProperty("column", column);
    c->setFrame(false);
    setCellWidget(row, column, c);
    connect(c, SIGNAL(currentTextChanged(QString)), this, SLOT(comboTextChanged(QString)));
    return c;
}

C5ComboBox *C5TableWidget::comboBox(int row, int column)
{
    return static_cast<C5ComboBox*>(cellWidget(row, column));
}

C5CheckBox *C5TableWidget::createCheckbox(int row, int column)
{
    C5CheckBox *c = new C5CheckBox(this);
    c->setProperty("row", row);
    c->setProperty("column", column);
    setCellWidget(row, column, c);
    connect(c, SIGNAL(clicked(bool)), this, SLOT(checkBoxChecked(bool)));
    return c;
}

C5CheckBox *C5TableWidget::checkBox(int row, int column)
{
    return static_cast<C5CheckBox*>(cellWidget(row, column));
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
    C5TableWidgetItem *i = item(row, column);
    return i->data(Qt::EditRole);
}

void C5TableWidget::setData(int row, int column, const QVariant &value)
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

int C5TableWidget::getInteger(int row, int column)
{
    return getData(row, column).toInt();
}

int C5TableWidget::getInteger(int row, const QString &columnName)
{
    return getInteger(row, columnIndexOfName(columnName));
}

void C5TableWidget::setInteger(int row, int column, int value)
{
    setData(row, column, value);
}

QString C5TableWidget::getString(int row, int column)
{
    C5TableWidgetItem *i = item(row, column);
    if (!i) {
        return "NO ITEM!";
    }
    return i->text();
}

void C5TableWidget::setString(int row, int column, const QString &str)
{
    setData(row, column, str);
}

double C5TableWidget::getDouble(int row, int column)
{
    return getData(row, column).toDouble();
}

double C5TableWidget::getDouble(int row, const QString &columnName)
{
    return getDouble(row, columnIndexOfName(columnName));
}

void C5TableWidget::setDouble(int row, int column, double value)
{
    setData(row, column, value);
}

int C5TableWidget::addEmptyRow()
{
    int row = rowCount();
    setRowCount(row + 1);
    for (int i = 0; i < columnCount(); i++) {
        setItem(row, i, new C5TableWidgetItem());
    }
    return row;
}

void C5TableWidget::exportToExcel()
{
    if (columnCount() == 0 || rowCount() == 0) {
        C5Message::info(tr("Empty report"));
        return;
    }
    XlsxDocument d;
    XlsxSheet *s = d.workbook()->addSheet("Sheet1");
    /* HEADER */
    QColor color = QColor::fromRgb(200, 200, 250);
    QFont headerFont(qApp->font());
    headerFont.setBold(true);
    d.style()->addFont("header", headerFont);
    d.style()->addBackgrounFill("header", color);
    for (int i = 0; i < columnCount(); i++) {
        s->addCell(1, i + 1, horizontalHeaderItem(i)->data(Qt::DisplayRole).toString(), d.style()->styleNum("header"));
        s->setColumnWidth(i + 1, columnWidth(i) / 7);
    }
    //e.setHorizontalAlignment(e.address(0, 0), e.address(0, colCount - 1), Excel::hCenter);
    /* BODY */
    QFont bodyFont(qApp->font());
    d.style()->addFont("body", bodyFont);
    for (int j = 0; j < rowCount(); j++) {
        for (int i = 0; i < columnCount(); i++) {
            s->addCell(j + 2, i + 1,  item(j, i)->data(Qt::EditRole), d.style()->styleNum("body"));
        }
    }
    QString err;
    if (!d.save(err, true)) {
        if (!err.isEmpty()) {
            C5Message::error(err);
        }
    }
}

void C5TableWidget::search(const QString &txt)
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

double C5TableWidget::sumOfColumn(int column)
{
    double total = 0;
    for (int i = 0; i < rowCount(); i++) {
        if (!isRowHidden(i)) {
            total += getDouble(i, column);
        }
    }
    return total;
}

int C5TableWidget::visibleRows()
{
    int rows = 0;
    for (int i = 0; i < rowCount(); i++) {
        if (!isRowHidden(i)) {
            rows++;
        }
    }
    return rows;
}

void C5TableWidget::setColumnDecimals(int column, int decimals)
{
    fColumnsDecimals[column] = decimals;
}

int C5TableWidget::columnIndexOfName(const QString &name)
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

void C5TableWidget::lineEditTextChanged(const QString arg1)
{
    C5LineEdit *l = static_cast<C5LineEdit*>(sender());
    int row, col;
    findWidget(l, row, col);
    setString(row, col, arg1);
}

void C5TableWidget::comboTextChanged(const QString &text)
{
    C5ComboBox *c = static_cast<C5ComboBox*>(sender());
    int row, col;
    findWidget(c, row, col);
    setString(row, col, text);
}

void C5TableWidget::checkBoxChecked(bool v)
{
    C5CheckBox *c = static_cast<C5CheckBox*>(sender());
    int row, col;
    findWidget(c, row, col);
    setString(row, col, (v ? "1" : "0"));
}

C5TableWidgetItem::C5TableWidgetItem(int type) :
    QTableWidgetItem (type)
{
    fDecimals = 2;
}

C5TableWidgetItem::C5TableWidgetItem(const QString &text, int type) :
    QTableWidgetItem (text, type)
{

}

QVariant C5TableWidgetItem::data(int role) const
{
    QVariant v = QTableWidgetItem::data(role);
    if (role == Qt::DisplayRole) {
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
    return v;
}
