#include "menucache.h"

#include "menuhelpers.h"
#include "ninterface.h"
#include "ndataprovider.h"
#include "serverconfig.h"
#include "version.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCoreApplication>
#include "selfboardsettings.h"

#include <QSettings>
#include <QStandardPaths>
#include <QDebug>

namespace {

constexpr auto kPlaceholderGroup = ":/res/dish_placeholder.png";
constexpr auto kPlaceholderDish = ":/res/dish_placeholder.png";

QString appFileVersion()
{
    return QStringLiteral("%1.%2.%3.%4")
        .arg(VER_MAJOR)
        .arg(VER_MINOR)
        .arg(VER_PATCH)
        .arg(VER_BUILD);
}

bool looksLikeImage(const QByteArray &raw)
{
    if (raw.size() < 4) {
        return false;
    }
    const auto b0 = static_cast<unsigned char>(raw.at(0));
    const auto b1 = static_cast<unsigned char>(raw.at(1));
    const auto b2 = static_cast<unsigned char>(raw.size() > 2 ? raw.at(2) : '\0');
    const auto b3 = static_cast<unsigned char>(raw.size() > 3 ? raw.at(3) : '\0');
    if (raw.startsWith("\x89PNG\r\n\x1a\n")) {
        return true;
    }
    if (b0 == 0xFF && b1 == 0xD8) {
        return true;
    }
    if (b0 == 'G' && b1 == 'I' && b2 == 'F') {
        return true;
    }
    if (raw.size() >= 12 && raw.startsWith("RIFF") && raw.mid(8, 4) == "WEBP") {
        return true;
    }
    if (b0 == 'B' && b1 == 'M') {
        return true;
    }
    return false;
}

QString cleanedBase64Text(QString text)
{
    text = text.trimmed();
    if (text.startsWith(QChar(0xFEFF))) {
        text.remove(0, 1);
    }
    const int comma = text.indexOf(QLatin1Char(','));
    if (text.startsWith(QStringLiteral("data:"), Qt::CaseInsensitive) && comma >= 0) {
        text = text.mid(comma + 1);
    }
    text.remove(QLatin1Char(' '));
    text.remove(QLatin1Char('\n'));
    text.remove(QLatin1Char('\r'));
    text.remove(QLatin1Char('\t'));
    return text;
}

QByteArray decodeBase64Padded(const QString &text)
{
    QByteArray b64 = text.toLatin1();
    const int mod = b64.size() % 4;
    if (mod != 0) {
        b64.append(QByteArray(4 - mod, '='));
    }
    return QByteArray::fromBase64(b64);
}

QByteArray decodeImagePayload(const QString &payload)
{
    if (payload.trimmed().isEmpty()) {
        return {};
    }

    const QString text = cleanedBase64Text(payload);

    // Same approach as CE5GoodsGroup::applyGroup — loadFromData after base64 decode.
    const QByteArray fromBase64 = decodeBase64Padded(text);
    if (!fromBase64.isEmpty()) {
        QImage img;
        if (img.loadFromData(fromBase64)) {
            return fromBase64;
        }
        if (looksLikeImage(fromBase64)) {
            return fromBase64;
        }
    }

    const QByteArray raw = text.toUtf8();
    if (looksLikeImage(raw)) {
        return raw;
    }

    return {};
}

QString imageExtensionForPayload(const QByteArray &raw)
{
    if (raw.startsWith("\x89PNG\r\n\x1a\n")) {
        return QStringLiteral("png");
    }
    if (raw.size() >= 2
        && static_cast<unsigned char>(raw.at(0)) == 0xFF
        && static_cast<unsigned char>(raw.at(1)) == 0xD8) {
        return QStringLiteral("jpg");
    }
    if (raw.startsWith("GIF8")) {
        return QStringLiteral("gif");
    }
    if (raw.size() >= 2) {
        const auto b0 = static_cast<unsigned char>(raw.at(0));
        const auto b1 = static_cast<unsigned char>(raw.at(1));
        if (b0 == 'B' && b1 == 'M') {
            return QStringLiteral("bmp");
        }
    }
    if (raw.size() >= 12 && raw.startsWith("RIFF") && raw.mid(8, 4) == "WEBP") {
        return QStringLiteral("webp");
    }
    return QStringLiteral("jpg");
}

} // namespace

MenuCache &MenuCache::instance()
{
    static MenuCache cache;
    return cache;
}

MenuCache::MenuCache(QObject *parent)
    : QObject(parent)
{
}

