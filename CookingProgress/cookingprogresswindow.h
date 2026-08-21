#ifndef COOKINGPROGRESSWINDOW_H
#define COOKINGPROGRESSWINDOW_H

#include "c5dialog.h"
#include <QJsonArray>
#include <QSet>
#include <QTimer>

namespace Ui
{
class CookingProgressWindow;
}

class C5User;
class QVBoxLayout;

class CookingProgressWindow : public C5Dialog
{
    Q_OBJECT

public:
    explicit CookingProgressWindow(C5User *user, QWidget *parent = nullptr);
    ~CookingProgressWindow() override;

protected:
    void showEvent(QShowEvent *e) override;

private slots:
    void on_btnExit_clicked();
    void pollQueue();

private:
    Ui::CookingProgressWindow *ui;
    QTimer mPollTimer;
    QJsonArray mOrders;
    QSet<QString> mReadyOrderIds;
    bool mReadyTrackingStarted = false;

    void clearLayout(QVBoxLayout *layout);
    void rebuildBoards();
    QWidget *makeOrderCard(const QJsonObject &order, bool readySide);
};

#endif // COOKINGPROGRESSWINDOW_H
