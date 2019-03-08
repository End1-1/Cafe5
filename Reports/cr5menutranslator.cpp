#include "cr5menutranslator.h"
#include "ui_cr5menutranslator.h"
#include "c5lineedit.h"
#include "c5utils.h"

CR5MenuTranslator::CR5MenuTranslator(const QStringList &dbParams, QWidget *parent) :
    C5Widget(dbParams, parent),
    ui(new Ui::CR5MenuTranslator)
{
    ui->setupUi(this);
    C5Database db(fDBParams);
    db.exec("select d.f_id, p2.f_name, d.f_name, t.f_en, t.f_ru "
            "from d_dish d "
            "left join d_part2 p2 on p2.f_id=d.f_part "
            "left join d_translator t on t.f_id=d.f_id "
            "order by 2, 3 ");
    while (db.nextRow()) {
        int row = ui->tbl->addEmptyRow();
        ui->tbl->setString(row, 0, db.getString(0));
        ui->tbl->setString(row, 1, db.getString(1));
        ui->tbl->setString(row, 2, db.getString(2));
        ui->tbl->createLineEdit(row, 3)->setText(db.getString(3));
        ui->tbl->createLineEdit(row, 4)->setText(db.getString(4));
    }
    fLabel = tr("Translator");
    fIcon = ":/translate.png";
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 100, 150, 300, 300, 300);
    ui->tbl->setEditTriggers(QTableWidget::AnyKeyPressed);
}

CR5MenuTranslator::~CR5MenuTranslator()
{
    delete ui;
}

QToolBar *CR5MenuTranslator::toolBar()
{
    if (!fToolBar) {
            QList<ToolBarButtons> btn;
            btn << ToolBarButtons::tbSave;
                createStandartToolbar(btn);
        }
        return fToolBar;
}

void CR5MenuTranslator::on_leSearch_textChanged(const QString &arg1)
{
    ui->tbl->search(arg1);
}

void CR5MenuTranslator::on_btnClearSearch_clicked()
{
    ui->leSearch->clear();
}

void CR5MenuTranslator::saveDataChanges()
{
    C5Database db(fDBParams);
    db.exec("delete from d_translator");
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        db[":f_id"] = ui->tbl->getInteger(i, 0);
        db[":f_mode"] = TRANSLATOR_MENU_DISHES;
        db[":f_en"] = ui->tbl->lineEdit(i, 3)->text();
        db[":f_ru"] = ui->tbl->lineEdit(i, 4)->text();
        db.insert("d_translator", false);
    }
    C5Message::info(tr("Saved"));
}