void MenuCache::configureNetwork()
{
    ServerConfig::loadFromSettings();
    ServerConfig::applyToNetwork();
    QSettings settings = SelfBoardSettings::store();
    NDataProvider::mDebug = settings.value(QStringLiteral("httpDebug"), false).toBool();
}

QString MenuCache::cacheDir() const
{
    return QStandardPaths::writableLocation(QStandardPaths::TempLocation)
        + QStringLiteral("/SelfBoard/menu");
}

QString MenuCache::menuJsonPath() const
{
    return cacheDir() + QStringLiteral("/menu.json");
}

QString MenuCache::sessionKey() const
{
    return NDataProvider::sessionKey;
}

QString MenuCache::groupImageFile(int groupId) const
{
    return cacheDir() + QStringLiteral("/images/groups/") + QString::number(groupId) + QStringLiteral(".png");
}

QString MenuCache::dishImageFile(int dishId) const
{
    return cacheDir() + QStringLiteral("/images/dishes/") + QString::number(dishId) + QStringLiteral(".jpg");
}

bool MenuCache::clearCache() const
{
    QDir root(cacheDir());
    if (!root.exists()) {
        return true;
    }
    return root.removeRecursively();
}

bool MenuCache::ensureCacheDirs() const
{
    QDir root(cacheDir());
    if (!root.exists() && !root.mkpath(QStringLiteral("."))) {
        return false;
    }
    return root.mkpath(QStringLiteral("images/groups"))
        && root.mkpath(QStringLiteral("images/dishes"));
}

bool MenuCache::saveMenuImage(const QString &payload, int id, bool isGroup, QString &outPath) const
{
    if (payload.trimmed().isEmpty() || id <= 0) {
        outPath = QString::fromUtf8(isGroup ? kPlaceholderGroup : kPlaceholderDish);
        return false;
    }

    const QByteArray raw = decodeImagePayload(payload);
    if (raw.isEmpty()) {
        if (NDataProvider::mDebug) {
            qWarning() << "MenuCache: decode failed for"
                       << (isGroup ? "group" : "dish") << id
                       << "payload bytes:" << payload.size();
        }
        outPath = QString::fromUtf8(isGroup ? kPlaceholderGroup : kPlaceholderDish);
        return false;
    }

    const QString subDir = isGroup ? QStringLiteral("images/groups/")
                                   : QStringLiteral("images/dishes/");
    const QString ext = isGroup ? QStringLiteral("png") : imageExtensionForPayload(raw);
    const QString path = cacheDir() + QLatin1Char('/') + subDir + QString::number(id)
        + QLatin1Char('.') + ext;

    const QDir parentDir = QFileInfo(path).dir();
    if (!parentDir.exists() && !parentDir.mkpath(QStringLiteral("."))) {
        if (NDataProvider::mDebug) {
            qWarning() << "MenuCache: cannot create dir" << parentDir.absolutePath();
        }
        outPath = QString::fromUtf8(isGroup ? kPlaceholderGroup : kPlaceholderDish);
        return false;
    }

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (NDataProvider::mDebug) {
            qWarning() << "MenuCache: cannot write" << path << file.errorString();
        }
        outPath = QString::fromUtf8(isGroup ? kPlaceholderGroup : kPlaceholderDish);
        return false;
    }
    if (file.write(raw) != raw.size()) {
        outPath = QString::fromUtf8(isGroup ? kPlaceholderGroup : kPlaceholderDish);
        return false;
    }
    file.close();

    outPath = QDir::toNativeSeparators(path);
    return true;
}

bool MenuCache::writeMenuJson(const QJsonObject &doc) const
{
    if (!ensureCacheDirs()) {
        return false;
    }
    QFile file(menuJsonPath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }
    return file.write(QJsonDocument(doc).toJson(QJsonDocument::Compact)) > 0;
}

