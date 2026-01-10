#ifndef C5DATASYNCHRONIZE_H
#define C5DATASYNCHRONIZE_H

#include "c5dialog.h"

namespace Ui
{
class C5DataSynchronize;
}

class NLoadingDlg;

class C5DataSynchronize : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5DataSynchronize(C5User *user);

    ~C5DataSynchronize();

private slots:
    void queryStarted();

    void queryError(const QString &err);

    void queryDone(const QJsonObject &jdoc);

    void on_btnSaveSyncTables_clicked();

    void on_btnRevert_clicked();

private:
    Ui::C5DataSynchronize *ui;

    NLoadingDlg *fLoadingDlg;

};

#endif // C5DATASYNCHRONIZE_H
