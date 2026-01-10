#pragma once

#include <QString>

extern QString loadTemplate(const QString &fileName) ;
extern QString applyTemplate(QString html, const QMap<QString, QString>& vars);
extern QString htmlEscape(QString s);
