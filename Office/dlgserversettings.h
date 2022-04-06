#ifndef DLGSERVERSETTINGS_H
#define DLGSERVERSETTINGS_H

#include <QDialog>

namespace Ui {
class DlgServerSettings;
}

class DlgServerSettings : public QDialog
{
    Q_OBJECT

public:
    explicit DlgServerSettings(QWidget *parent = nullptr);
    ~DlgServerSettings();

private slots:
    void on_btnCancel_clicked();

    void on_btnSave_clicked();

private:
    Ui::DlgServerSettings *ui;
};

#endif // DLGSERVERSETTINGS_H
