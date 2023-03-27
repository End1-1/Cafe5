#ifndef C5CACHE_H
#define C5CACHE_H

#include <QObject>
#include <QMap>
#include <QVariant>

#define cache_users_groups 1
#define cache_users_states 2
#define cache_dish_part1 3
#define cache_dish_part2 4
#define cache_goods_unit 5
#define cache_goods_group 6
#define cache_goods 7
#define cache_goods_store 8
#define cache_goods_partners 9
#define cache_store_inout 10
#define cache_doc_state 11
#define cache_doc_type 12
#define cache_users 14
#define cache_waiter_printers 15
#define cache_dish 17
#define cache_goods_waste 18
#define cache_credit_card 19
#define cache_dish_remove_reason 20
#define cache_settings_names 21
#define cache_hall_list 22
#define cache_discount_cards 24
#define cache_dish_comments 25
#define cache_menu_names 26
#define cache_s_db 27
#define cache_halls 28
#define cache_tables 29
#define cache_dish_state 30
#define cache_order_state 31
#define cache_cash_names 32
#define cache_store_reason 33
#define cache_discount_type 34
#define cache_header_payment 35
#define cache_header_paid 36
#define cache_dish_package 37
#define cache_salary_shift 38
#define cache_goods_classes 39
#define cache_dish_menu_state 40
#define cache_order_mark 41
#define cache_mf_actions 42
#define cache_mf_process 43
#define cache_mf_products 44
#define cache_mf_workshop 45
#define cache_mf_action_stage 46
#define cache_mf_active_task 47
#define cache_mf_task_state 48
#define cache_currency 49
#define cache_currency_rate 50
#define cache_currency_cross_rate 51
#define cache_currency_cross_rate_history 52

class C5Cache : public QObject
{
    Q_OBJECT

public:
    C5Cache(const QStringList &dbParams);

    inline QString getString(int row, int column) {return fCacheData.at(row).at(column).toString();}

    inline QList<QVariant> getRow(int row) {return fCacheData.at(row);}

    inline QList<QVariant> getValuesForId(int id) {return find(id) > -1 ? getRow(find(id)) : QList<QVariant>(); }

    QList<QVariant> getJoinedColumn(const QString &columnName);

    QString getString(int id);

    QString getString(int row, const QString &columnName);

    inline int getInt(int row, int column) {return fCacheData.at(row).at(column).toInt();}

    inline int rowCount() {return fCacheData.count();}

    inline int find(int id) {return fCacheIdRow.contains(id) ? fCacheIdRow[id] : -1;}

    void refresh();

    static C5Cache *cache(const QStringList &dbParams, int cacheId);

    static QMap<QString, C5Cache*> fCacheList;

    static int idForTable(const QString &table) {return fTableCache[table];}

    static QString cacheName(const QStringList &dbParams, int cacheId);

    static void resetCache(const QStringList &dbParams, const QString &table);

    QString query(int cacheId);

protected:
    QList<QList<QVariant> > fCacheData;

    QMap<int, int> fCacheIdRow;

    virtual void loadFromDatabase(const QString &query);

private:
    static QMap<int, QString> fCacheQuery;

    static QMap<QString, int> fTableCache;

    static QMap<int, QHash<QString, int> > fCacheColumns;

    int fId;

    int fVersion;

    QStringList fDBParams;

    void setCacheSimpleQuery(int cacheId, const QString &table);
};

#endif // C5CACHE_H
