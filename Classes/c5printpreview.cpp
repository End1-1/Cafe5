#include "c5printpreview.h"
#include "ui_c5printpreview.h"
#include "c5printing.h"
#include <QPrinterInfo>
#include <QGraphicsScene>

C5PrintPreview::C5PrintPreview(C5Printing *printData, const QStringList &dbParams) :
    C5Dialog(dbParams),
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
    ui->btnNext->setEnabled(fPageNumber < fPrintData->pageCount());
}

void C5PrintPreview::zoom()
{
    if (fScaleFactor > 0.0) {
        ui->gv->scale(1 / fScaleFactor, 1 / fScaleFactor);
    }
    qreal z = ui->cbZoom->currentText().remove(ui->cbZoom->currentText().length() - 1, 1).toInt() / 100.0;
    z /= 2.5;
    fScaleFactor = z;
    QRectF r(fPrintData->page(fPageNumber - 1)->sceneRect());
    r.setWidth(r.width() *z);
    r.setHeight(r.height() *z);
    ui->gv->setMinimumSize(r.size().toSize());
    ui->gv->setMaximumSize(r.size().toSize());
    ui->gv->scale(fScaleFactor, fScaleFactor);
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
    fPrintData->print(ui->cbPrinters->currentText(), QPageSize(QPageSize::A4));
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
