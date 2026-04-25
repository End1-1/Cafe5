#ifndef DATAHELPER_H
#define DATAHELPER_H

#include <QJsonArray>
#include <QJsonObject>

class QWidget;

class DataHelper
{
public:
    DataHelper();

    void setWidgetData(QWidget *dataWidget, const QJsonObject &jdoc);

    /// Собирает поля с непустым \c Field; результат — в \c fieldSavePayload().
    bool saveData(QWidget *dataWidget);

    QJsonArray fieldSavePayload() const { return m_fieldSavePayload; }

private:
    QJsonArray m_fieldSavePayload;
};

#endif // DATAHELPER_H
