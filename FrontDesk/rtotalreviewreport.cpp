#include "rtotalreviewreport.h"
#include "rcashmovement.h"
#include <QJsonDocument>
#include "c5config.h"
#include "c5mainwindow.h"

namespace {

void openDrillDownReport(const QString &editorName,
                         const QString &title,
                         const QIcon &icon,
                         const QJsonArray &filterValues,
                         bool cashMovementReport)
{
    const QString key = QStringLiteral("filter_values_") + editorName;
    const QString prevFilter = __c5config.getRegValue(key, QString()).toString();
    __c5config.setRegValue(key, QJsonDocument(filterValues).toJson(QJsonDocument::Compact));

    RAbstractEditorReport *report = nullptr;
    if(cashMovementReport) {
        report = new RCashMovement(title, icon, editorName);
    } else {
        report = new RAbstractEditorReport(title, icon, editorName);
    }

    __c5config.setRegValue(key, prevFilter);
    __mainWindow->addWidget(report);
}

} // namespace

RTotalReviewReport::RTotalReviewReport(const QString &title, QIcon icon, const QString &editorName)
    : RAbstractEditorReport(title, icon, editorName)
{}

void RTotalReviewReport::on_tbl_doubleClicked(const QModelIndex &index)
{
    if(!index.isValid()) {
        return;
    }

    const QString date = index.siblingAtColumn(0).data(Qt::DisplayRole).toString();
    if(date.isEmpty()) {
        return;
    }

    QString editorName;
    QString title;
    QJsonArray filterValues;
    bool cashMovementReport = false;

    if(index.column() == 1) {
        editorName = QStringLiteral("form_cashsessions");
        title = tr("Cash sessions");
        filterValues = QJsonArray{
            QJsonObject{{QStringLiteral("datemode"), 1}},
            QJsonObject{{QStringLiteral("date1"), date}},
            QJsonObject{{QStringLiteral("date2"), date}}
        };
    } else if(index.column() == 2) {
        editorName = QStringLiteral("form_sold_items");
        title = tr("Sold items");
        filterValues = QJsonArray{
            QJsonObject{{QStringLiteral("summarize"), 0}},
            QJsonObject{{QStringLiteral("date1"), date}},
            QJsonObject{{QStringLiteral("date2"), date}}
        };
    } else if(index.column() == reportColumnFromEnd(-2)) {
        editorName = QStringLiteral("form_revenue");
        title = tr("Revenue");
        cashMovementReport = true;
        filterValues = QJsonArray{
            QJsonObject{{QStringLiteral("viewmode"), 2}},
            QJsonObject{{QStringLiteral("date1"), date}},
            QJsonObject{{QStringLiteral("date2"), date}},
            QJsonObject{{QStringLiteral("currency"), 1}},
            QJsonObject{{QStringLiteral("other_expenses"), 1}}
        };
    } else {
        return;
    }

    openDrillDownReport(editorName, title, fIcon, filterValues, cashMovementReport);
}
