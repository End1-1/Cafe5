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

    void on_cbPartnerStatus_currentIndexChanged(int index);

    void on_cbDriver_currentIndexChanged(int index);

    void on_tbl_cellChanged(int row, int column);

private:
    Ui::C5Route *ui;
};

#endif // C5ROUTE_H
