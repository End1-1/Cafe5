#ifndef CHARTDATESCENE_H
#define CHARTDATESCENE_H

#include <QGraphicsScene>

class ChartDateScene : public QGraphicsScene
{
public:
    ChartDateScene(QObject *parent = nullptr);
    void setDateRange(const QDate &d1, const QDate &d2);
};

#endif // CHARTDATESCENE_H
