#ifndef MAGNIT_H
#define MAGNIT_H

#include <QJsonObject>
#include <QStringList>

#ifdef BUILDING_MAGNIT_DLL
    #define MAGNIT_EXPORT Q_DECL_EXPORT
#else
    #define MAGNIT_EXPORT Q_DECL_IMPORT
#endif

extern "C" MAGNIT_EXPORT bool armsoft(const QJsonObject &jreq, QJsonObject &jret, QString &err);

#endif // MAGNIT_H
