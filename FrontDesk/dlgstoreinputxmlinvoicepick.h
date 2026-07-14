#pragma once

#include "storeinputxmlimport.h"
#include <QDialog>

namespace Ui
{
class DlgStoreInputXmlInvoicePick;
}

class DlgStoreInputXmlInvoicePick : public QDialog
{
    Q_OBJECT

public:
    explicit DlgStoreInputXmlInvoicePick(const QVector<StoreInputXmlInvoice> &invoices, QWidget *parent = nullptr);

    ~DlgStoreInputXmlInvoicePick() override;

    StoreInputXmlInvoice selectedInvoice() const;

private slots:
    void tryAccept();

private:
    Ui::DlgStoreInputXmlInvoicePick *ui;

    QVector<StoreInputXmlInvoice> mInvoices;
};
