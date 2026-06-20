#ifndef MENUCACHE_H
#define MENUCACHE_H

#include "menutypes.h"

#include <QObject>
#include <QString>
#include <functional>

class MenuCache : public QObject
{
    Q_OBJECT

public:
    static MenuCache &instance();

    static void configureNetwork();

    QString cacheDir() const;
    QString menuJsonPath() const;

    bool isReady() const { return m_ready; }
    QString lastError() const { return m_lastError; }

    void preload(QObject *context, std::function<void(bool ok)> finished);

    QVector<MenuGroup> groups() const { return m_groups; }
    QVector<MenuDish> dishesByGroup(int groupId) const;
    QVector<MenuDish> popularDishes() const;
    MenuDish dishById(int dishId) const;

    QString groupImageFile(int groupId) const;
    QString dishImageFile(int dishId) const;

private:
    explicit MenuCache(QObject *parent = nullptr);

    bool clearCache() const;
    bool ensureCacheDirs() const;
    bool writeMenuJson(const QJsonObject &doc) const;
    bool saveMenuImage(const QString &payload, int id, bool isGroup, QString &outPath) const;
    QString sessionKey() const;
    bool ingestMenuResponse(const QJsonObject &jdoc);

    bool m_ready = false;
    QString m_lastError;
    QVector<MenuGroup> m_groups;
    QVector<MenuDish> m_dishes;
};

#endif // MENUCACHE_H
