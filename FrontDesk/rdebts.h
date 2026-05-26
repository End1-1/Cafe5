#ifndef RDEBTS_H
#define RDEBTS_H

#include "rabstracteditorreport.h"

class QToolBar;

class RDebts : public RAbstractEditorReport
{
    Q_OBJECT
public:
    RDebts(const QString &title, QIcon icon, const QString &editorName);

    QToolBar *toolBar() override;

    void reloadDebts();

private:
    void redeemDebt();

    bool mDebtRedeemActionInserted = false;
};

#endif // RDEBTS_H
