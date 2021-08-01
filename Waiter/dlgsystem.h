#ifndef DLGSYSTEM_H
#define DLGSYSTEM_H

#include "c5dialog.h"

namespace Ui {
class DlgSystem;
}

class DlgSystem : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgSystem(const QStringList &dbParams);

    ~DlgSystem();

private slots:
    void on_btnConnection_clicked();

private:
    Ui::DlgSystem *ui;
};

#endif // DLGSYSTEM_H
