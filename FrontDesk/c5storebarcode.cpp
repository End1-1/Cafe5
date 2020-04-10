#include "c5storebarcode.h"
#include "ui_c5storebarcode.h"
#include "barcode5.h"
#include "c5checkbox.h"
#include <QPainter>
#include <QPrintDialog>

C5StoreBarcode::C5StoreBarcode(const QStringList &dbParams, QWidget *parent) :
    C5Widget(dbParams, parent),
    ui(new Ui::C5StoreBarcode)
{
    ui->setupUi(this);
    fIcon = ":/barcode.png";
    fLabel = tr("Barcode print");
    ui->tbl->setColumnWidths(4, 300, 300, 80, 30);
}

C5StoreBarcode::~C5StoreBarcode()
{
    delete ui;
}

void C5StoreBarcode::addRow(const QString &name, const QString &barcode, int qty)
{
    if (qty == 0) {
        return;
    }
    int row = ui->tbl->rowCount();
    ui->tbl->setRowCount(row + 1);
    ui->tbl->setString(row, 0, name);
    ui->tbl->setString(row, 1, barcode);
    ui->tbl->setInteger(row, 2, qty);
    ui->tbl->createCheckbox(row, 3)->setChecked(true);
}

QToolBar *C5StoreBarcode::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbPrint;
        createStandartToolbar(btn);
    }
    return fToolBar;
}

bool C5StoreBarcode::printOneBarcode(const QString &code, const QString &printerName)
{
    QPrinter printer(QPrinter::PrinterResolution);
    printer.setPrinterName(printerName);
    printer.setOrientation(QPrinter::Portrait);
    printer.setPageSize(QPrinter::Custom);
    QPainter p(&printer);
    p.rotate(90);
    p.translate(0, -420);
    QFont f("Arial", 32, QFont::Normal);
    p.setFont(f);
    p.drawText(QPoint(70, 50), "elina");
    f.setPointSize(14);
    p.setFont(f);
    p.drawText(QPoint(20, 110), "MODEL: ");
    f.setBold(true);
    p.setFont(f);
    p.drawText(QPoint(20, 110), "               " + code.right(code.length() - 7));
    f.setBold(false);
    p.setFont(f);
    if (code.left(2).toInt() < 20) {
        f.setPointSize(9);
        p.setFont(f);
        QMap<QString, QString> s;
        s["01"] = "34 36 38";
        s["02"] = "34 36 38 40";
        s["03"] = "34 36 38 40 42";
        s["04"] = "36 36 38 40";
        s["05"] = "36 38 40";
        s["06"] = "36 38 40 42";
        s["07"] = "36 38 40 42 44";
        s["08"] = "38 40 42 44";
        s["09"] = "38 40 42 46";
        s["10"] = "40 42 44 46";
        s["11"] = "42 44 46";
        s["12"] = "46 48 50";
        QStringList sizes = s[code.left(2)].split(" ", QString::SkipEmptyParts);
        for (int i = 0; i < sizes.count(); i++) {
            p.drawText(QPoint(((i + 1) * 35) - 15, 150), sizes.at(i));
            p.drawText(QPoint(((i + 1) * 35) - 15, 190), "1");
        }
        p.drawText(QPoint(190, 190), QString("pieces: %1").arg(sizes.count()));
    } else {
        p.drawText(QPoint(20, 170), "SIZE: ");
        f.setBold(true);
        p.setFont(f);
        p.drawText(QPoint(20, 170), "             " + code.left(2));
    }
    BarcodeI2of5 b;
    b.EncodeI2of5(code.toLatin1().data());
    b.DrawBarcode(p, 50, 210, 290, 290);
    f.setBold(true);
    f.setPointSize(10);
    p.setFont(f);
    QString code2 = QString("%1x%2x%3x%4")
            .arg(code.left(2))
            .arg(code.mid(2,3))
            .arg(code.mid(5,2))
            .arg(code.right(code.length() - 7));
    p.drawText(QPoint(20, 350), code2);
    p.drawText(QPoint(20, 400), "MADE IN ARMENIA");
    return printer.printerState() != QPrinter::Error;
}

void C5StoreBarcode::print()
{
    QPrintDialog pd;
    if (pd.exec() == QDialog::Accepted) {

    } else {
        return;
    }
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        if (!ui->tbl->checkBox(i, 3)->isChecked()) {
            continue;
        }
        for (int j = 0; j < ui->tbl->getInteger(i, 2); j++) {
            printOneBarcode(ui->tbl->getString(i, 1), pd.printer()->printerName());
            ui->tbl->checkBox(i, 3)->setChecked(false);
        }
    }
}
