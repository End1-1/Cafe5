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
    ~DlgServerSettings() override;

    bool sessionObtained() const { return m_sessionOk; }

private slots:
    void on_btnSave_clicked();
    void on_btnCancel_clicked();

private:
    void loadFields();
    void applyFields();

    Ui::DlgServerSettings *ui;
    bool m_sessionOk = false;
};

#endif // DLGSERVERSETTINGS_H
