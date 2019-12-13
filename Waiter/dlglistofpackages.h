#ifndef DLGLISTOFPACKAGES_H
#define DLGLISTOFPACKAGES_H

#include <QDialog>

namespace Ui {
class DlgListOfPackages;
}

class DlgListOfPackages : public QDialog
{
    Q_OBJECT

public:
    explicit DlgListOfPackages(QWidget *parent = nullptr);

    ~DlgListOfPackages();

    static bool package(int &id, QString &name);

private slots:
    void on_tbl_cellClicked(int row, int column);

private:
    Ui::DlgListOfPackages *ui;

    int fId;

    QString fName;
};

#endif // DLGLISTOFPACKAGES_H
