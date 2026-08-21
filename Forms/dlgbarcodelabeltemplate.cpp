#include "dlgbarcodelabeltemplate.h"
#include "ui_dlgbarcodelabeltemplate.h"
#include "c5storebarcode.h"
#include <QListWidgetItem>
#include <QFont>

DlgBarcodeLabelTemplate::DlgBarcodeLabelTemplate(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgBarcodeLabelTemplate)
{
    ui->setupUi(this);
    setWindowTitle(tr("Barcode label template"));

    auto *item60 = new QListWidgetItem(QStringLiteral("60 × 30"), ui->lstTemplates);
    item60->setData(Qt::UserRole, QString::fromLatin1(C5StoreBarcode::kTemplate60x30));
    auto *item57 = new QListWidgetItem(QStringLiteral("57 × 30 QR"), ui->lstTemplates);
    item57->setData(Qt::UserRole, QString::fromLatin1(C5StoreBarcode::kTemplate57x30Qr));

    const QString current = C5StoreBarcode::currentLabelTemplateId();
    for(int i = 0; i < ui->lstTemplates->count(); ++i) {
        QListWidgetItem *it = ui->lstTemplates->item(i);
        if(it && it->data(Qt::UserRole).toString() == current) {
            ui->lstTemplates->setCurrentItem(it);
            break;
        }
    }
    if(!ui->lstTemplates->currentItem() && ui->lstTemplates->count() > 0) {
        ui->lstTemplates->setCurrentRow(0);
    }
    ui->cbFont->setCurrentFont(QFont(C5StoreBarcode::currentLabelFontFamily()));
}

DlgBarcodeLabelTemplate::~DlgBarcodeLabelTemplate()
{
    delete ui;
}

bool DlgBarcodeLabelTemplate::selectTemplate(QWidget *parent)
{
    DlgBarcodeLabelTemplate dlg(parent);
    return dlg.exec() == QDialog::Accepted;
}

void DlgBarcodeLabelTemplate::on_btnOk_clicked()
{
    QListWidgetItem *it = ui->lstTemplates->currentItem();
    if(!it) {
        return;
    }
    C5StoreBarcode::setCurrentLabelTemplateId(it->data(Qt::UserRole).toString());
    C5StoreBarcode::setCurrentLabelFontFamily(ui->cbFont->currentFont().family());
    accept();
}

void DlgBarcodeLabelTemplate::on_btnCancel_clicked()
{
    reject();
}
