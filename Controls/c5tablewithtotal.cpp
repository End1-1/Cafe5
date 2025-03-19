#include "c5tablewithtotal.h"
#include "ui_c5tablewithtotal.h"
#include "c5lineedit.h"
#include "c5lineeditwithselector.h"
#include "c5message.h"
#include <QScrollBar>
#include <QDebug>
#include <QElapsedTimer>
#include <QFileDialog>
#include <QDesktopServices>
#include <QXlsx/header/xlsxdocument.h>

C5TableWithTotal::C5TableWithTotal(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::C5TableWithTotal)
{
    ui->setupUi(this);
    QStringList vl;
    vl.append("");
    ui->tblTotal->setVerticalHeaderLabels(vl);
    connect(ui->tblMain->horizontalHeader(), SIGNAL(sectionResized(int, int, int)), this, SLOT(mainHeaderResized(int, int,
            int)));
    connect(ui->tblTotal->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(totalScroll(int)));
}

C5TableWithTotal::~C5TableWithTotal()
{
    delete ui;
}

C5TableWithTotal &C5TableWithTotal::addColumn(const QString &name, int width, bool sum)
{
    int index = ui->tblMain->columnCount();
    fSumColumns[index] = sum;
    ui->tblMain->setColumnCount(index + 1);
    ui->tblMain->setHorizontalHeaderItem(index, new QTableWidgetItem(name));
    ui->tblMain->setColumnWidth(index, width);
    ui->tblTotal->setColumnCount(index + 1);
    ui->tblTotal->setColumnWidth(index, width);
    ui->tblTotal->setString(0, index, "");
    return *this;
}

C5TableWithTotal &C5TableWithTotal::countTotal(int index)
{
    if (index > -1) {
        ui->tblTotal->setDouble(0, index, ui->tblMain->sumOfColumn(index));
    } else {
        for (QMap<int, bool>::const_iterator it = fSumColumns.begin(); it != fSumColumns.end(); it++) {
            if (it.value()) {
                ui->tblTotal->setData(0, it.key(), ui->tblMain->sumOfColumn(it.key()));
            }
        }
    }
    return *this;
}

void C5TableWithTotal::exportToExcel()
{
    if (columnCount() == 0 || rowCount() == 0) {
        C5Message::info(tr("Empty report!"));
        return;
    }
    QXlsx::Document d;
    d.addSheet("Sheet1");
    /* HEADER */
    QColor color = QColor::fromRgb(200, 200, 250);
    QFont headerFont(qApp->font());
    headerFont.setBold(true);
    QXlsx::Format hf;
    hf.setFont(headerFont);
    hf.setBorderStyle(QXlsx::Format::BorderThin);
    hf.setPatternBackgroundColor(color);
    for (int i = 0; i < columnCount(); i++) {
        d.write(1, i + 1, columnTitle(i), hf);
        d.setColumnWidth(i + 1, columnWidth(i) / 7);
    }
    /* BODY */
    QFont bodyFont(qApp->font());
    QXlsx::Format bf;
    bf.setFont(bodyFont);
    bf.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    bf.setBorderStyle(QXlsx::Format::BorderThin);
    for (int j = 0; j < rowCount(); j++) {
        for (int i = 0; i < columnCount(); i++) {
            d.write(j + 2, i + 1, getData(j, i, Qt::EditRole), bf);
        }
    }
    /* TOTALS ROWS */
    for (int i = 0; i < columnCount(); i++) {
        d.write(1 + rowCount() + 1, i + 1, totalStr(i), hf);
    }
    QString filename = QFileDialog::getSaveFileName(nullptr, "", "", "*.xlsx");
    if (filename.isEmpty()) {
        return;
    }
    d.saveAs(filename);
    QDesktopServices::openUrl(filename);
}

void C5TableWithTotal::setRowColor(int row, const QColor &c)
{
    for (int i = 0; i < ui->tblMain->columnCount(); i++) {
        ui->tblMain->item(row, i)->setBackground(c);
    }
}

void C5TableWithTotal::setItemData(int row, int column, int role, const QVariant &d)
{
    ui->tblMain->item(row, column)->setData(role, d);
}

const QVariant C5TableWithTotal::itemData(int row, int column, int role) const
{
    return ui->tblMain->item(row, column)->data(role);
}

int C5TableWithTotal::addRow()
{
    int r = ui->tblMain->addEmptyRow();
    QStringList l;
    l.append(QString::number(r));
    ui->tblTotal->setVerticalHeaderLabels(l);
    return r;
}

void C5TableWithTotal::removeRow(int row)
{
    ui->tblMain->removeRow(row);
    for (QMap<int, bool>::const_iterator it = fSumColumns.begin(); it != fSumColumns.end(); it++) {
        if (it.value()) {
            ui->tblTotal->setData(0, it.key(), ui->tblMain->sumOfColumn(it.key()));
        }
    }
}

int C5TableWithTotal::currentRow()
{
    return ui->tblMain->currentRow();
}

int C5TableWithTotal::rowCount()
{
    return ui->tblMain->rowCount();
}

int C5TableWithTotal::columnCount()
{
    return ui->tblMain->columnCount();
}

