#ifndef RORDERINPROGRESS_H
#define RORDERINPROGRESS_H

#include "rabstracteditorreport.h"
#include <QModelIndex>
#include <QTimer>

class ROrderInProgress : public RAbstractEditorReport
{
    Q_OBJECT

public:
    ROrderInProgress(const QString &title, QIcon icon, const QString &editorName);

protected:
    void showEvent(QShowEvent *e) override;
    void hideEvent(QHideEvent *e) override;

    void applyFilter() override;

protected slots:
    void on_tbl_doubleClicked(const QModelIndex &index) override;

private slots:
    void onTableModelReset();

private:
    bool isLiveQueueMode() const;

    void startAutoReload();
    void stopAutoReload();

    QTimer mReloadTimer;
};

#endif // RORDERINPROGRESS_H
