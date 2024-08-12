#ifndef DLGSERVICEVALUES_H
#define DLGSERVICEVALUES_H

#include <QDialog>
#include <QJsonObject>

namespace Ui
{
class DlgServiceValues;
}

class DlgServiceValues : public QDialog
{
    Q_OBJECT

public:
    explicit DlgServiceValues(QWidget *parent = nullptr);
    ~DlgServiceValues();
    void config(const QJsonObject &jo);

private slots:
    void on_lst_clicked(const QModelIndex &index);

private:
    Ui::DlgServiceValues *ui;
};

#endif // DLGSERVICEVALUES_H
