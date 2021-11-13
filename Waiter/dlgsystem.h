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

    void setupNoPassword();

private slots:
    void on_btnConnection_clicked();

    void on_btnShutdown_clicked();

    void on_btnRestart_clicked();

    void on_btnExit_clicked();

    void on_btnExitToWindows_clicked();

private:
    Ui::DlgSystem *ui;
};

#endif // DLGSYSTEM_H
