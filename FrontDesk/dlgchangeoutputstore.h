#ifndef DLGCHANGEOUTPUTSTORE_H
#define DLGCHANGEOUTPUTSTORE_H

#include "c5dialog.h"

namespace Ui {
class DlgChangeOutputStore;
}

class DlgChangeOutputStore : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgChangeOutputStore(const QStringList &dbParams, QWidget *parent = nullptr);

    ~DlgChangeOutputStore();

    void refresh(const QDate &d1, const QDate &d2);

private:
    Ui::DlgChangeOutputStore *ui;

    void searchInTable(int col, const QString &str);

private slots:
    void search(const QString &arg1);

    void on_btnCheckVisible_clicked();

    void on_btnRefresh_clicked();

    void on_btnGo_clicked();
};

#endif // DLGCHANGEOUTPUTSTORE_H
