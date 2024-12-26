#include "c5datasynchronize.h"
#include "ui_c5datasynchronize.h"
#include "nloadingdlg.h"
#include "ndataprovider.h"
#include "logwriter.h"
#include <QScrollBar>
#include <QMenu>

C5DataSynchronize::C5DataSynchronize(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::C5DataSynchronize)
{
    ui->setupUi(this);
}

C5DataSynchronize::~C5DataSynchronize()
{
    delete ui;
}

void C5DataSynchronize::queryStarted()
{
    fLoadingDlg = new NLoadingDlg(this);
    fLoadingDlg->open();
}

void C5DataSynchronize::queryError(const QString &err)
{
    fLoadingDlg->deleteLater();
    sender()->deleteLater();
    C5Message::error(err);
}

void C5DataSynchronize::queryDone(const QJsonObject &jdoc)
{
    fLoadingDlg->deleteLater();
    sender()->deleteLater();
    C5Message::info(tr("Done"));
}

void C5DataSynchronize::on_btnSaveSyncTables_clicked()
{
    auto *dp = new NDataProvider(this);
    connect(dp, &NDataProvider::started, this, &C5DataSynchronize::queryStarted);
    connect(dp, &NDataProvider::error, this, &C5DataSynchronize::queryError);
    connect(dp, &NDataProvider::done, this, &C5DataSynchronize::queryDone);
    dp->getData("/engine/sync/create-triggers.php", QJsonObject{{"clearonly", 0}});
}

void C5DataSynchronize::on_btnRevert_clicked()
{
    auto *dp = new NDataProvider(this);
    connect(dp, &NDataProvider::started, this, &C5DataSynchronize::queryStarted);
    connect(dp, &NDataProvider::error, this, &C5DataSynchronize::queryError);
    connect(dp, &NDataProvider::done, this, &C5DataSynchronize::queryDone);
    dp->getData("/engine/sync/create-triggers.php", QJsonObject{{"clearonly", 1}});
}
