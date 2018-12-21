#ifndef DLGREPORTSLIST_H
#define DLGREPORTSLIST_H

#include <QDialog>
#include <QJsonArray>

namespace Ui {
class DlgReportsList;
}

class DlgReportsList : public QDialog
{
    Q_OBJECT

public:
    explicit DlgReportsList(QWidget *parent = 0);

    ~DlgReportsList();

    static bool getReport(const QJsonArray &ja, QString &fileName);

private slots:
    void on_lst_clicked(const QModelIndex &index);

private:
    Ui::DlgReportsList *ui;

    QString fResult;
};

#endif // DLGREPORTSLIST_H
