#include "dlgorderdatamatrix.h"
#include "ui_dlgorderdatamatrix.h"
#include "c5message.h"
#include <QTimer>

DlgOrderDatamatrix::DlgOrderDatamatrix(C5User *user, const QStringList &codes, QWidget *parent)
    : C5Dialog(user, parent)
    , ui(new Ui::DlgOrderDatamatrix)
{
    ui->setupUi(this);
    ui->lst->setFocusPolicy(Qt::ClickFocus);
    for (const QString &code : codes) {
        const QString trimmed = code.trimmed();
        if (trimmed.isEmpty()) {
            continue;
        }
        ui->lst->addItem(trimmed);
    }
    focusCodeInput();
}

DlgOrderDatamatrix::~DlgOrderDatamatrix()
{
    delete ui;
}

QStringList DlgOrderDatamatrix::codes() const
{
    QStringList result;
    for (int i = 0; i < ui->lst->count(); ++i) {
        result.append(ui->lst->item(i)->text());
    }
    return result;
}

bool DlgOrderDatamatrix::editCodes(C5User *user, QStringList &codes, QWidget *parent)
{
    DlgOrderDatamatrix dlg(user, codes, parent);
    if (dlg.exec() != QDialog::Accepted) {
        return false;
    }
    codes = dlg.codes();
    return true;
}

void DlgOrderDatamatrix::focusCodeInput()
{
    QTimer::singleShot(0, this, [this]() {
        ui->leCode->setFocus(Qt::OtherFocusReason);
    });
}

void DlgOrderDatamatrix::addCode(const QString &raw)
{
    const QString code = raw.trimmed();
    if (code.isEmpty()) {
        focusCodeInput();
        return;
    }
    for (int i = 0; i < ui->lst->count(); ++i) {
        if (ui->lst->item(i)->text().compare(code, Qt::CaseSensitive) == 0) {
            ui->leCode->clear();
            focusCodeInput();
            return;
        }
    }
    ui->lst->addItem(code);
    ui->leCode->clear();
    focusCodeInput();
}

void DlgOrderDatamatrix::on_leCode_returnPressed()
{
    addCode(ui->leCode->text());
}

void DlgOrderDatamatrix::on_btnAdd_clicked()
{
    addCode(ui->leCode->text());
}

void DlgOrderDatamatrix::on_btnRemove_clicked()
{
    const QList<QListWidgetItem *> selected = ui->lst->selectedItems();
    for (QListWidgetItem *item : selected) {
        delete ui->lst->takeItem(ui->lst->row(item));
    }
    focusCodeInput();
}

void DlgOrderDatamatrix::on_btnClear_clicked()
{
    if (ui->lst->count() == 0) {
        focusCodeInput();
        return;
    }
    if (C5Message::question(tr("Clear all codes?")) != QDialog::Accepted) {
        focusCodeInput();
        return;
    }
    ui->lst->clear();
    focusCodeInput();
}

void DlgOrderDatamatrix::on_btnOk_clicked()
{
    accept();
}

void DlgOrderDatamatrix::on_btnCancel_clicked()
{
    reject();
}
