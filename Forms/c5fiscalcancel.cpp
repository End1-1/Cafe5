#include "c5fiscalcancel.h"
#include "ui_c5fiscalcancel.h"
#include "c5message.h"
#include "printtaxn.h"
#include "c5config.h"
#include "c5database.h"
#include "ninterface.h"
#include <QJsonDocument>
#include <QJsonParseError>

C5FiscalCancel::C5FiscalCancel(C5User *user, const QString &id)
    : C5Dialog(user), ui(new Ui::C5FiscalCancel),
      fUuid(id)
{
    ui->setupUi(this);
    getInfo();
}

C5FiscalCancel::~C5FiscalCancel() { delete ui; }

void C5FiscalCancel::on_btnRequestCancel_clicked()
{
    int row = ui->tbl->currentRow();

    if(row < 0) {
        return;
    }

    if(C5Message::question(tr("Confirm fiscal return")) != QDialog::Accepted) {
        return;
    }

    C5Database db;
    QString crn = ui->tbl->getString(row, 2);
    int rseq = ui->tbl->getInteger(row, 1);
    QString jstr = ui->tbl->getString(row, 3);
    QJsonParseError jerr;
    QJsonObject jb = QJsonDocument::fromJson(jstr.toUtf8(), &jerr).object();

    if(jerr.error != QJsonParseError::NoError) {
        C5Message::error(jerr.errorString());
        return;
    }

    PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(), C5Config::taxUseExtPos(),
                 C5Config::taxCashier(), C5Config::taxPin(), this);
    QString jsnin, jsnout, err;
    int result;
    QJsonArray emarks = jb["emarks"].toArray();

    if(emarks.isEmpty()) {
        emarks =  jb["eMarks"].toArray();
    }

    for(const QJsonValue &v : emarks) {
        pt.fEmarks << v.toString();
    }

    result = pt.printTaxback(rseq, crn, jsnin, jsnout, err);
    db[":f_id"] = C5Database::uuid();
    db[":f_order"] = fUuid;
    db[":f_date"] = QDate::currentDate();
    db[":f_time"] = QTime::currentTime();
    db[":f_in"] = jsnin;
    db[":f_out"] = jsnout;
    db[":f_err"] = err;
    db[":f_result"] = result;
    db.insert("o_tax_log", false);

    if(result != pt_err_ok) {
        C5Message::error(err);
    } else {
        db[":f_fiscal"] = QVariant();
        db[":f_receiptnumber"] = QVariant();
        db[":f_time"] = QVariant();
        db.update("o_tax", "f_id", fUuid);
        db[":f_order"] = fUuid;
        db.exec("update o_tax_log set f_state=0 where f_order=:f_order and f_state=1");
        C5Message::info(tr("Taxback complete"));
    }

    getInfo();
}

void C5FiscalCancel::getInfo()
{
    fHttp->createHttpQueryLambda("/engine/v2/officen/fiscal-data/get", {{"id", fUuid}}, [ = ](const QJsonObject & jo) {
        QJsonArray ja = jo["data"].toArray();
        ui->tbl->setRowCount(ja.count());

        for(int i = 0; i < ja.count(); i++) {
            QJsonObject j = ja.at(i).toObject();
            QJsonObject jf = QJsonDocument::fromJson(j["f_out"].toString().toUtf8()).object();
            ui->tbl->setInteger(i, 0, j["f_id"].toInt());
            ui->tbl->setInteger(i, 1, jf["rseq"].toInt());
            ui->tbl->setString(i, 2, jf["crn"].toString());
            ui->tbl->setString(i, 3, j["f_in"].toString());
            ui->tbl->setString(i, 4, j["f_out"].toString());
            ui->tbl->setInteger(i, 5, j["f_state"].toInt());
        }
    }, [ = ](const QJsonObject & je) {
    });
}

void C5FiscalCancel::on_btnRefresh_clicked()
{
    getInfo();
}
