#include "cr5databases.h"
#include "c5dbuseraccess.h"
#include "ce5databases.h"
#include "c5mainwindow.h"
#include "c5datasynchronize.h"
#include "c5tablemodel.h"
#include "c5serviceconfig.h"
#include "c5checkdatabase.h"
#include "c5dbresetoption.h"
#include "c5replication.h"
#include "c5random.h"
#include <QFileDialog>
#include <QProcess>
#include <QDesktopServices>
#include <QApplication>

CR5Databases::CR5Databases(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/database.png";
    fLabel = tr("Databases");
    fSqlQuery = "select f_id, f_host, f_db, f_name, f_description, f_user, f_password from s_db ";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_host"] = tr("Host");
    fTranslation["f_db"] = tr("Database");
    fTranslation["f_name"] = tr("Name");
    fTranslation["f_description"] = tr("Description");
    fTranslation["f_user"] = tr("User");
    fTranslation["f_password"] = tr("Password");

    fTableView->setItemDelegateForColumn(1, new C5TextDelegate(fTableView));
    fTableView->setItemDelegateForColumn(2, new C5TextDelegate(fTableView));
    fTableView->setItemDelegateForColumn(3, new C5TextDelegate(fTableView));
    fTableView->setItemDelegateForColumn(4, new C5TextDelegate(fTableView));
    fTableView->setItemDelegateForColumn(5, new C5TextDelegate(fTableView));
    fTableView->setItemDelegateForColumn(6, new C5TextDelegate(fTableView));

    fEditor = new CE5Databases(dbParams);
}

QToolBar *CR5Databases::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbNew
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
            createStandartToolbar(btn);
        fToolBar->addAction(QIcon(":/access.png"), tr("Access"), this, SLOT(actionAccess()));
        fToolBar->addAction(QIcon(":data-transfer.png"), tr("Synchronization"), this, SLOT(actionSync()));
        fToolBar->addAction(QIcon(":/service.png"), tr("Service"), this, SLOT(actionService()));
        fToolBar->addAction(QIcon(":/maintenance.png"), tr("Check\ndatabase"), this, SLOT(checkDatabase()));
        fToolBar->addAction(QIcon(":/save-file.png"), tr("Backup\ndatabase"), this, SLOT(backupDatabase()));
        fToolBar->addAction(QIcon(":/reset.png"), tr("Reset\ndatabase"), this, SLOT(resetDatabase()));
        fToolBar->addAction(QIcon(":/restore.png"), tr("Upload\ndata"), this, SLOT(uploadData()));
    }
    return fToolBar;
}

void CR5Databases::actionAccess()
{
    __mainWindow->createTab<C5DbUserAccess>(fDBParams);
}

void CR5Databases::actionSync()
{
    QList<QVariant> values = fModel->getRowValues(fTableView->currentIndex().row());
    if (values.at(0).toString().isEmpty()) {
        return;
    }
    QStringList p;
    p.append(values.at(1).toString());
    p.append(values.at(2).toString());
    p.append(values.at(5).toString());
    p.append(values.at(6).toString());
    p.append(values.at(3).toString());
    C5DataSynchronize *ds = new C5DataSynchronize(p);
    ds->exec();
    delete ds;
}

void CR5Databases::actionService()
{
    QList<QVariant> values = fModel->getRowValues(fTableView->currentIndex().row());
    if (values.at(0).toString().isEmpty()) {
        return;
    }
    C5ServiceConfig *sc = new C5ServiceConfig(values.at(1).toString(), this);
    sc->exec();
    delete sc;
}

void CR5Databases::checkDatabase()
{
    QList<QVariant> values = fModel->getRowValues(fTableView->currentIndex().row());
    if (values.at(0).toString().isEmpty()) {
        return;
    }
    QStringList p;
    p.append(values.at(1).toString());
    p.append(values.at(2).toString());
    p.append(values.at(5).toString());
    p.append(values.at(6).toString());
    p.append(values.at(3).toString());
    C5CheckDatabase *ds = new C5CheckDatabase(p);
    ds->exec();
    delete ds;
}

void CR5Databases::backupDatabase()
{
    QList<QVariant> values = fModel->getRowValues(fTableView->currentIndex().row());
    if (values.at(0).toString().isEmpty()) {
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this, tr("Select folder for dump"), "", "*.sql");
    if (fileName.isEmpty()) {
        return;
    }
    if (fileName.right(4).toLower() != ".sql") {
        fileName += ".sql";
    }
    QStringList args;
    args.append("-h");
    args.append(values.at(1).toString());
    args.append(values.at(2).toString());
    args.append("-u" + values.at(5).toString());
    args.append("-p" + values.at(6).toString());
    args.append("-r" + fileName);
    QProcess p;
    p.setProcessChannelMode(QProcess::MergedChannels);
    p.start(qApp->applicationDirPath() + "/mysqldump.exe", args);
    p.waitForStarted(-1);
    p.waitForFinished(-1);
    if (p.exitCode() != 0) {
        C5Message::error(tr("Backup with error") + "<br>" + p.readAll());
    } else {
        C5Message::info(tr("Backup completed") + "<br>" + p.readAll());
    }
}