QVariant C5TableWithTotal::getData(int row, int column, int role) const
{
    return ui->tblMain->getData(row, column, role);
}

QString C5TableWithTotal::columnTitle(int column) const
{
    return ui->tblMain->horizontalHeaderItem(column)->data(Qt::DisplayRole).toString();
}

int C5TableWithTotal::columnWidth(int column)
{
    return ui->tblMain->columnWidth(column);
}

double C5TableWithTotal::total(int column)
{
    return ui->tblTotal->getDouble(0, column);
}

QString C5TableWithTotal::totalStr(int column)
{
    return ui->tblTotal->getString(0, column);
}

void C5TableWithTotal::setRowCount(int count)
{
    ui->tblMain->setRowCount(count);
    for (int r = 0; r < count; r++) {
        for (int c = 0; c < ui->tblMain->columnCount(); c++) {
            ui->tblMain->setItem(r, c, new C5TableWidgetItem());
        }
    }
}

C5TableWithTotal &C5TableWithTotal::setData(int &row, int column, const QVariant &data, bool newrow)
{
    if (newrow) {
        row = addRow();
    }
    ui->tblMain->setData(row, column, data);
    return *this;
}

C5TableWithTotal &C5TableWithTotal::createLineEdit(int &row, int column, const QVariant &data, QObject *obj,
        const char *slot, bool newrow)
{
    if (newrow) {
        ui->tblMain->addEmptyRow();
    }
    ui->tblMain->createLineEdit(row, column)->setData(data);
    if (slot) {
        connect(ui->tblMain->lineEdit(row, column), SIGNAL(textChanged(QString)), obj, slot);
    }
    return *this;
}

C5TableWithTotal &C5TableWithTotal::createLineEditDblClick(int &row, int column, QObject *obj, const char *slot,
        bool newrow)
{
    setData(row, column, QVariant(), newrow);
    ui->tblMain->createLineEditWithSelector(row, column, nullptr, nullptr)->setData("");
    if (slot) {
        connect(ui->tblMain->lineEditWithSelector(row, column), SIGNAL(doubleClicked()), obj, slot);
    }
    return *this;
}

void C5TableWithTotal::sumColumns()
{
    for (QMap<int, bool>::const_iterator it = fSumColumns.constBegin(); it != fSumColumns.constEnd(); it++) {
        if (it.value()) {
            ui->tblTotal->setDouble(0, it.key(), ui->tblMain->sumOfColumn(it.key()));
        }
    }
    QStringList l;
    l.append(QString::number(ui->tblMain->rowCount()));
    ui->tblTotal->setVerticalHeaderLabels(l);
}

C5LineEdit *C5TableWithTotal::lineEdit(int row, int column)
{
    return ui->tblMain->lineEdit(row, column);
}

void C5TableWithTotal::setWidget(int row, int column, QWidget *w)
{
    ui->tblMain->setCellWidget(row, column, w);
}

bool C5TableWithTotal::findWidget(int &row, int &column, QWidget *w)
{
    for (int r = 0; r < ui->tblMain->rowCount(); r++) {
        for (int c = 0; c < ui->tblMain->columnCount(); c++) {
            if (w == ui->tblMain->cellWidget(r, c)) {
                row = r;
                column = c;
                return true;
            }
        }
    }
    return false;
}

void C5TableWithTotal::clearTables()
{
    ui->tblMain->clearContents();
    ui->tblMain->setRowCount(0);
    for(QMap<int, bool>::const_iterator it = fSumColumns.begin(); it != fSumColumns.end(); it++) {
        if (it.value()) {
            ui->tblTotal->setDouble(0, it.key(), 0);
        }
    }
}

void C5TableWithTotal::moveRowUp(int row)
{
    if (row > 0 && ui->tblMain->rowCount() > 1) {
        replaceRows(row, row - 1);
        ui->tblMain->setCurrentCell(row - 1, 0);
    }
}

void C5TableWithTotal::moveRowDown(int row)
{
    if (row < ui->tblMain->rowCount() - 1) {
        replaceRows(row, row + 1);
        ui->tblMain->setCurrentCell(row + 1, 0);
    }
}

void C5TableWithTotal::replaceRows(int row1, int row2)
{
    for (int i = 0; i < ui->tblMain->columnCount(); i++) {
        QVariant t;
        if (ui->tblMain->cellWidget(row1, i)) {
            if (dynamic_cast<C5LineEdit * >(ui->tblMain->cellWidget(row1, i))) {
                t = ui->tblMain->lineEdit(row1, i)->text();
                ui->tblMain->lineEdit(row1, i)->setText(ui->tblMain->lineEdit(row2, i)->text());
                ui->tblMain->lineEdit(row2, i)->setText(t.toString());
                continue;
            }
        }
        t = ui->tblMain->getData(row1, i);
        ui->tblMain->setData(row1, i, ui->tblMain->getData(row2, i));
        ui->tblMain->setData(row2, i, t);
    }
}

void C5TableWithTotal::mainHeaderResized(int index, int oldsize, int newsize)
{
    Q_UNUSED(oldsize);
    ui->tblTotal->setColumnWidth(index, newsize);
}

void C5TableWithTotal::totalScroll(int value)
{
    ui->tblMain->horizontalScrollBar()->setValue(value);
}
