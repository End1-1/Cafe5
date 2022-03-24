#ifndef DLGLICENSES_H
#define DLGLICENSES_H

#include <QDialog>

namespace Ui {
class DlgLicenses;
}

class DlgLicenses : public QDialog
{
    Q_OBJECT

public:
    explicit DlgLicenses(QWidget *parent = nullptr);
    ~DlgLicenses();
    void addLicense(int id, const QString &d1, const QString &d2, int ltype);

private slots:
    void on_tbl_cellClicked(int row, int column);

    void on_btnClose_clicked();

    void on_btnActivate_clicked();

private:
    Ui::DlgLicenses *ui;
};

#endif // DLGLICENSES_H
