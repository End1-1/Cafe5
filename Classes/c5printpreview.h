#ifndef C5PRINTPREVIEW_H
#define C5PRINTPREVIEW_H

#include "c5dialog.h"

namespace Ui {
class C5PrintPreview;
}

class C5Printing;

class C5PrintPreview : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5PrintPreview(C5Printing *printData, const QStringList &dbParams);

    ~C5PrintPreview();

    virtual int exec();

private slots:
    void on_cbPrintSelection_currentIndexChanged(int index);

    void on_cbZoom_currentIndexChanged(int index);

    void on_btnPrint_clicked();

    void on_cbPrinters_currentIndexChanged(const QString &arg1);

    void on_btnNext_clicked();

    void on_btnLast_clicked();

    void on_btnBack_clicked();

    void on_btnFirst_clicked();

    void on_btnZoomOut_clicked();

    void on_btnZoopIn_clicked();

private:
    Ui::C5PrintPreview *ui;

    C5Printing *fPrintData;

    int fPageNumber;

    void renderPage();

    void zoom();

    qreal fScaleFactor;
};

#endif // C5PRINTPREVIEW_H
