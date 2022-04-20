#include "tablewidget.h"
#include "ui_tablewidget.h"
#include "tablemodel.h"
#include "rawmessage.h"
#include "socketconnection.h"
#include "messagelist.h"
#include <QScrollBar>

TableWidget::TableWidget(QWidget *parent) :
    SocketWidget(parent),
    ui(new Ui::TableWidget)
{
    ui->setupUi(this);
    fTableModel = new TableModel();
    ui->view->setModel(fTableModel);
    ui->totals->setVerticalHeaderLabels(QStringList() << "");
    connect(ui->view->horizontalHeader(), &QHeaderView::sectionResized,  this, &TableWidget::resizeTotalHeader);
    connect(ui->totals->horizontalScrollBar(), &QScrollBar::valueChanged, this, &TableWidget::changeViewHorizontalScroll);

    RawMessage r(nullptr);
    r.setHeader(SocketConnection::instance()->getTcpPacketNumber(), 0, MessageList::dll_op);
    r.putString("rwjzstore");
    r.putUShort(2022);
    r.putUByte(3);
    r.putUByte(2);
    r.putUByte(2);
    emit dataReady(r.data());
}

TableWidget::~TableWidget()
{
    delete ui;
}

void TableWidget::externalDataReceived(quint16 cmd, const QByteArray &data)
{
    fTableModel->setSourceData(data);
    QStringList footer;
    footer.append(QString::number(fTableModel->rowCount()));
    ui->totals->setVerticalHeaderLabels(footer);
    ui->totals->clearContents();
    ui->totals->setColumnCount(fTableModel->columnCount());
    for (QMap<int, double>::const_iterator it = fTableModel->fSumOfColumns.constBegin(); it != fTableModel->fSumOfColumns.constEnd(); it++) {
        ui->totals->setItem(0, it.key(), new QTableWidgetItem(float_str(it.value(), 3)));
    }
    for (int i = 0; i < fTableModel->columnCount(); i++) {
        ui->view->setColumnWidth(i, fTableModel->columnWidth(i));
    }
}

void TableWidget::resizeTotalHeader(int logicalIndex, int oldSize, int newSize)
{
    Q_UNUSED(oldSize);
    ui->totals->setColumnWidth(logicalIndex, newSize);
}

void TableWidget::changeViewHorizontalScroll(int value)
{
    ui->view->horizontalScrollBar()->setValue(value);
}

void TableWidget::on_leQuickFilter_textChanged(const QString &arg1)
{
    fTableModel->quickFilter(arg1);
}
