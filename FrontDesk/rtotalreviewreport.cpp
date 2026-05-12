#include "rtotalreviewreport.h"
#include <QJsonDocument>
#include "c5config.h"
#include "c5mainwindow.h"

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
    QString key;
    QJsonArray filterValues;

    if(index.column() == 1) {
        editorName = "form_cashsessions";
        title = tr("Cash sessions");
        key = QStringLiteral("filter_values_form_cashsessions");
        filterValues = QJsonArray{
            QJsonObject{{"datemode", 1}},
            QJsonObject{{"date1", date}},
            QJsonObject{{"date2", date}}
        };
    } else if(index.column() == 2) {
        editorName = "form_sold_items";
        title = tr("Sold items");
        key = QStringLiteral("filter_values_form_sold_items");
        filterValues = QJsonArray{
            QJsonObject{{"summarize", 1}},
            QJsonObject{{"date1", date}},
            QJsonObject{{"date2", date}}
        };
    } else {
        return;
    }

    const QString prevFilter = __c5config.getRegValue(key, "").toString();
    __c5config.setRegValue(key, QJsonDocument(filterValues).toJson(QJsonDocument::Compact));
    auto *report = new RAbstractEditorReport(title, fIcon, editorName);
    __c5config.setRegValue(key, prevFilter);
    __mainWindow->addWidget(report);
}
