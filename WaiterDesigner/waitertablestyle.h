#pragma once

#include <QColor>
#include <QString>

class QTableWidget;

struct WaiterTblTablesStyle
{
    int borderWidth = 1;
    QColor gridColor = QColor(QStringLiteral("#6b7a84"));
    QColor colorEmpty = QColor(Qt::white);
    QColor colorOrder = QColor(200, 247, 197);
    QColor colorPrecheck = QColor(247, 197, 197);
    QColor colorEmptyText = QColor(Qt::black);
    QColor colorOrderText = QColor(Qt::black);
    QColor colorPrecheckText = QColor(Qt::black);

    bool loadFromCssFile(const QString &path, QString *error = nullptr);
    bool saveToCssFile(const QString &path, QString *error = nullptr) const;

    void applyToTableWidget(QTableWidget *table) const;
    QString hallTableStylesheet() const;

    static QString cssFilePath();
    static bool loadAtStartup(QString *error = nullptr);
    static const WaiterTblTablesStyle &cachedTblStyle();
    static void applyToTable(QTableWidget *table);
};
