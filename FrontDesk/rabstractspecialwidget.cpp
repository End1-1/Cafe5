#include "rabstractspecialwidget.h"
#include "logwriter.h"
#include <QJsonDocument>

RAbstractSpecialWidget::RAbstractSpecialWidget(QWidget *parent)
    : C5Widget(parent)
{
}

QJsonArray RAbstractSpecialWidget::getValues()
{
    return {};
}

void RAbstractSpecialWidget::setup(const QJsonObject &jdoc)
{
    LogWriterVerbose("setup rabstractspecialwidget", QJsonDocument(jdoc).toJson());
}

void RAbstractSpecialWidget::save()
{
    emit goAccept();
}