void CR5Databases::resetDatabase()
{
    if (!fTableView->currentIndex().isValid()) {
        C5Message::info(tr("Nothing was selected"));
        return;
    }
    QList<QVariant> values = fModel->getRowValues(fTableView->currentIndex().row());
    if (values.at(0).toString().isEmpty()) {
        return;
    }
    QStringList p;
    p.append(values.at(1).toString());
    p.append(values.at(2).toString());
    p.append(values.at(5).toString());
    p.append(values.at(6).toString());
    p.append(values.at(3).toString());
    C5DbResetOption *dr = new C5DbResetOption(fDBParams);
    if (dr->exec() == QDialog::Accepted) {
        C5Database db(p);
        if (dr->saleAndBuy()) {
            db.exec("delete from op_body");
            db.exec("delete from op_header");
            db.exec("delete from a_calc_price");
            db.exec("delete from a_complectation_additions");
            db.exec("delete from a_header_shop2partner");
            db.exec("delete from a_header_shop2partneraccept");
            db.exec("delete from a_store");
            db.exec("delete from a_store_draft");
            db.exec("delete from o_draft_sale_body");
            db.exec("delete from o_draft_sale");
            db.exec("delete from o_draft_sound");
            db.exec("delete from e_cash");
            db.exec("delete from a_header_store");
            db.exec("delete from a_header_cash");
            db.exec("delete from a_header");
            db.exec("delete from b_history");
            db.exec("delete from b_clients_debts");
            db.exec("delete from o_tax_log");
            db.exec("delete from o_tax_debug");
            db.exec("delete from o_tax");
            db.exec("delete from o_payment");
            db.exec("delete from o_package");
            db.exec("delete from o_goods");
            db.exec("delete from o_body");
            db.exec("delete from o_preorder");
            db.exec("delete from o_header_hotel");
            db.exec("delete from o_header_hotel_date");
            db.exec("delete from o_header_options");
            db.exec("delete from o_header_flags");
            db.exec("delete from o_header");
            db.exec("delete from o_pay_cl");
            db.exec("delete from o_pay_room");
            db.exec("delete from o_additional");
        }
        if (dr->fullReset()) {
            QFile f(":/cleardb.sql");
            if (f.open(QIODevice::ReadOnly)) {
                QList<QByteArray> sqls = f.readAll().split(';');
                QStringList errors;
                foreach (const QString &s, sqls) {
                    if (!db.exec(s)) {
                        errors += db.fLastError + "\r\n";
                    }
                }
                f.close();
                if (!errors.isEmpty()) {
                    QFile fe(QDir::tempPath() + "/clearerrors.txt");
                    if (fe.open(QIODevice::WriteOnly)) {
                        QString ba = errors.join("\r\n");
                        fe.write(ba.toUtf8());
                        fe.close();
                        QDesktopServices::openUrl(QDir::tempPath() + "/clearerrors.txt");
                    }
                }
            } else {
                C5Message::error("Cannot open file <br>" + f.errorString());
            }
        }
        C5Message::info(tr("Done"));
    }
    dr->deleteLater();
}

void CR5Databases::uploadData()
{
    QList<QVariant> values = fModel->getRowValues(fTableView->currentIndex().row());
    if (values.at(0).toString().isEmpty()) {
        return;
    }
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select folder for dump"), "", "*.sql");
    if (fileName.isEmpty()) {
        return;
    }
    QString randomDBName = "db_" + C5Random::randomStr(10000, 99999);
    C5Database db(values.at(1).toString(), values.at(2).toString(), values.at(5).toString(), values.at(6).toString());
    db.exec("create database " + randomDBName + " default character set utf8 collate utf8_general_ci");
    QProcess p;
    p.setProcessChannelMode(QProcess::MergedChannels);
    QStringList args;
    args.append("-h");
    args.append(values.at(1).toString());
    args.append(randomDBName);
    args.append("-u" + values.at(5).toString());
    args.append("-p" + values.at(6).toString());
    p.setStandardInputFile(fileName);
    p.start(qApp->applicationDirPath() + "/mysql.exe", args);
    p.waitForStarted(-1);
    p.waitForFinished(-1);
    if (p.exitCode() != 0) {
        C5Message::error(tr("Upload with error") + "<br>" + p.readAll());
        return;
    }
    QString sqlMsg = p.readAll();
    QStringList randomDB = db.dbParams();
    randomDB[1] = randomDBName;
    C5Replication r;
    r.fIgnoreErrors = true;
    r.downloadDataFromServer(randomDB, fDBParams);
    r.uploadDataToServer(randomDB, db.dbParams());
    db.exec("drop database "  + randomDBName);
    C5Message::info(tr("Upload completed") + "<br>" + sqlMsg);
}
