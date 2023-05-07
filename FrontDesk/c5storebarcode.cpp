#include "c5storebarcode.h"
#include "ui_c5storebarcode.h"
#include "barcode.h"
#include "barcode5.h"
#include "c5storebarcodelist.h"
#include "QRCodeGenerator.h"
#include "c5lineedit.h"
#include "c5checkbox.h"
#include "dlgselectcurrency.h"
#include <QPainter>
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsTextItem>

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

void C5StoreBarcode::addRow(const QString &name, const QString &barcode, int qty, int curr, const QString &sizes)
{
    if (qty == 0) {
        return;
    }
    if (curr == 0) {
        curr = __c5config.getValue(param_default_currency).toInt();
    }
    int row = ui->tbl->rowCount();
    ui->tbl->setRowCount(row + 1);
    ui->tbl->setString(row, 0, name);
    ui->tbl->setString(row, 1, barcode);
    ui->tbl->createLineEdit(row, 2)->setInteger(qty);
    ui->tbl->createCheckbox(row, 3)->setChecked(true);
    C5Database db(fDBParams);
    db[":f_scancode"] = barcode;
    db[":f_currency"] = curr;
    db.exec("select c.*, c1.f_name as f_class1name, gpr.f_price1 "
            "from c_goods c "
            "left join c_goods_classes c1 on c1.f_id=c.f_group1 "
            "left join c_goods_prices gpr on gpr.f_goods=c.f_id "
            "where f_scancode=:f_scancode and gpr.f_currency=:f_currency");
    if (db.nextRow()) {
        ui->tbl->setDouble(row, 4, db.getDouble("f_price1"));
        ui->tbl->setString(row, 5, db.getString("f_description"));
        ui->tbl->setString(row, 6, db.getString("f_class1name"));
        ui->tbl->setString(row, 7, db.getString("f_weblink"));
        ui->tbl->setString(row, 8, sizes.right(sizes.length() - 3));
    } else {
        ui->tbl->setDouble(row, 4, 0);
        ui->tbl->setString(row, 5, "");
        ui->tbl->setString(row, 6, "");
        ui->tbl->setString(row, 7, "");
        ui->tbl->setString(row, 8, "");
    }
}

QToolBar *C5StoreBarcode::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << tbFilter;
        createStandartToolbar(btn);
        fToolBar->addAction(QIcon(":/print_description.png"), tr("Print\nscancodes"), this, SLOT(print()));
        fToolBar->addAction(QIcon(":/print_description.png"), tr("Print\nscancodes 2"), this, SLOT(print2()));
        fToolBar->addAction(QIcon(":/print_description.png"), tr("Print\ndescriptions"), this, SLOT(printDescriptions()));
        fToolBar->addAction(QIcon(":/show_list.png"), tr("Set list"), this, SLOT(setList()));
        fToolBar->addAction(QIcon(":/show_list.png"), tr("1"), this, SLOT(setQtyToOne()));
    }
    return fToolBar;
}

//SAMO XANUT VERSION

bool C5StoreBarcode::printOneBarcode(const QString &code, const QString &price, const QString &class1, const QString &name, QPrintDialog &pd)
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setPrinterName(pd.printer()->printerName());
    printer.setOrientation(pd.printer()->orientation());
    QSizeF szf = printer.pageSizeMM();
    szf = pd.printer()->pageSizeMM();
    //szf.setWidth(400);
    //    szf.setHeight(20);
    printer.setPageSizeMM(szf);
    //    printer.setResolution(pd.printer()->resolution());
    QPrinter::PageSize ps = printer.pageSize();
    ps = pd.printer()->pageSize();
    printer.setPageSize(ps);
    QPainter p(&printer);
    Barcode93 b;
    bool r = b.Encode93(code.toLatin1().data());
    qDebug() << r;
    QFont f(__c5config.getValue(param_app_font_family), 25, QFont::Normal);
    p.setFont(f);
    qreal plen = 2;

    QTextOption to;
    to.setWrapMode(QTextOption::WordWrap);
    f.setPointSize(8);
    f.setBold(true);
    p.setFont(f);
    p.drawText(QRectF(0, 0, 350, 80), name, to);
    b.DrawBarcode(p, 100, 85, 70, 130, plen);
    p.drawText(250, 185, code);
    f.setPointSize(10);
    p.setFont(f);
    p.drawText(5, 185, QString("%1: %2").arg(tr("Price"), price));

    return printer.printerState() != QPrinter::Error;
}


