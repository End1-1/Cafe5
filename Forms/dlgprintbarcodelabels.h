#ifndef DLGPRINTBARCODELABELS_H
#define DLGPRINTBARCODELABELS_H

#include <QDialog>
#include <QList>

namespace Ui
{
class DlgPrintBarcodeLabels;
}

struct BarcodeLabelItem {
    QString name;
    QString barcode;
    int qty = 0;
    QString price;
};

class DlgPrintBarcodeLabels : public QDialog
{
    Q_OBJECT

public:
    explicit DlgPrintBarcodeLabels(QWidget *parent = nullptr);
    ~DlgPrintBarcodeLabels();

    static bool printLabels(QWidget *parent, const QList<BarcodeLabelItem> &items);

private slots:
    void on_btnPrint_clicked();
    void on_btnCancel_clicked();

private:
    Ui::DlgPrintBarcodeLabels *ui;

    void setItems(const QList<BarcodeLabelItem> &items);
    bool doPrint();
};

#endif // DLGPRINTBARCODELABELS_H
