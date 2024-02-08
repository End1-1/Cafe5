#ifndef DLGCONFIG_H
#define DLGCONFIG_H

#include <QDialog>

namespace Ui {
class DlgConfig;
}

class DlgConfig : public QDialog
{
    Q_OBJECT

public:
    explicit DlgConfig(QWidget *parent = nullptr);
    ~DlgConfig();

private slots:
    void on_btnExplore_clicked();

    void on_btnOK_clicked();

private:
    Ui::DlgConfig *ui;
};

#endif // DLGCONFIG_H
