#include "c5tablewithtotal.h"
#include "ui_c5tablewithtotal.h"
#include "c5lineedit.h"
#include "c5message.h"
#include "xlsxall.h"
#include <QScrollBar>
#include <QDebug>

C5TableWithTotal::C5TableWithTotal(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::C5TableWithTotal)
{
    ui->setupUi(this);
    QStringList vl;
    vl.append("");
    ui->tblTotal->setVerticalHeaderLabels(vl);
    connect(ui->tblMain->horizontalHeader(), SIGNAL(sectionResized(int,int,int)), this, SLOT(mainHeaderResized(int,int,int)));
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
    XlsxDocument d;
    XlsxSheet *s = d.workbook()->addSheet("Sheet1");
    /* HEADER */
    QColor color = QColor::fromRgb(200, 200, 250);
    QFont headerFont(qApp->font());
    headerFont.setBold(true);
    d.style()->addFont("header", headerFont);
    d.style()->addBackgrounFill("header", color);
    for (int i = 0; i < columnCount(); i++) {
        s->addCell(1, i + 1, columnTitle(i), d.style()->styleNum("header"));
        s->setColumnWidth(i + 1, columnWidth(i) / 7);
    }

    /* BODY */
    QMap<int, QString> bgFill;
    QFont bodyFont(qApp->font());
    d.style()->addFont("body", bodyFont);
    d.style()->addBackgrounFill("body", QColor(Qt::white));
    bgFill[QColor(Qt::white).rgb()] = "body";
    for (int j = 0; j < rowCount(); j++) {
        for (int i = 0; i < columnCount(); i++) {
            int bgColor = getData(j, i, Qt::BackgroundColorRole).value<QColor>().rgb();
            bgColor = QColor::fromRgb(255, 255, 255).rgb();
            if (!bgFill.contains(bgColor)) {
                d.style()->addFont(QString::number(bgColor), bodyFont);
                d.style()->addBackgrounFill(QString::number(bgColor), QColor::fromRgb(bgColor));
                bgFill[bgColor] = QString::number(bgColor);
            }
            QString bgStyle = bgFill[bgColor];
            s->addCell(j + 2, i + 1, getData(j, i, Qt::EditRole), d.style()->styleNum(bgStyle));
        }
    }

    /* TOTALS ROWS */
    QFont totalFont(qApp->font());
    totalFont.setBold(true);
    d.style()->addFont("footer", headerFont);
    color = QColor::fromRgb(193, 206, 221);
    d.style()->addBackgrounFill("footer", color);
    for (int i = 0; i < columnCount(); i++) {
        s->addCell(1 + rowCount() + 1, i + 1, totalStr(i), d.style()->styleNum("footer"));
    }

    QString err;
    if (!d.save(err, true)) {
        if (!err.isEmpty()) {
            C5Message::error(err);
        }
    }
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

C5TableWithTotal &C5TableWithTotal::setData(int &row, int column, const QVariant &data, bool newrow)
{
    if (newrow) {
        row = addRow();
    }
    ui->tblMain->setData(row, column, data);
    if (fSumColumns[column]) {
        ui->tblTotal->setDouble(0, column, ui->tblMain->sumOfColumn(column));
    }
    return *this;
}

C5TableWithTotal &C5TableWithTotal::createLineEdit(int &row, int column, const QVariant &data, QObject *obj, const char *slot, bool newrow)
{
    setData(row, column, data, newrow);
    ui->tblMain->createLineEdit(row, column)->setData(data);
    if (slot) {
        connect(ui->tblMain->lineEdit(row, column), SIGNAL(textChanged(QString)), obj, slot);
    }
    return *this;
}

C5LineEdit *C5TableWithTotal::lineEdit(int row, int column)
{
    return ui->tblMain->lineEdit(row, column);
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
        qDebug() << ui->tblMain->getData(row1, i) << ui->tblMain->getData(row2, i);
        QVariant t;
        if (ui->tblMain->cellWidget(row1, i)) {
            if (dynamic_cast<C5LineEdit*>(ui->tblMain->cellWidget(row1, i))) {
                t = ui->tblMain->lineEdit(row1, i)->text();
                ui->tblMain->lineEdit(row1, i)->setText(ui->tblMain->lineEdit(row2, i)->text());
                ui->tblMain->lineEdit(row2, i)->setText(t.toString());
                continue;
            }
        }
        t = ui->tblMain->getData(row1, i);
        ui->tblMain->setData(row1, i, ui->tblMain->getData(row2, i));
        ui->tblMain->setData(row2, i, t);
        qDebug() << ui->tblMain->getData(row1, i) << ui->tblMain->getData(row2, i);
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
