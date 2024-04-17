#ifndef NSEARCHDLG_H
#define NSEARCHDLG_H

#include <QDialog>
#include <QJsonArray>

namespace Ui {
class NSearchDlg;
}

class NSearchDlg : public QDialog
{
    Q_OBJECT

public:
    explicit NSearchDlg(QWidget *parent = nullptr);
    ~NSearchDlg();
    void setData(const QJsonArray &jcols, const QJsonArray &jdata);
    QString mId;
    QString mName;

private slots:
    void on_btnCancel_clicked();

    void on_lineEdit_textChanged(const QString &arg1);

    void on_mTableView_clicked(const QModelIndex &index);

    void on_mTableView_doubleClicked(const QModelIndex &index);

    void on_btnApply_clicked();

private:
    Ui::NSearchDlg *ui;
};

#endif // NSEARCHDLG_H
