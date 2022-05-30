#ifndef REPLICADIALOG_H
#define REPLICADIALOG_H

#include "c5dialog.h"

namespace Ui {
class ReplicaDialog;
}

class C5Replication;

class ReplicaDialog : public C5Dialog
{
    Q_OBJECT

public:
    explicit ReplicaDialog();

    ~ReplicaDialog();

private:
    Ui::ReplicaDialog *ui;

    C5Replication *fReplica;

private slots:
    void progress(const QString &msg);
};

#endif // REPLICADIALOG_H
