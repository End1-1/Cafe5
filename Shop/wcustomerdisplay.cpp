#include "wcustomerdisplay.h"
#include "ui_wcustomerdisplay.h"
#include <QApplication>
#include <QCloseEvent>
#include <QScreen>
#include <QWindow>

WCustomerDisplay::WCustomerDisplay(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui(new Ui::WCustomerDisplay)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    ui->tbl->horizontalHeader()->setStretchLastSection(true);
    ui->tbl->setColumnWidth(0, 400);
    ui->tbl->setColumnWidth(1, 100);
    ui->tbl->setColumnWidth(2, 100);
    ui->tbl->setColumnWidth(3, 100);
    ui->tbl->setColumnWidth(4, 100);
}

WCustomerDisplay::~WCustomerDisplay()
{
    delete ui;
}

void WCustomerDisplay::placeOnSecondaryScreen()
{
    const QList<QScreen *> screens = qApp->screens();
    QScreen *target = screens.size() > 1 ? screens.at(1) : (screens.isEmpty() ? nullptr : screens.first());

    // Create native window first, then bind to the target screen (needed for multi-monitor).
    show();
    if (target && windowHandle()) {
        windowHandle()->setScreen(target);
        setGeometry(target->geometry());
    }
    showFullScreen();
    raise();
    activateWindow();
}

void WCustomerDisplay::clear()
{
    ui->tbl->setRowCount(0);
    ui->lbAmount->setText(QStringLiteral("0"));
}

void WCustomerDisplay::addRow(const QString &name, const QString &qty, const QString &price, const QString &total, const QString &discount)
{
    const int r = ui->tbl->rowCount();
    ui->tbl->setRowCount(r + 1);
    ui->tbl->setItem(r, 0, new QTableWidgetItem(name));
    ui->tbl->setItem(r, 1, new QTableWidgetItem(qty));
    ui->tbl->setItem(r, 2, new QTableWidgetItem(price));
    ui->tbl->setItem(r, 3, new QTableWidgetItem(total));
    ui->tbl->setItem(r, 4, new QTableWidgetItem(discount));
}

void WCustomerDisplay::setTotal(const QString &total)
{
    QStringList labels;
    labels.reserve(ui->tbl->rowCount());
    for (int i = 0; i < ui->tbl->rowCount(); ++i) {
        labels.append(QString::number(i + 1));
    }
    ui->tbl->setVerticalHeaderLabels(labels);
    ui->tbl->resizeRowsToContents();
    ui->lbAmount->setText(total);
}

void WCustomerDisplay::closeEvent(QCloseEvent *event)
{
    emit displayClosed();
    QWidget::closeEvent(event);
}
