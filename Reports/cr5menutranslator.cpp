#include "cr5menutranslator.h"
#include "ui_cr5menutranslator.h"
#include "c5lineedit.h"
#include "c5database.h"
#include "c5message.h"
#include <QJsonArray>

CR5MenuTranslator::CR5MenuTranslator(const QStringList &dbParams,  QWidget *parent) :
    C5Widget(dbParams, parent),
    ui(new Ui::CR5MenuTranslator)
{
    ui->setupUi(this);
    fLabel = tr("Translator");
    fIcon = ":/translate.png";
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 100, 150, 300, 300, 300);
    //ui->tbl->setEditTriggers(QTableWidget::AnyKeyPressed);
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

void CR5MenuTranslator::setMode(int mode)
{
    C5Database db(fDBParams);
    fMode = mode;
    if (mode == 1) {
        db.exec("select d.f_id, p2.f_name, d.f_name, t.f_en, t.f_ru "
                "from d_dish d "
                "left join d_part2 p2 on p2.f_id=d.f_part "
                "left join d_translator t on t.f_id=d.f_id and t.f_mode=1 "
                "order by 2, 3 ");
    }
    if (mode == 2) {
        db.exec("select p2.f_id, p1.f_name, p2.f_name, t.f_en, t.f_ru "
                "from  d_part2 p2  "
                "left join d_part1 p1 on p1.f_id=p2.f_part "
                "left join d_translator t on t.f_id=p2.f_id and t.f_mode=2 "
                "order by 2, 3 ");
    }
    if (mode == 3) {
        db.exec("select d.f_id, d.f_name, d.f_comment, t.f_en, t.f_ru "
                "from d_dish d "
                "left join d_translator t on t.f_id=d.f_id and t.f_mode=3 "
                "order by 2 ");
    }
    ui->tbl->setUpdatesEnabled(false);
    while (db.nextRow()) {
        int row = ui->tbl->addEmptyRow();
        ui->tbl->setString(row, 0, db.getString(0));
        ui->tbl->setString(row, 1, db.getString(1));
        ui->tbl->setString(row, 2, db.getString(2));
        ui->tbl->createLineEdit(row, 3)->setText(db.getString(3));
        ui->tbl->createLineEdit(row, 4)->setText(db.getString(4));
    }
    ui->tbl->setUpdatesEnabled(true);
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
    QJsonArray jt;
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        QJsonObject jo;
        jo["f_id"] = ui->tbl->getInteger(i, 0);
        jo["f_mode"] = fMode;
        jo["f_am"] = ui->tbl->getString(i, 2);
        jo["f_en"] = ui->tbl->lineEdit(i, 3)->text();
        jo["f_ru"] = ui->tbl->lineEdit(i, 4)->text();
        jt.append(jo);
    }
    fHttp->createHttpQuery("/engine/office/save-translator.php", QJsonObject{
        {"translator", jt},
        {"mode", fMode}

    }, SLOT(saveResponse(QJsonObject)));
}

void CR5MenuTranslator::saveResponse(const QJsonObject &jo)
{
    fHttp->httpQueryFinished(sender());
    C5Message::info(tr("Saved"));
}
