#include "cr5databases.h"
#include "c5dbuseraccess.h"
#include "ce5databases.h"
#include "c5mainwindow.h"
#include "c5datasynchronize.h"
#include "c5tablemodel.h"
#include "c5checkdatabase.h"
#include "c5dbresetoption.h"
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
        fToolBar->addAction(QIcon(":/save-file.png"), tr("Backup\ndatabase"), this, SLOT(backupDatabase()));
        fToolBar->addAction(QIcon(":/reset.png"), tr("Reset\ndatabase"), this, SLOT(resetDatabase()));
    }
    return fToolBar;
}

void CR5Databases::actionAccess()
{
    __mainWindow->createTab<C5DbUserAccess>(fDBParams);
}

void CR5Databases::backupDatabase()
{
    QVector<QJsonValue> values = fModel->getRowValues(fTableView->currentIndex().row());
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
    auto *dr = new C5DbResetOption(fDBParams);
    if (dr->exec() == QDialog::Accepted) {
        if (dr->saleAndBuy()) {
            fHttp->createHttpQuery("/engine/worker/cleardb.php", QJsonObject{{"mode", "storeandsale"}}, SLOT(done(QJsonObject)));
        }
        if (dr->fullReset()) {
            fHttp->createHttpQuery("/engine/worker/cleardb.php", QJsonObject{{"mode", "all"}}, SLOT(done(QJsonObject)));
        }
    }
    dr->deleteLater();
}

void CR5Databases::done(QJsonObject)
{
    fHttp->httpQueryFinished(sender());
    C5Message::info(tr("Done"));
}
