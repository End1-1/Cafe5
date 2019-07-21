#ifndef WIDGETCONTAINER_H
#define WIDGETCONTAINER_H

#include <QWidget>
#include <QMap>

class WidgetContainer : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetContainer(QWidget *parent = nullptr);
    void getWatchList(QWidget *c);
    bool hasChanges();
    void saveChanges();
    int getInt(const QString &name) const;

private:
    QList<QWidget*> fWatchList;
    QMap<QString, QWidget*> fWidgetsList;
    QMap<QString, int> fWidgetsTypes;
};

#endif // WIDGETCONTAINER_H
