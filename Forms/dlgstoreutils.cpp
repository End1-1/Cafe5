#include "dlgstoreutils.h"
#include "ui_dlgstoreutils.h"
#include "c5storedraftwriter.h"
#include "c5utils.h"

QStringList docs;
QList<int> types;
int total;

DlgStoreUtils::DlgStoreUtils(C5User *user) :
    C5Dialog(user),
    ui(new Ui::DlgStoreUtils)
{
    ui->setupUi(this);
}

DlgStoreUtils::~DlgStoreUtils()
{
    delete ui;
}
void DlgStoreUtils::on_btnWriteAllDocuments_clicked()
{
    C5Database db;
    C5StoreDraftWriter dw(db);
    QString err;

    if(docs.isEmpty()) {
        db.exec("select f_id, f_type from a_header where f_type in (1,2,3,6,9) and f_state=1 order by f_createdate, f_createtime");

        while(db.nextRow()) {
            docs.append(db.getString(0));
            types.append(db.getInt(1));
        }

        total = docs.count();
    }

    ui->lbError->clear();
    ui->lbDocsCount->setText(QString("Docs count: %1").arg(docs.count()));

    while(docs.count() > 0) {
        ui->lbCurrentlyProceed->setText(QString("%1 of %2 remains").arg(docs.count()).arg(total));

        switch(types.at(0)) {
        case DOC_TYPE_STORE_INPUT:
            dw.writeInput(docs.at(0), err);
            break;

        case DOC_TYPE_STORE_OUTPUT:
            dw.writeOutput(docs.at(0), err);
            break;

        case DOC_TYPE_STORE_MOVE:
            dw.writeOutput(docs.at(0), err);
            break;

        case DOC_TYPE_COMPLECTATION:
            dw.writeOutput(docs.at(0), err);
            break;
        }

        if(!err.isEmpty()) {
            ui->lbError->setText(QString("%1, %2").arg(types.at(0)).arg(err));
            return;
        }

        docs.removeAt(0);
        types.removeAt(0);

        if(docs.count() % 100 == 0) {
            qApp->processEvents();
        }
    }
}
