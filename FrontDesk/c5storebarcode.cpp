#include "c5storebarcode.h"
#include "ui_c5storebarcode.h"
#include "barcode.h"
#include "barcode5.h"
#include "c5storebarcodelist.h"
#include "c5lineedit.h"
#include "c5checkbox.h"
#include <QPainter>
#include <QDebug>

C5StoreBarcode::C5StoreBarcode(const QStringList &dbParams, QWidget *parent) :
    C5Widget(dbParams, parent),
    ui(new Ui::C5StoreBarcode)
{
    ui->setupUi(this);
    fIcon = ":/barcode.png";
    fLabel = tr("Barcode print");
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 300, 300, 80, 30, 80, 200, 100);
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
    C5Database db(fDBParams);
    db[":f_scancode"] = barcode;
    db.exec("select c.*, c1.f_name as f_class1name from c_goods c "
            "left join c_goods_classes c1 on c1.f_id=c.f_group1 "
            "where f_scancode=:f_scancode");
    if (db.nextRow()) {
        ui->tbl->setDouble(row, 4, db.getDouble("f_saleprice"));
        ui->tbl->setString(row, 5, db.getString("f_description"));
        ui->tbl->setString(row, 6, db.getString("f_class1name"));
    } else {
        ui->tbl->setDouble(row, 4, 0);
        ui->tbl->setString(row, 5, "");
        ui->tbl->setString(row, 6, "");
    }
}

QToolBar *C5StoreBarcode::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        createStandartToolbar(btn);
        fToolBar->addAction(QIcon(":/print_description.png"), tr("Print\nscancodes"), this, SLOT(print()));
        fToolBar->addAction(QIcon(":/print_description.png"), tr("Print\ndescriptions"), this, SLOT(printDescriptions()));
        fToolBar->addAction(QIcon(":/show_list.png"), tr("Set list"), this, SLOT(setList()));
    }
    return fToolBar;
}

bool C5StoreBarcode::printOneBarcode(const QString &code, const QString &price, const QString &class1, QPrintDialog &pd)
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setPrinterName(pd.printer()->printerName());
    printer.setOrientation(pd.printer()->orientation());
    QSizeF szf = printer.pageSizeMM();
    szf = pd.printer()->pageSizeMM();
    szf.setWidth(300);
//    szf.setHeight(20);
    printer.setPageSizeMM(szf);
//    printer.setResolution(pd.printer()->resolution());
    QPrinter::PageSize ps = printer.pageSize();
    ps = pd.printer()->pageSize();
    printer.setPageSize(ps);
    QPainter p(&printer);
    BarcodeEan13 b;
    bool r = b.EncodeEan13(code.toLatin1().data());
    Barcode bb;
    qDebug() << r;
    QFont f("Arial", 25, QFont::Normal);
    p.setFont(f);
    qreal plen = 2;

    f.setPointSize(8);
    f.setBold(true);
    p.setFont(f);
    b.DrawBarcode(p, 100, 40, 100, 100, plen);
    p.drawText(110, 120, code + QString::number(bb.ean13CheckSum(code)));
    f.setPointSize(10);
    p.setFont(f);
    p.drawText(110, 150, price + " AMD");
    p.drawText(160, 35, class1);

    return printer.printerState() != QPrinter::Error;
}

/*
//ELINA VERSION
bool C5StoreBarcode::printOneBarcode(const QString &code, QPrintDialog &pd)
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setPrinterName(pd.printer()->printerName());
    printer.setOrientation(pd.printer()->orientation());
    QSizeF szf = printer.pageSizeMM();
    szf = pd.printer()->pageSizeMM();
    printer.setPageSizeMM(szf);
    QPrinter::PageSize ps = printer.pageSize();
    ps = pd.printer()->pageSize();
    printer.setPageSize(ps);
    QPainter p(&printer);
    Barcode93 b;
    b.Encode93(code.toLatin1().data());
    QFont f("ArTarumianHandes", 40, QFont::Normal);
    p.setFont(f);
    qreal plen = 2;
    if (code.length() == 4) {
        plen = 2.5;
        f.setPointSize(14);
        f.setBold(true);
        p.setFont(f);
        b.DrawBarcode(p, 150, 60, 180, 180, plen);
        p.drawText(160, 220, code);
        return printer.printerState() != QPrinter::Error;
    }
    p.rotate(90);
    p.translate(0, -420);
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
        of = 20;
        break;
    }
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
*/

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
        Barcode bc;
        for (int j = 0; j < ui->tbl->lineEdit(i, 2)->getInteger(); j++) {
            QString code = ui->tbl->getString(i, 1);
            if (bc.isEan13(code)) {
                code = code.left(code.length() - 1);
            }
            printOneBarcode(code, ui->tbl->getString(i, 4), ui->tbl->getString(i, 6), pd);
            ui->tbl->checkBox(i, 3)->setChecked(false);
        }
    }
}

void C5StoreBarcode::printDescriptions()
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
            QPrinter printer(QPrinter::HighResolution);
            printer.setPrinterName(pd.printer()->printerName());
            printer.setOrientation(pd.printer()->orientation());
            QSizeF szf = printer.pageSizeMM();
            szf = pd.printer()->pageSizeMM();
            szf.setWidth(250);
        //    szf.setHeight(20);
            printer.setPageSizeMM(szf);
            QPrinter::PageSize ps = printer.pageSize();
            ps = pd.printer()->pageSize();
            printer.setPageSize(ps);
            QPainter p(&printer);
            QFont f("Arial", 6, QFont::Normal);
            p.setFont(f);
            QRectF tr(80, 20, 200, 200);
            QTextOption to;
            to.setWrapMode(QTextOption::WordWrap);
            p.drawText (tr, ui->tbl->getString(i, 5), to);
        }
        ui->tbl->checkBox(i, 3)->setChecked(false);
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