bool MenuCache::ingestMenuResponse(const QJsonObject &jdoc)
{
    if (jdoc.value(QStringLiteral("status")).toInt() != 1) {
        m_lastError = jdoc.value(QStringLiteral("message")).toString();
        if (m_lastError.isEmpty()) {
            m_lastError = QStringLiteral("Menu request failed");
        }
        return false;
    }

    if (!ensureCacheDirs()) {
        m_lastError = QStringLiteral("Cannot create cache directory");
        return false;
    }

    m_groups.clear();
    m_dishes.clear();

    QJsonObject stored;
    stored.insert(QStringLiteral("groups"), jdoc.value(QStringLiteral("groups")));
    stored.insert(QStringLiteral("dishes"), jdoc.value(QStringLiteral("dishes")));

    const QJsonArray groups = jdoc.value(QStringLiteral("groups")).toArray();
    for (const QJsonValue &gv : groups) {
        const QJsonObject g = gv.toObject();
        const int id = g.value(QStringLiteral("f_id")).toInt();
        MenuGroup group;
        group.id = id;
        group.name = g.value(QStringLiteral("f_name")).toString();

        saveMenuImage(g.value(QStringLiteral("f_image")).toString(), id, true, group.iconPath);
        m_groups.append(group);
    }

    const QJsonArray dishes = jdoc.value(QStringLiteral("dishes")).toArray();
    for (const QJsonValue &dv : dishes) {
        const QJsonObject d = dv.toObject();
        MenuDish dish;
        MenuHelpers::fillDishFromMenuRow(dish, d);

        saveMenuImage(d.value(QStringLiteral("f_image")).toString(), dish.id, false, dish.imagePath);

        if (dish.isPackage()) {
            const QJsonObject packageDataObj =
                MenuHelpers::parseGoodsDataObject(d.value(QStringLiteral("f_data")));
            dish.modificators = MenuHelpers::parseModificators(packageDataObj);
            dish.relatedDrinks = MenuHelpers::parseRelatedItems(packageDataObj.value(QStringLiteral("f_related_drink")));
            dish.relatedOther = MenuHelpers::parseRelatedItems(packageDataObj.value(QStringLiteral("f_related_other")));

            const QJsonArray complect = d.value(QStringLiteral("f_complectation")).toArray();
            for (const QJsonValue &cv : complect) {
                const QJsonObject row = cv.toObject();
                const int componentId = row.value(QStringLiteral("f_goods")).toInt();
                QString componentImagePath;
                saveMenuImage(row.value(QStringLiteral("f_image")).toString(), componentId, false, componentImagePath);
                dish.packageComponents.append(
                    MenuHelpers::parsePackageComponent(row, componentImagePath));
            }
        }

        m_dishes.append(dish);
    }

    if (!writeMenuJson(stored)) {
        m_lastError = QStringLiteral("Cannot write menu cache");
        return false;
    }

    m_ready = true;
    m_lastError.clear();
    return true;
}

void MenuCache::preload(QObject *context, std::function<void(bool ok)> finished)
{
    m_ready = false;
    m_lastError.clear();
    m_groups.clear();
    m_dishes.clear();

    if (!clearCache()) {
        m_lastError = QCoreApplication::translate("MenuCache", "Cannot clear menu cache");
        finished(false);
        return;
    }

    configureNetwork();

    if (!ServerConfig::isConfigured()) {
        m_lastError = QCoreApplication::translate("MenuCache", "Configure server address and login in settings");
        finished(false);
        return;
    }

    const auto fetchMenu = [this, context, finished]() {
        QSettings settings = SelfBoardSettings::store();
        const QString locale = settings.value(QStringLiteral("locale"), QStringLiteral("en")).toString();

        NInterface::query(
            QStringLiteral("/engine/v2/waiter/menu/get"),
            sessionKey(),
            context,
            QJsonObject{{QStringLiteral("locale"), locale}},
            [this, finished](const QJsonObject &jdoc) {
                const bool ok = ingestMenuResponse(jdoc);
                finished(ok);
            },
            [this, finished](const QJsonObject &jerr) -> bool {
                m_lastError = jerr.value(QStringLiteral("errorMessage")).toString();
                if (m_lastError.isEmpty()) {
                    m_lastError = QStringLiteral("Menu request failed");
                }
                finished(false);
                return true;
            },
            false,
            120000,
            true);
    };

    if (ServerConfig::hasSession()) {
        fetchMenu();
        return;
    }

    ServerConfig::login(context, [this, fetchMenu, finished](bool ok, const QString &error) {
        if (!ok) {
            m_lastError = error;
            finished(false);
            return;
        }
        fetchMenu();
    });
}

QVector<MenuDish> MenuCache::dishesByGroup(int groupId) const
{
    QVector<MenuDish> result;
    for (const MenuDish &dish : m_dishes) {
        if (dish.groupId == groupId) {
            result.append(dish);
        }
    }
    return result;
}

QVector<MenuDish> MenuCache::popularDishes() const
{
    QVector<MenuDish> result;
    for (const MenuDish &dish : m_dishes) {
        if (dish.popular) {
            result.append(dish);
        }
    }
    if (result.size() >= 3) {
        return result.mid(0, 3);
    }
    return result.mid(0, qMin(3, result.size()));
}

MenuDish MenuCache::dishById(int dishId) const
{
    for (const MenuDish &dish : m_dishes) {
        if (dish.id == dishId) {
            return dish;
        }
    }
    return {};
}
