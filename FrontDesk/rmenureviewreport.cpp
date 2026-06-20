#include "rmenureviewreport.h"
#include "c5config.h"
#include "c5mainwindow.h"
#include "c5tablewidget.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QTreeView>

namespace {

QString menuReviewTreeStyleSheet()
{
    return QStringLiteral(
        "QTreeView#mTreeView {"
        "    border: none;"
        "    background-color: #fafcfe;"
        "    alternate-background-color: #f4f7fb;"
        "}"
        "QTreeView#mTreeView::item {"
        "    border-right: 1px solid #e2eaf4;"
        "    border-bottom: 1px solid #e2eaf4;"
        "    color: #1a2533;"
        "    padding: 4px 8px;"
        "}"
        "QTreeView#mTreeView::item:selected {"
        "    background-color: rgba(99, 132, 232, 0.18);"
        "    color: #1a2533;"
        "}"
        "QTreeView#mTreeView::item:hover:!selected {"
        "    background-color: rgba(99, 132, 232, 0.08);"
        "}"
        "QHeaderView::section {"
        "    background-color: #eef2f8;"
        "    color: #1a2533;"
        "    border: none;"
        "    border-right: 1px solid #e2eaf4;"
        "    border-bottom: 1px solid #d0dae6;"
        "    padding: 5px 10px;"
        "}");
}

QString menuReviewTotalStyleSheet()
{
    return QStringLiteral(
        "C5TableWidget#tblTotal {"
        "    border-top: 1px solid #d0dae6;"
        "    background-color: #f4f7fb;"
        "}");
}

QJsonArray insertSpacerRows(const QJsonArray &rows)
{
    if(rows.isEmpty()) {
        return rows;
    }

    const int columnCount = rows.at(0).toObject().value(QStringLiteral("data")).toArray().size();
    if(columnCount <= 0) {
        return rows;
    }

    QJsonArray result;

    for(int i = 0; i < rows.size(); ++i) {
        result.append(rows.at(i));

        if(i + 1 >= rows.size()) {
            continue;
        }

        QJsonArray emptyData;

        for(int c = 0; c < columnCount; ++c) {
            emptyData.append(QString());
        }

        QJsonObject spacer;
        spacer.insert(QStringLiteral("data"), emptyData);
        spacer.insert(QStringLiteral("children"), QJsonArray());
        spacer.insert(QStringLiteral("spacer"), true);
        result.append(spacer);
    }

    return result;
}

void applyMenuReviewTableStyle(QWidget *widget)
{
    if(auto *tree = widget->findChild<QTreeView*>(QStringLiteral("mTreeView"))) {
        tree->setAlternatingRowColors(true);
        tree->setUniformRowHeights(false);
        tree->setSortingEnabled(false);
        tree->setStyleSheet(menuReviewTreeStyleSheet());
    }

    if(auto *total = widget->findChild<C5TableWidget*>(QStringLiteral("tblTotal"))) {
        total->setStyleSheet(menuReviewTotalStyleSheet());
    }
}

} // namespace

RMenuReviewReport::RMenuReviewReport(C5User *user, const QString &title, QIcon icon, QWidget *parent)
    : NTreeWidget(user, QStringLiteral("/engine/v2/reports/menu-review/get"), parent)
{
    fLabel = title;
    fIcon = icon;
    mMainWindow = __mainWindow;
    mHost = __c5config.dbParams().at(1);
    applyMenuReviewTableStyle(this);
    initParams({});
    query();
}

void RMenuReviewReport::queryFinished(const QJsonObject &ba)
{
    QJsonObject jo = ba;

    if(!jo.value(QStringLiteral("childcols")).toArray().isEmpty()) {
        jo.insert(QStringLiteral("rows"), insertSpacerRows(jo.value(QStringLiteral("rows")).toArray()));
    }

    NTreeWidget::queryFinished(jo);
}
