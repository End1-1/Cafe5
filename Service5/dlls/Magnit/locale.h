#ifndef LOCALE_H
#define LOCALE_H

#include <QString>

static const char* lkInvalidAPIKey = "Invalid server API key";
static const char* lkUnknownProtocol = "Unknown protocol";
static const char* lkDeviceNotRegistered = "Device doesn't registered";
static const char* lkInvalidPin = "Invalid pin";
static const char* lkInvalidHash = "Invalid password hash";
static const char* lkInvalidOrderId = "Invalid order id";
static const char* lkInvalidCredentials = "Invalid credentials";

#define lk(v) Locale::getStringOf(const QString &key)

class Locale
{
public:
    Locale();
    static QString getStringOf(const QString &key);
};

#endif // LOCALE_H
