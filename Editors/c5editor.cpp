#include "c5editor.h"
#include "ui_c5editor.h"
#include "ce5editor.h"

C5Editor::C5Editor(const QStringList &dbParams, QWidget *parent) :
    C5Dialog(dbParams, parent),
    ui(new Ui::C5Editor)
{
    ui->setupUi(this);
    fVerticalLayout = ui->vl;
}

C5Editor::~C5Editor()
{
    delete ui;
}

C5Editor *C5Editor::createEditor(const QStringList &dbParams, CE5Editor *e, int id)
{
    C5Editor *de = new C5Editor(dbParams, C5Config::fParentWidget);
    de->fEditor = e;
    de->fVerticalLayout->addWidget(e);
    de->adjustSize();
    e->getLineEdit(e);
    e->setId(id);
    de->setWindowTitle(e->title());
    return de;
}

bool C5Editor::getResult(QList<QMap<QString, QVariant> > &data)
{
    fEditor->focusFirst();
    if (exec() == QDialog::Accepted) {
        data = fData;
        return true;
    }
    return false;
}

void C5Editor::on_btnSave_clicked()
{
    if (on_btnSaveAndNew_clicked()) {
        accept();
    }
}

bool C5Editor::on_btnSaveAndNew_clicked()
{
    QString err;
    if (!fEditor->checkData(err)) {
        C5Message::error(err);
        return false;
    }
    err = "";
    if (!fEditor->save(err, fData)) {
        C5Message::error(err);
        return false;
    }
    fEditor->clear();
    return true;
}

void C5Editor::on_btnCancel_clicked()
{
    fEditor->clear();
    reject();
}