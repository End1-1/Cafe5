#include "c5htmlprint.h"
#include <QApplication>
#include <QFile>

QString loadTemplate(const QString &fileName)
{
    QString fullName = QCoreApplication::applicationDirPath() + "/templates/" + fileName;
    QFile f(fullName);

    if(!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }

    return QString::fromUtf8(f.readAll());
}

QString applyTemplate(QString html, const QMap<QString, QString>& vars)
{
    for(auto it = vars.begin(); it != vars.end(); ++it) {
        html.replace("{{" + it.key() + "}}", it.value());
    }

    return html;
}

QString htmlEscape(QString s)
{
    s.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")
     .replace("\"", "&quot;").replace("'", "&#39;");
    s.replace("\n", "<br>");
    return s;
}
