#ifndef DLGGETIDNAME_H
#define DLGGETIDNAME_H

#include "c5dialog.h"

#define idname_users_fullname 1
#define idname_partners_full 2

namespace Ui {
class DlgGetIDName;
}

class DlgGetIDName : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgGetIDName(const QStringList &dbParams, QWidget *parent = nullptr);
    ~DlgGetIDName();
    static bool get(const QStringList &dbParams, QString &id, QString &name, int table, QWidget *parent = nullptr);

private slots:
    void on_btnCancel_clicked();

    void on_btnOk_clicked();

    void on_btnRefresh_clicked();

    void on_leFilter_textChanged(const QString &arg1);

    void on_tbl_cellDoubleClicked(int row, int column);

private:
    Ui::DlgGetIDName *ui;
    int fTable;
    QString fId;
    QString fName;
    void getData();
    static QHash<int, QString> fQueries;
    static QHash<int, DlgGetIDName*> fDialogs;
};

#endif // DLGGETIDNAME_H
