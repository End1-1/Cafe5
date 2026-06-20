#ifndef RCASHMOVEMENT_H
#define RCASHMOVEMENT_H

#include "rabstracteditorreport.h"
#include <QModelIndex>

class RCashMovement : public RAbstractEditorReport
{
    Q_OBJECT
public:
    RCashMovement(const QString &title, QIcon icon, const QString &editorName);

    QToolBar* toolBar() override;

protected:
    void newData() override;

private:
    void openCashTransfer();

protected slots:
    void on_tbl_doubleClicked(const QModelIndex &index) override;
};

#endif // RCASHMOVEMENT_H
