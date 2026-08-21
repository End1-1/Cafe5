#pragma once

#include "c5dialog.h"
#include <QJsonArray>
#include <QStringList>

namespace Ui
{
class DlgRecentDishes;
}

class DlgRecentDishes : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgRecentDishes(C5User *user, int minutes, QWidget *parent = nullptr);
    ~DlgRecentDishes() override;

    static void open(C5User *user, int minutes, QWidget *parent = nullptr);

private slots:
    void on_btnClose_clicked();
    void stationClicked();

private:
    Ui::DlgRecentDishes *ui;
    int mMinutes = 40;
    QString mCurrentStation;
    QJsonArray mRows;
    QStringList mStations;

    void loadData();
    void buildStationButtons();
    void showStation(const QString &station);
};
