#include "previewdialog.h"

#include <QDir>
#include <QGraphicsPixmapItem>
#include <QPainter>
#include <QPdfDocument>
#include <QPicture>
#include <QPrintDialog>

PrintPreviewDialog::PrintPreviewDialog(QTextDocument *doc, QPrinter *printer, QWidget *parent)
    : QDialog(parent)
    , m_doc(doc)
    , m_view(new QGraphicsView(this))
    , m_scene(new QGraphicsScene(this))
    , m_printer(printer)
{
    setWindowTitle("Preview");
    resize(900, 700);

    m_view->setScene(m_scene);

    auto *printBtn = new QPushButton("Print");

    connect(printBtn, &QPushButton::clicked, this, &PrintPreviewDialog::slotPrint);

    QVBoxLayout *vl = new QVBoxLayout(this);
    vl->addWidget(m_view);

    QHBoxLayout *hl = new QHBoxLayout();
    hl->addStretch();
    hl->addWidget(printBtn);

    vl->addLayout(hl);

    m_view->setResizeAnchor(QGraphicsView::NoAnchor);
    m_view->setTransformationAnchor(QGraphicsView::NoAnchor);
    m_view->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    m_view->setCacheMode(QGraphicsView::CacheBackground);
    renderPages();
}

void PrintPreviewDialog::renderPages()
{
    m_scene->clear();

    QString pdfPath = QDir::tempPath() + "/preview_tmp.pdf";

    QPrinter pdfPrinter(QPrinter::HighResolution);

    pdfPrinter.setPageLayout(m_printer->pageLayout());
    pdfPrinter.setResolution(m_printer->resolution());

    pdfPrinter.setOutputFormat(QPrinter::PdfFormat);
    pdfPrinter.setOutputFileName(pdfPath);

    m_doc->print(&pdfPrinter);

    QPdfDocument pdf;
    if (pdf.load(pdfPath) != QPdfDocument::Error::None)
        return;

    int yOffset = 0;
    int dpi = 150;

    for (int i = 0; i < pdf.pageCount(); ++i) {
        QSizeF ptSize = pdf.pagePointSize(i);

        QSize renderSize(ptSize.width() * dpi / 72, ptSize.height() * dpi / 72);

        QImage img = pdf.render(i, renderSize, QPdfDocumentRenderOptions());

        m_scene->addPixmap(QPixmap::fromImage(img))->setPos(0, yOffset);

        yOffset += img.height() + 20;
    }

    m_view->setSceneRect(m_scene->itemsBoundingRect());
    m_view->resetTransform();
}

void PrintPreviewDialog::slotPrint()
{
    QPrintDialog dlg(m_printer, this);

    if (dlg.exec() == QDialog::Accepted) {
        m_doc->print(m_printer);
    }
}
