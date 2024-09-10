#ifndef C5TABLEDATA_H
#define C5TABLEDATA_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>

#define tds(t, f, i) C5TableData::instance()->string(t, f, i)
#define objs(t) C5TableData::instance()->objects(t)

class C5TableData : public QObject
{
    Q_OBJECT
public:
    explicit C5TableData(QObject *parent = nullptr);

    inline QJsonObject jsonObject(const QString &table, const QString &id) const;

    QVariant variant(const QString &table, const QString &field, int id);

    inline QString string(const QString &table, const QString &field, int id)
    {
        return variant(table, field, id).toString();
    }

    QJsonArray objects(const QString &table);

    QJsonObject data;

    void setData(const QJsonObject &d);

    static C5TableData *instance();

    int version();

    QStringList ids(const QString &table);

    QJsonArray dishes(int menu, int part2);

    QJsonArray part2(int menu, int part1, int parentid);

    QMap<QString, int> cashNames();

    void setStopList(const QJsonArray &ja);

    QMap<int, double> mStopList;
private:
    static C5TableData *mInstance;

    QMap<QString, QJsonArray> mMenuDish;

    QMap<QString, QJsonArray> mPart2List;

};

#endif // C5TABLEDATA_H
