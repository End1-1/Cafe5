#include "nsearchdlg.h"
#include "ui_nsearchdlg.h"
#include "ntablemodel.h"

NSearchDlg::NSearchDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NSearchDlg)
{
    ui->setupUi(this);
    auto *m = new NTableModel();
    ui->mTableView->setModel(m);
}

NSearchDlg::~NSearchDlg()
{
    delete ui;
}

void NSearchDlg::setData(const QJsonArray &jcols, const QJsonArray &jdata)
{
    auto *m = static_cast<NTableModel *>(ui->mTableView->model());
    m->setDatasource(jcols, jdata);
    m->setCheckedBox(true, true);
    ui->mTableView->resizeColumnsToContents();
}

void NSearchDlg::on_btnCancel_clicked()
{
    reject();
}

void NSearchDlg::on_lineEdit_textChanged(const QString &arg1)
{
    auto *m = static_cast<NTableModel *>(ui->mTableView->model());
    m->setFilter(arg1);
}

void NSearchDlg::on_mTableView_clicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    if (index.column() == 0) {
        static_cast<NTableModel *>(ui->mTableView->model())->check(index.row());
    }
}

void NSearchDlg::on_mTableView_doubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    static_cast<NTableModel * >(ui->mTableView->model())->check(index.row(), true);
    ui->btnApply->click();
}

void NSearchDlg::on_btnApply_clicked()
{
    std::vector<int> rows;
    auto *m = static_cast<NTableModel *>(ui->mTableView->model());
    if (m->checkedRows(rows)) {
        for (int i : rows) {
            mId.append(m->data(i, 1).toString());
            mName.append(m->data(i, 2).toString());
        }
        accept();
    }
}
