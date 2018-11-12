#include "c5printpreview.h"
#include "ui_c5printpreview.h"
#include "c5printing.h"
#include <QPrinterInfo>
#include <QMatrix>

C5PrintPreview::C5PrintPreview(C5Printing *printData, const QStringList &dbParams, QWidget *parent) :
    C5Dialog(dbParams, parent),
    ui(new Ui::C5PrintPreview)
{
    ui->setupUi(this);
    fPageNumber = 1;
    fScaleFactor = 0.0;
    fPrintData = printData;
    QString lastPrinter = C5Config::getRegValue("last_printer").toString();
    ui->cbPrinters->addItems(QPrinterInfo::availablePrinterNames());
    ui->cbPrinters->setCurrentIndex(ui->cbPrinters->findText(lastPrinter));
    if (ui->cbPrinters->currentIndex() < 0) {
        ui->cbPrinters->setCurrentIndex(ui->cbPrinters->findText(QPrinterInfo::defaultPrinterName()));
    }
}

C5PrintPreview::~C5PrintPreview()
{
    delete ui;
}

int C5PrintPreview::exec()
{
    showMaximized();
    zoom();
    renderPage();
    return C5Dialog::exec();
}

void C5PrintPreview::renderPage()
{
    ui->lbPage->setText(QString("%1 %2 %3 %4")
                        .arg(tr("Page"))
                        .arg(fPageNumber)
                        .arg(tr("of"))
                        .arg(fPrintData->pageCount()));
    QGraphicsScene *s = fPrintData->page(fPageNumber - 1);
    ui->gv->setScene(s);
    ui->btnFirst->setEnabled(fPageNumber > 1);
    ui->btnBack->setEnabled(fPageNumber > 1);
    ui->btnLast->setEnabled(fPageNumber < fPrintData->pageCount());
    ui->btnNext->setEnabled(fPageNumber <fPrintData->pageCount());
}

void C5PrintPreview::zoom()
{
    qreal z = ui->cbZoom->currentText().remove(ui->cbZoom->currentText().length() - 1, 1).toInt();
    z /= 30.0;
    QSizeF s(fPrintData->orientation(fPageNumber - 1) == QPrinter::Portrait ? QSizeF(200.0, 280.0) : QSizeF(280.0, 200.0));
    s.setWidth(s.width() * z);
    s.setHeight(s.height() * z);
    ui->gv->setMinimumSize(s.toSize());
    ui->gv->setMaximumSize(s.toSize());
    z /= 3.35;
    if (fScaleFactor > 0.0) {
        ui->gv->scale(1 / fScaleFactor, 1 / fScaleFactor);
    }
    fScaleFactor = z;
    ui->gv->scale(z, z);
}

void C5PrintPreview::on_cbPrintSelection_currentIndexChanged(int index)
{
    ui->lePages->setEnabled(index == 2);
}

void C5PrintPreview::on_cbZoom_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    zoom();
}

void C5PrintPreview::on_btnPrint_clicked()
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setPrinterName(ui->cbPrinters->currentText());
    QPrinter::Margins margins;
    margins.left = 0;
    margins.top = 0;
    margins.bottom = 0;
    margins.right = 0;
    printer.setMargins(margins);
    QPainter painter(&printer);
    QMatrix matrix;
    qreal scale = (static_cast<double>(printer.resolution()) / static_cast<double>(fPrintData->resolution())) * 0.8;
    matrix.scale(scale, scale);
    painter.setMatrix(matrix);
    for (int i = 0; i < fPrintData->pageCount(); i++) {
        printer.setOrientation(fPrintData->orientation(i));
        if (i > 0) {
            printer.newPage();
        }
        QGraphicsScene *s = fPrintData->page(i);
        qDebug() << s->sceneRect().size();
        s->render(&painter);
    }
}

void C5PrintPreview::on_cbPrinters_currentIndexChanged(const QString &arg1)
{
    C5Config::setRegValue("last_printer", arg1);
}

void C5PrintPreview::on_btnNext_clicked()
{
    if (fPageNumber < fPrintData->pageCount() + 1) {
        fPageNumber++;
        renderPage();
    }
}

void C5PrintPreview::on_btnLast_clicked()
{
    fPageNumber = fPrintData->pageCount();
    renderPage();
}

void C5PrintPreview::on_btnBack_clicked()
{
    if (fPageNumber > 1) {
        fPageNumber--;
        renderPage();
    }
}

void C5PrintPreview::on_btnFirst_clicked()
{
    fPageNumber = 1;
    renderPage();
}

void C5PrintPreview::on_btnZoomOut_clicked()
{
    int index = ui->cbZoom->currentIndex();
    if (index < ui->cbZoom->count() - 1) {
        index++;
        ui->cbZoom->setCurrentIndex(index);
    }
}

void C5PrintPreview::on_btnZoopIn_clicked()
{
    int index = ui->cbZoom->currentIndex();
    if (index > 0) {
        index--;
        ui->cbZoom->setCurrentIndex(index);
    }
}
