#ifndef REPLICADIALOG_H
#define REPLICADIALOG_H

#include <QDialog>

namespace Ui {
class ReplicaDialog;
}

class C5Replication;

class ReplicaDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReplicaDialog(QWidget *parent = nullptr);

    ~ReplicaDialog();

private:
    Ui::ReplicaDialog *ui;

    C5Replication *fReplica;

private slots:
    void progress(const QString &msg);
};

#endif // REPLICADIALOG_H
