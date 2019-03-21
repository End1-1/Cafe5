#include "c5translatorform.h"
#include "ui_c5translatorform.h"
#include "c5lineedit.h"

C5TranslatorForm::C5TranslatorForm(const QStringList &dbParams, QWidget *parent) :
    C5Widget(dbParams, parent),
    ui(new Ui::C5TranslatorForm)
{
    ui->setupUi(this);
    fLabel = tr("Translator");
    fIcon = ":/translate.png";
    C5Database db(dbParams);
    db.exec("select * from s_translator");
    while (db.nextRow()) {
        int row = ui->tbl->addEmptyRow();
        ui->tbl->setString(row, 0, db.getString(0));
        ui->tbl->createLineEdit(row, 1)->setText(db.getString(1));
        ui->tbl->createLineEdit(row, 2)->setText(db.getString(2));
    }
}

C5TranslatorForm::~C5TranslatorForm()
{
    delete ui;
}

QToolBar *C5TranslatorForm::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbSave
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        fToolBar = createStandartToolbar(btn);
    }
    return fToolBar;
}

void C5TranslatorForm::saveDataChanges()
{
    C5Database db(fDBParams);
    db.exec("delete from s_translator");
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        db[":f_text"] = ui->tbl->getString(i, 0);
        db[":f_en"] = ui->tbl->lineEdit(i, 1)->text();
        db[":f_ru"] = ui->tbl->lineEdit(i, 2)->text();
        db.insert("s_translator", false);
    }
    C5Message::info(tr("Saved"));
}
