#ifndef INSTALL_H
#define INSTALL_H

#include <QWizard>

namespace Ui {
class Install;
}

class Install : public QWizard
{
    Q_OBJECT

public:
    explicit Install(QWidget *parent = nullptr);
    ~Install();

private:
    Ui::Install *ui;
};

#endif // INSTALL_H
