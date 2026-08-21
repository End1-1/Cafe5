#pragma once

#include "storeinputxmlimport.h"
#include <QDialog>

namespace Ui {
class DlgStoreInputXmlInvoicePreview;
}

class DlgStoreInputXmlInvoicePreview : public QDialog
{
    Q_OBJECT

public:
    explicit DlgStoreInputXmlInvoicePreview(const StoreInputXmlInvoice &invoice, QWidget *parent = nullptr);
    ~DlgStoreInputXmlInvoicePreview() override;

    static void showInvoice(const StoreInputXmlInvoice &invoice, QWidget *parent = nullptr);

private:
    Ui::DlgStoreInputXmlInvoicePreview *ui;
};
