#pragma once

#include "storeinputxmlimport.h"
#include <QDialog>
#include <QVector>

class QCheckBox;

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

    QVector<StoreInputXmlInvoice> selectedInvoices() const;

private slots:
    void tryAccept();
    void selectAll(bool checked);
    void onInvoiceDoubleClicked(int row, int column);
    void onSearchTextChanged(const QString &text);

private:
    Ui::DlgStoreInputXmlInvoicePick *ui;

    QVector<StoreInputXmlInvoice> mInvoices;

    QVector<QCheckBox *> mChecks;
};
