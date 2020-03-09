#include "config.h"
#include <QSettings>
#include <QCryptographicHash>

QString APPDIR;
QString DBHOST;
QString DBFILE;
QString DBUSER;
QString DBPASSWORD;
QString ADMINPASS = "4a7d1ed414474e4033ac29ccb8653d9b"; //MD5(0000)

void INITCONFIG()
{
    QSettings s(_ORGANIZATION_, _APPLICATION_+ QString("\\") + _MODULE_);
    DBHOST = s.value("dbhost", "").toString();
    DBFILE = s.value("dbfile", "").toString();
    DBUSER = s.value("dbuser", "").toString();
    DBPASSWORD = s.value("dbpass", "").toString();
    if (!DBPASSWORD.isEmpty()) {
        ADMINPASS = QCryptographicHash::hash(DBPASSWORD.toUtf8(), QCryptographicHash::Md5).toHex().toLower();
    }
}