bool C5StoreBarcode::printOneBarcode2(const QString &code, const QString &price, QString link, const QString &name, const QString &sizeList, QPrintDialog &pd)
{
    if (link.isEmpty()) {
        link = " ";
    }

    int levelIndex = 1;
    int versionIndex = 0;
    bool bExtent = true;
    int maskIndex = -1;

    CQR_Encode qrEncode;
    bool successfulEncoding = qrEncode.EncodeData(levelIndex, versionIndex, bExtent, maskIndex, link.toUtf8().data() );
    if (!successfulEncoding) {
        //fLog.append("Cannot encode qr image");
    }
    int qrImageSize = qrEncode.m_nSymbleSize;
    int encodeImageSize = qrImageSize + ( QR_MARGIN * 2 );
    QImage encodeImage(encodeImageSize, encodeImageSize, QImage::Format_Mono);
    encodeImage.fill(1);

    for ( int i = 0; i < qrImageSize; i++ ) {
        for ( int j = 0; j < qrImageSize; j++ ) {
            if ( qrEncode.m_byModuleData[i][j] ) {
                encodeImage.setPixel(i + QR_MARGIN, j + QR_MARGIN, 0);
            }
        }
    }

    QPixmap pix = QPixmap::fromImage(encodeImage);
    pix = pix.scaled(150, 150);

    QPrinter printer(QPrinter::HighResolution);
    printer.setPrinterName(pd.printer()->printerName());
    printer.setOrientation(QPrinter::Landscape);
    QSizeF szf = printer.pageSizeMM();
    szf = pd.printer()->pageSizeMM();
    //szf.setWidth(400);
    //    szf.setHeight(20);
    printer.setPageSizeMM(szf);
    //    printer.setResolution(pd.printer()->resolution());
    QPrinter::PageSize ps = printer.pageSize();
    ps = pd.printer()->pageSize();
    printer.setPageSize(ps);

    QPixmap px(400, 550);
    px.fill(Qt::white);
    QPainter painter(&px);


    // painter.setRenderHint(QPainter::Antialiasing);


    QGraphicsScene gs;
    gs.setSceneRect(0, 0, 400.0, 550.0);
    Barcode128 b;
    qreal plen = 3;
    bool r = b.Encode128C(code.toLatin1().data());
    //b.DrawBarcode(painter, 20, 190, 300, 300, plen);
    qDebug() << b.DrawBarcodeGSLength(0, 150, plen);
    b.DrawBarcodeGS(gs, 200 - (b.DrawBarcodeGSLength(0, 150, plen) / 2), 280, 150, 150, plen);

    QFont font = painter.font();
    font.setPointSize(font.pointSize() + 8);
    painter.setFont(font);
    painter.drawImage(QRectF(80, 0, 150, 150), encodeImage);
    QSet<QString> sizes;
    QStringList t = sizeList.split(" ");
    for (const QString &s: t) {
        sizes.insert(s);
    }

    QStringList sizess = sizes.toList();
    qSort(sizess.begin(), sizess.end());
    int shift = 170 - ((sizess.length() * 65) / 2);

    for (int i = 0; i < sizess.length(); i++) {
        painter.fillRect((i * 50) + 15 + shift, 150, 40, 40, code.mid(0, 2) == sizess.at(i) ? Qt::black : Qt::white);
        if (code.mid(0, 2) == sizess.at(i)) {
            painter.setBrush(Qt::black);
            painter.setPen(Qt::white);
            font.setBold(true);
            painter.setFont(font);
        } else {
            font.setBold(false);
            painter.setFont(font);
            painter.setBrush(Qt::white);
            painter.setPen(Qt::black);
        }
        painter.drawText(QPoint((i * 50) + 20 + shift, 180), sizess.at(i));
    }
    font.setBold(false);
    painter.setFont(font);

    QFontMetrics mf(painter.font());
    QString code2 = QString("%1x%2x%3x%4").arg(code.mid(0, 2), code.mid(2, 3), code.mid(5, 2), code.mid(7, code.length() - 7));
    qDebug() <<  mf.width(code2) << mf.width(price);
    painter.drawText(QPointF(140 - (mf.width(code2) / 2), 380), code2);
    painter.drawText(QPointF(140 - (mf.width(price) / 2), 420), price);


    QPainter pp(&printer);
    pp.drawImage(0, 0, px.toImage());
    gs.render(&pp);
    return printer.printerState() != QPrinter::Error;
}

void C5StoreBarcode::setSearchParameters()
{
    int id;
    QString name;
    if (DlgSelectCurrency::getCurrency(fDBParams, id, name, this) == false) {
        return;
    }
    C5Database db(fDBParams);
    db[":f_currency"] = id;
    db.exec("select g.f_scancode, gpr.f_price1 "
            "from c_goods_prices gpr "
            "left join c_goods g on g.f_id=gpr.f_goods "
            "where gpr.f_currency=:f_currency");
    QMap<QString, double> prices;
    while (db.nextRow()) {
        prices[db.getString("f_scancode")] = db.getDouble("f_price1");
    }
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        double price = prices[ui->tbl->getString(i, 1)];
        ui->tbl->setDouble(i, 4, price);
    }
    db[":f_id"] = id;
    db.exec("select * from e_currency where f_id=:f_id");
    db.nextRow();
    fCurrencyName = db.getString("f_symbol");
}


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
            //VAHE VERSION
            printOneBarcode(code, ui->tbl->getString(i, 4) + " " + fCurrencyName, ui->tbl->getString(i, 6), ui->tbl->getString(i, 0), pd);
            //ELINA VERSION
            //printOneBarcode(code, pd);
            ui->tbl->checkBox(i, 3)->setChecked(false);
        }
    }
}

void C5StoreBarcode::print2()
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
            printOneBarcode2(code, ui->tbl->getString(i, 4) + " " + fCurrencyName,
                             ui->tbl->getString(i, 7), ui->tbl->getString(i, 0),
                             ui->tbl->getString(i, 8), pd);
            //printOneBarcode(code, pd);
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
        for (int i = r1; i < r2; i++) {
            ui->tbl->checkBox(i, 3)->setChecked(true);
        }
    }
}

void C5StoreBarcode::setQtyToOne()
{
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        ui->tbl->lineEdit(i, 2)->setInteger(1);
    }
}


