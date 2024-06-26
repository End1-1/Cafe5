#include "replicadialog.h"
#include "ui_replicadialog.h"
#include "c5database.h"
#include "c5config.h"
#include "c5logsystem.h"
#include "c5message.h"
#include "c5replication.h"

ReplicaDialog::ReplicaDialog() :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::ReplicaDialog)
{
    ui->setupUi(this);
    fReplica = new C5Replication();
    connect(fReplica, SIGNAL(progress(QString)), this, SLOT(progress(QString)));
    connect(fReplica, SIGNAL(finished()), this, SLOT(accept()));
    fReplica->start(SLOT(downloadFromServer()));
}

ReplicaDialog::~ReplicaDialog()
{
    delete ui;
}

void ReplicaDialog::progress(const QString &msg)
{
    ui->lbMsg->setText(msg);
    //qApp->processEvents();
    C5LogSystem::writeEvent(msg);
}
