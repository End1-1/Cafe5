#ifndef REPLICADIALOG_H
#define REPLICADIALOG_H

#include "c5database.h"
#include <QDialog>

namespace Ui {
class ReplicaDialog;
}

class ReplicaDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReplicaDialog(QWidget *parent = nullptr);
    ~ReplicaDialog();

private:
    Ui::ReplicaDialog *ui;

    void progress(const QString &msg);

private slots:
    void timeout();

    void updateTable(C5Database &dr, C5Database &db, int &step, int steps, const QString &tableName);
};

#endif // REPLICADIALOG_H
