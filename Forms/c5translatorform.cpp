#include "c5translatorform.h"
#include "ui_c5translatorform.h"
#include "c5lineedit.h"
#include "c5message.h"

C5TranslatorForm::C5TranslatorForm(const QStringList &dbParams, QWidget *parent) :
    C5Widget(dbParams, parent),
    ui(new Ui::C5TranslatorForm)
{
    ui->setupUi(this);
    fLabel = tr("Translator");
    fIcon = ":/translate.png";
    refreshData();
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
            << ToolBarButtons::tbRefresh
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
    QString sql;
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        if (sql.isEmpty()) {
            sql = "insert into s_translator (f_en, f_am, f_ru) values ";
        } else {
            sql += ",";
        }
        sql += QString("('%1', '%2', '%3')")
               .arg(ui->tbl->getString(i, 0),
                    ui->tbl->lineEdit(i, 1)->text(),
                    ui->tbl->lineEdit(i, 2)->text() );
    }
    db.execDirect(sql);
    C5Message::info(tr("Saved"));
}

void C5TranslatorForm::refreshData()
{
    ui->tbl->setRowCount(0);
    C5Database db(__c5config.dbParams());
    db.exec("select f_en, f_am, f_ru from s_translator");
    while (db.nextRow()) {
        int row = ui->tbl->addEmptyRow();
        ui->tbl->setString(row, 0, db.getString(0));
        ui->tbl->createLineEdit(row, 1)->setText(db.getString(1));
        ui->tbl->createLineEdit(row, 2)->setText(db.getString(2));
    }
}
