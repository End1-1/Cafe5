#include "c5storebarcode.h"
#include "ui_c5storebarcode.h"
#include "barcode5.h"
#include "c5storebarcodelist.h"
#include "c5lineedit.h"
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
    ui->tbl->createLineEdit(row, 2)->setInteger(qty);
    ui->tbl->createCheckbox(row, 3)->setChecked(true);
}

QToolBar *C5StoreBarcode::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbPrint;
        createStandartToolbar(btn);
        fToolBar->addAction(QIcon(":/show_list.png"), tr("Set list"), this, SLOT(setList()));
    }
    return fToolBar;
}

bool C5StoreBarcode::printOneBarcode(const QString &code, const QString &printerName)
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setPrinterName(printerName);
    printer.setOrientation(QPrinter::Portrait);
    printer.setPageSize(QPrinter::Custom);
    QPainter p(&printer);
    p.rotate(90);
    p.translate(0, -420);
    QFont f("ArTarumianHandes", 40, QFont::Normal);
    p.setFont(f);
    p.drawText(QPoint(40, 50), "elina");
    f.setFamily("Arial");
    f.setPointSize(14);
    p.setFont(f);
    p.drawText(QPoint(20, 110), "MODEL: ");
    f.setBold(true);
    f.setPointSize(16);
    p.setFont(f);
    p.drawText(QPoint(20, 110), "             " + code.right(code.length() - 7));
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
        f.setBold(false);
        f.setPointSize(14);
        p.setFont(f);
        p.drawText(QPoint(20, 170), "SIZE: ");
        f.setBold(true);
        f.setPointSize(16);
        p.setFont(f);
        p.drawText(QPoint(20, 170), "             " + code.left(2));
    }
//    Barcode39 b;
//    b.Encode39(code.toLatin1().data());
    Barcode93 b;
    b.Encode93(code.toLatin1().data());
//    Barcode128 b;
//    b.Encode128A(code.toLatin1().data());
    int of = 0;
    switch (code.length()) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        of = 80;
        break;
    case 7:
    case 8:
    case 9:
        of = 50;
        break;
    case 10:
        of = 40;
        break;
    default:
        of = 30;
        break;
    }

    qreal plen = 2;
    b.DrawBarcode(p, of, 210, 270, 270, plen);

    f.setBold(true);
    f.setPointSize(10);
    p.setFont(f);
    QString code2 = QString("%1x%2x%3x%4")
            .arg(code.left(2))
            .arg(code.mid(2,3))
            .arg(code.mid(5,2))
            .arg(code.right(code.length() - 7));
    p.drawText(QPoint(60, 350), code2);
    p.drawText(QPoint(45, 400), "MADE IN ARMENIA");
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
        for (int j = 0; j < ui->tbl->lineEdit(i, 2)->getInteger(); j++) {
            printOneBarcode(ui->tbl->getString(i, 1), pd.printer()->printerName());
            ui->tbl->checkBox(i, 3)->setChecked(false);
        }
    }
}

void C5StoreBarcode::setList()
{
    int r1, r2 = ui->tbl->rowCount();
    if (C5StoreBarcodeList::getList(r1, r2)) {
        for (int i = 0; i < ui->tbl->rowCount(); i++) {
            ui->tbl->checkBox(i, 3)->setChecked(false);
        }
        for (int i = r1 - 1; i < r2; i++) {
            ui->tbl->checkBox(i, 3)->setChecked(true);
        }
    }
}