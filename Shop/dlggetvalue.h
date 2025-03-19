#ifndef DLGGETVALUE_H
#define DLGGETVALUE_H

#include <QDialog>

namespace Ui
{
class DlgGetValue;
}

class DlgGetValue : public QDialog
{
    Q_OBJECT

public:
    explicit DlgGetValue(const QStringList &values, QWidget *parent = nullptr);

    ~DlgGetValue();

    QString value() const;

private slots:
    void on_btnCancel_clicked();

    void on_btnOk_clicked();

private:
    Ui::DlgGetValue *ui;
};

#endif // DLGGETVALUE_H
