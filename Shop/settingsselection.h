#ifndef SETTINGSSELECTION_H
#define SETTINGSSELECTION_H

#include <QDialog>

namespace Ui {
class SettingsSelection;
}

class SettingsSelection : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsSelection(QWidget *parent = nullptr);

    ~SettingsSelection();

    void addSettingsId(int settingsId, const QString &settingsName);

private slots:
    void on_lst_clicked(const QModelIndex &index);

private:
    Ui::SettingsSelection *ui;
};

#endif // SETTINGSSELECTION_H
