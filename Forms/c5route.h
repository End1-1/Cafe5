#ifndef C5ROUTE_H
#define C5ROUTE_H

#include "c5document.h"

namespace Ui {
class C5Route;
}

class C5Route : public C5Document
{
    Q_OBJECT

public:
    explicit C5Route(const QStringList &dbParams, QWidget *parent = nullptr);
    ~C5Route();
    virtual QToolBar *toolBar() override;

private:
    void loadPartners();

private slots:
    void setSearchParameters();

    void on_leFilter_textChanged(const QString &arg1);

    void saveDataChanges();

    void on_tbl_doubleClicked(const QModelIndex &index);

    void on_btnGo_clicked();

    void on_tbl_itemSelectionChanged();

    void on_cI_clicked();

    void on_cII_clicked();

    void on_cIII_clicked();

    void on_cIV_clicked();

    void on_cV_clicked();

    void on_cVI_clicked();

    void on_cVII_clicked();

private:
    Ui::C5Route *ui;
    void filterRows();
};

#endif // C5ROUTE_H
