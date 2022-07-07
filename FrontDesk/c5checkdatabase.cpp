#include "c5checkdatabase.h"
#include "ui_c5checkdatabase.h"
#include "c5document.h"
#include "c5storedraftwriter.h"
#include "dlgstoreutils.h"

C5CheckDatabase::C5CheckDatabase(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::C5CheckDatabase)
{
    ui->setupUi(this);
}

C5CheckDatabase::~C5CheckDatabase()
{
    delete ui;
}

void C5CheckDatabase::on_btnStart_clicked()
{
    C5Database db1(fDBParams);
    C5Database db2(fDBParams);
    C5StoreDraftWriter dw(db2);
    C5Document doc(fDBParams);
    db1[":f_type"] = DOC_TYPE_STORE_INPUT;
    db1.exec("select * from a_header where f_type=:f_type");
    int errnum = 0;
    int r = 0;
    while (db1.nextRow()) {
        db2[":f_id"] = db1.getString("f_id");
        db2.exec("select * from a_header_store where f_id=:f_id");
        if (db2.nextRow()) {
            QString cashuuid = db2.getString("f_cashuuid");
            if (cashuuid.isEmpty()) {
                CORRECTCASH:
                errnum++;
                QString purpose = tr("Store input") + " #" + db1.getString("f_userid") + ", " + db1.getDate("f_date").toString(FORMAT_DATE_TO_STR);
                QString fCashRowId;
                QString cashdocnum = QString("%1").arg(doc.genNumber(DOC_TYPE_CASH), C5Config::docNumDigitsInput(), 10, QChar('0'));
                dw.writeAHeader(cashuuid, cashdocnum, DOC_STATE_DRAFT, DOC_TYPE_CASH, 1, db1.getDate("f_date"), QDate::currentDate(), QTime::currentTime(), db1.getInt("f_partner"), db1.getDouble("f_amount"), purpose);
                dw.writeAHeaderCash(cashuuid, 0, 1, 1, db1.getString("f_id"), "", 0);
                dw.writeECash(fCashRowId, cashuuid, 1, -1, purpose, db1.getDouble("f_amount"), fCashRowId, 1);
                db2[":f_id"] = db1.getString("f_id");
                db2[":f_cashuuid"] = cashuuid;
                db2.exec("update a_header_store set f_cashuuid=:f_cashuuid where f_id=:f_id");
            } else {
                db2[":f_id"] = cashuuid;
                db2.exec("select * from a_header where f_id=:f_id");
                if (db2.nextRow()) {
                    //check existing document
                } else {
                    goto CORRECTCASH;
                }
            }
        } else {
            //error
        }
        ui->lst->item(0)->setText(QString("Store input documents. (%1 of %2, %3 errors)").arg(++r).arg(db1.rowCount()).arg(errnum));
        qApp->processEvents();
    }
    C5Message::info(tr("Complete"));
}

void C5CheckDatabase::on_btnStoreUtlis_clicked()
{
    DlgStoreUtils d(fDBParams);
    d.exec();
}
