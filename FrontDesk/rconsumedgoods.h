#pragma once

#include "rabstracteditorreport.h"

class RConsumedGoods : public RAbstractEditorReport
{
    Q_OBJECT
public:
    RConsumedGoods(const QString &title, QIcon icon, const QString &editorName);

    QToolBar *toolBar() override;

private:
    void recalculateStoreOutput();

    bool syncQuery(const QString &route, const QJsonObject &params, QJsonObject &response, QString &error);
};
