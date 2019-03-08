#ifndef C5DATASYNCHRONIZE_H
#define C5DATASYNCHRONIZE_H

#include "c5dialog.h"

namespace Ui {
class C5DataSynchronize;
}

class C5DataSynchronize : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5DataSynchronize(const QStringList &dbParams, QWidget *parent = 0);

    ~C5DataSynchronize();

private slots:
    void removeSyncTable();

    void on_btnStartUpload_clicked();

    void statusUpdate(const QString &name, int row, const QString &msg);

    void statusUpdateUpload(int row, int column, const QString &msg);

    void progressBarMax(int max);

    void progressBarValue(int value);

    void dstFinished();

    void errLog(const QString &msg);

    void on_tblSyncTables_customContextMenuRequested(const QPoint &pos);

    void on_btnSaveSyncTables_2_clicked();

    void on_btnSaveSyncTables_clicked();

    void on_leAddSyncTable_returnPressed();

    void on_btnAddSyncTable_clicked();

    void on_tabWidget_currentChanged(int index);

private:
    Ui::C5DataSynchronize *ui;

    int fThreadsCount;

    QStringList fRemovedSyncTables;

    void downloadData();

    void uploadData();

    void getSyncTables();
};

#endif // C5DATASYNCHRONIZE_H
